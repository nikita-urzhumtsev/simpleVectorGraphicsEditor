#include "rectangleitem.h"
#include <QtMath>

#include "mainwindow.h" //TODO там пока(!!!) живут глобальные данные - непорядок

RectangleItem::RectangleItem(QObject *parent):
  MovableItem(parent), mousePressStartPos(0,0)
{
    width=120;          // ширина прямоугольника
    height=60;          // высота прямоугольника
    leftBaseCorner=-width/2; // левый/верхний угол привязки прямоугольника
    topBaseCorner=-height/2;

    angle=currentAngle;           // угол поворта прямоугольника в градусах

    isRotatingNow=false;          // флаг состояния вращения в настоящий момент
    widthIsChangingNow=false;     // флаг состояния изменения ширины в настоящий момент
    heightIsChangingNow=false;    // флаг состояния изменения высоты в настоящий момент


    hotPoints.set(width,height,angle, getLineWidth()); // вычисляю горячие точки и boundingRect

}

RectangleItem::~RectangleItem()
{
}

void HotPoints::set(int width, int height, int angle, int borderWidth)
{   // TODO правильно посчитать объемлющий прямоугольник
    // чистая математика, надо приложить картинку, поясняющую формулы
    qreal Sin, Cos;
    Sin=qSin(qDegreesToRadians(static_cast<double>(angle)));
    Cos=qCos(qDegreesToRadians(static_cast<double>(angle)));
    Ax=0.5*width*Cos;  // Точки А и (-А) - середина одной стороны (и параллельной ей)
    Ay=0.5*width*Sin;
    Dx=0.5*height*Sin; // Точка D и (-D) - середина второй стороны (и параллельной ей)
    Dy=-0.5*height*Cos;

    Bx=Ax+0.5*height*Sin; // Точка B - один угол, (-B) - угол по диагонали от первого
    By=Ay-0.5*height*Cos;
    Cx=Ax-0.5*height*Sin; // Точка C - второй угол, (-С) - угол по диагонали от второго
    Cy=Ay+0.5*height*Cos;

    visibleRadius = borderWidth<fixedMark ? borderWidth : fixedMark; // горячая точка не должна быть слишком большой
    if (visibleRadius==1) visibleRadius=2;

    radius=10;//visibleRadius+fixedMark; // зона захвата мыши пусть будет примерно в 2 раза больше, чем видимая точка, так ее будет легче поймать
}

//////////////////-------------------------------
///
///
/// -------------------------------------------
qreal vectorLength( const QPointF & A)
{   qreal x1=A.x(), y1=A.y();
    return qSqrt(x1 * x1 + y1 * y1);
}

//расчет угла между двумерными векторами - нужен для расчета угла поворота
int angleBetweenVectors( const QPointF & A, const QPointF & B)
{   qreal x1=A.x(), y1=A.y(), x2=B.x(), y2=B.y();
    qreal t = ( x1 * x2 + y1 * y2 ) / ( vectorLength(A) * vectorLength(B));
    if     (t<-1) t=-1; // допустимые значения для арккосинуса лнежат на отрезке от -1 до 1
    else if(t> 1) t= 1; // если вдруг выскочили за него, нужно поправить
    return qRadiansToDegrees(qAcos(t));
}

QRectF RectangleItem::boundingRect() const
{   // вычисляю минимальный (по координатам) угол прямоугольника
    std::vector<qreal> vX = {hotPoints.Bx, -hotPoints.Bx, hotPoints.Cx, -hotPoints.Cx};
    std::vector<qreal> vY = {hotPoints.By, -hotPoints.By, hotPoints.Cy, -hotPoints.Cy};
    qreal minX = *std::min_element(vX.begin(), vX.end());
    qreal minY = *std::min_element(vY.begin(), vY.end());

    // добавляю длину и ширину фигуры к минимальному углу и т.к.
    // рамка фигуры может быть толстой, ее (половинку) тоже нужно включить в объемлющий прямоугольник
    return QRectF (minX-borderWidth/2-1,minY-borderWidth/2-1,-2*minX+borderWidth+2,-2*minY+borderWidth+2);
}


void RectangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // рисую прямоугольник с заданными цветами, толщиной линии и углом поворота
    QPen pen(getLineColor());
    pen.setWidth(getLineWidth());
    painter->setPen(pen);
    painter->setBrush(getFillColor());
    painter->rotate(angle);
    painter->drawRect(leftBaseCorner,topBaseCorner,width,height);


    if (activeGraphicsItem == this)
    { // рисую границы активного объекта
      painter->rotate(-angle);
      QPen pen(Qt::gray );
      pen.setStyle(Qt::DotLine);
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      QRectF boundRect=boundingRect();
      painter->drawRect(boundRect);
      // рисую оси координат
      painter->drawLine(boundRect.left(),0,boundRect.width()/2,0);
      painter->drawLine(0,boundRect.top(),0,boundRect.height()/2);

      // рисую горячие точки для изменения размера
      qreal radius=hotPoints.visibleRadius;
      pen.setColor(Qt::darkBlue); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
      pen.setStyle(Qt::SolidLine);
      painter->setPen(pen);
      painter->setBrush(Qt::darkBlue); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
      painter->drawEllipse( QPointF( hotPoints.Ax, hotPoints.Ay),radius,radius);
      painter->drawEllipse( QPointF(-hotPoints.Ax,-hotPoints.Ay),radius,radius);
      painter->drawEllipse( QPointF( hotPoints.Dx, hotPoints.Dy),radius,radius);
      painter->drawEllipse( QPointF(-hotPoints.Dx,-hotPoints.Dy),radius,radius);

      // рисую горячие точки для вращения
      painter->drawEllipse( QPointF( hotPoints.Cx, hotPoints.Cy),radius,radius);
      painter->drawEllipse( QPointF(-hotPoints.Cx,-hotPoints.Cy),radius,radius);
      painter->drawEllipse( QPointF( hotPoints.Bx, hotPoints.By),radius,radius);
      painter->drawEllipse( QPointF(-hotPoints.Bx,-hotPoints.By),radius,radius);
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void RectangleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mouseMoveEvent:    pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << hotPoints.Ax<<", y:"<<hotPoints.Ay<< ") ";

    if ( this->widthIsChangingNow)
      {
        //TODO при уменьшении размера отсаются паразитные линии (boundingRect) - разобраться
        this->width=2*vectorLength(event->pos());                           // меняю ширину
        leftBaseCorner=-width/2;                                            // обновляю координаты верхнего левого угла
        hotPoints.set(width,height,angle, borderWidth);                     // обновляю горячие точки и boundingRect
        update();                                                           // перерисовываю в сцене
        setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
    if ( this->heightIsChangingNow)
      {
        //TODO при уменьшении размера отсаются паразитные линии (boundingRect) - разобраться
        this->height=2*vectorLength(event->pos());                          // меняю высоту
        topBaseCorner=-height/2;                                            // обновляю координаты верхнего левого угла
        hotPoints.set(width,height,angle, borderWidth);                     // обновляю горячие точки и boundingRect
        update();                                                           // перерисовываю в сцене
        setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
    if ( this->isRotatingNow)
      { //вращаю
        //TODO менять вид курсора в зависимости от угла
        //TODO что-то мешает вращать фигуру против часовой стрелки - разобраться
        this->angle=mousePressStartAngle+angleBetweenVectors(event->pos(), mousePressStartPos); // собственно вращаю
        hotPoints.set(width,height,angle, borderWidth);                     // обновляю горячие точки и boundingRect
        update();                                                           // перерисовываю в сцене
        setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
      { // двигаю объект
        this->setPos(mapToScene(event->pos()-mousePressStartPos));
      }

}

void RectangleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mousePressEvent:   pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), C=(x:" << hotPoints.Cx<<", y:"<<hotPoints.Cy<< ") ";

    //запоминаю начальное смещение точки в фигуре, ее ширину, высоту и начальный угол, когда была нажата клавиша мыши
    mousePressStartPos=event->pos();
    mousePressStartAngle=this->angle;
    mousePressStartWidth=this->width;
    mousePressStartHeight=this->height;

    // создаю "горячие точки" для изменения размера
    QRect A ( hotPoints.Ax-hotPoints.radius, hotPoints.Ay-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect A1(-hotPoints.Ax-hotPoints.radius,-hotPoints.Ay-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect D ( hotPoints.Dx-hotPoints.radius, hotPoints.Dy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect D1(-hotPoints.Dx-hotPoints.radius,-hotPoints.Dy-hotPoints.radius,hotPoints.radius,hotPoints.radius);

    // создаю "горячие точки" для вращения
    QRect C ( hotPoints.Cx-hotPoints.radius, hotPoints.Cy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect C1(-hotPoints.Cx-hotPoints.radius,-hotPoints.Cy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect B ( hotPoints.Bx-hotPoints.radius, hotPoints.By-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect B1(-hotPoints.Bx-hotPoints.radius,-hotPoints.By-hotPoints.radius,hotPoints.radius,hotPoints.radius);

    if ( // проверяю попадание в зону захвата мышкой для изменения ширины
         A.contains(event->pos().x(), event->pos().y()) || A1.contains(event->pos().x(), event->pos().y())
       )
      { //начал менять размер - (точку я уже запомнил, курсор поменял)
        widthIsChangingNow=true;
      }
    else
    if ( // проверяю попадание в зоны захвата мышкой для изменения высоты
         D.contains(event->pos().x(), event->pos().y()) || D1.contains(event->pos().x(), event->pos().y())
       )
       { //начал менять размер - (точку я уже запомнил, курсор поменял)
          heightIsChangingNow=true;
       }
    else
    if ( // проверяю попадание в зоны захвата мышкой для вращения
         // TODO: вид курсора еще зависит от угла поворота - надо посчитать
            C.contains(event->pos().x(), event->pos().y()) || C1.contains(event->pos().x(), event->pos().y()) ||
            B.contains(event->pos().x(), event->pos().y()) || B1.contains(event->pos().x(), event->pos().y())
       )
      { // начал вращать  - (точку я уже запомнил, курсор поменял)
        isRotatingNow=true;
      }
    else
      { // двигаю объект
        // готовлю мышку к перетаскиванию
        this->setCursor(QCursor(Qt::ClosedHandCursor));

        // меняю активный элемент
        setActiveGraphicsItem(this);
      }
    Q_UNUSED(event);
}



void RectangleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{   // восстанавливаю курсор и сбрасываю флаги изменения размера и вращения
    this->setCursor(QCursor(Qt::ArrowCursor));
    isRotatingNow=false;
    widthIsChangingNow=false;
    heightIsChangingNow=false;

    qDebug() << "mouseReleaseEvent: pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), C=(x:" << hotPoints.Cx<<", y:"<<hotPoints.Cy<< ") ";

    Q_UNUSED(event);
}

void RectangleItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

    // создаю "горячие точки" для изменения размера
    QRect A ( hotPoints.Ax-hotPoints.radius, hotPoints.Ay-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect A1(-hotPoints.Ax-hotPoints.radius,-hotPoints.Ay-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect D ( hotPoints.Dx-hotPoints.radius, hotPoints.Dy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect D1(-hotPoints.Dx-hotPoints.radius,-hotPoints.Dy-hotPoints.radius,hotPoints.radius,hotPoints.radius);

    // создаю "горячие точки" для вращения
    QRect C ( hotPoints.Cx-hotPoints.radius, hotPoints.Cy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect C1(-hotPoints.Cx-hotPoints.radius,-hotPoints.Cy-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect B ( hotPoints.Bx-hotPoints.radius, hotPoints.By-hotPoints.radius,hotPoints.radius,hotPoints.radius);
    QRect B1(-hotPoints.Bx-hotPoints.radius,-hotPoints.By-hotPoints.radius,hotPoints.radius,hotPoints.radius);

    if ( // проверяю попадание в зоны захвата мышкой для изменения размера
            A.contains(event->pos().x(), event->pos().y()) || A1.contains(event->pos().x(), event->pos().y()) ||
            D.contains(event->pos().x(), event->pos().y()) || D1.contains(event->pos().x(), event->pos().y())
       )
      {
        this->setCursor(QCursor(Qt::SizeAllCursor));
        qDebug() << "hoverMoveEvent S : pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << hotPoints.Ax<<", y:"<<hotPoints.Ay<< ") ";
      }
    else
    if ( // проверяю попадание в зоны захвата мышкой для вращения
         // TODO: вид курсора еще зависит от угла поворота - надо посчитать
            C.contains(event->pos().x(), event->pos().y()) || C1.contains(event->pos().x(), event->pos().y()) ||
            B.contains(event->pos().x(), event->pos().y()) || B1.contains(event->pos().x(), event->pos().y())
       )
      {
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
        qDebug() << "hoverMoveEvent R : pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), C=(x:" << hotPoints.Cx<<", y:"<<hotPoints.Cy<< ") ";
      }
    else
      {
        qDebug() << "hoverMoveEvent - : pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), C=(x:" << hotPoints.Cx<<", y:"<<hotPoints.Cy<< ") ";
        this->setCursor(QCursor(Qt::ArrowCursor));
      }


    Q_UNUSED(event);

}

void RectangleItem::setLineWidth(int lineWidth)
{
    borderWidth=lineWidth;
    hotPoints.set(width,height,angle, borderWidth);
}

void RectangleItem::setAngle(int newAngle)
{
    angle=newAngle;
    hotPoints.set(width,height,angle, borderWidth);
}
