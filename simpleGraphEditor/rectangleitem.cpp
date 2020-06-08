#include "rectangleitem.h"
#include <QtMath>

#include "globaldata.h"

RectangleItem::RectangleItem(QObject *parent):
  MovableItem(parent)
{
    setHeightWidth(60, 120); // TODO вынести в константы

    angle=globalData.currentAngle;           // угол поворта прямоугольника в градусах

    isRotatingNow=false;          // флаг состояния вращения в настоящий момент
    widthIsChangingNow=false;     // флаг состояния изменения ширины в настоящий момент
    heightIsChangingNow=false;    // флаг состояния изменения высоты в настоящий момент


    hotPoints.set(width,height,angle); // вычисляю горячие точки и frameRect

}

RectangleItem::~RectangleItem()
{
}

void  RectangleItem::setHeightWidth(int height, int width)
{
    this->width=width;       // ширина прямоугольника
    this->height=height;     // высота прямоугольника
    leftBaseCorner=-width/2; // левый/верхний угол привязки прямоугольника
    topBaseCorner=-height/2;

}

void HotPoints::set(int width, int height, int angle)
{   // правильно посчитать объемлющий прямоугольник
    // чистая математика, см картинку, поясняющую формулы (..\images\RectangleHotPointIdentification.jpg)
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

}

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
    qreal sign=(x1*y2 - y1*x2)<0 ? -1: 1;
    return qRadiansToDegrees(qAcos(t))*sign;
}

QRectF RectangleItem::frameRect() const
{   // вычисляю минимальный (по координатам) угол прямоугольника
    std::vector<qreal> vX = {hotPoints.Bx, -hotPoints.Bx, hotPoints.Cx, -hotPoints.Cx};
    std::vector<qreal> vY = {hotPoints.By, -hotPoints.By, hotPoints.Cy, -hotPoints.Cy};
    qreal minX = *std::min_element(vX.begin(), vX.end());
    qreal minY = *std::min_element(vY.begin(), vY.end());

    // добавляю длину и ширину фигуры к минимальному углу и т.к.
    // рамка фигуры может быть толстой, ее (половинку) тоже нужно включить в объемлющий прямоугольник
    return QRectF (minX-borderWidth/2-1,minY-borderWidth/2-1,-2*minX+borderWidth+2,-2*minY+borderWidth+2);
}

QRectF RectangleItem::boundingRect() const
{   // большой размер нужен тогда, когда я меняю размер или двигаю или поворачиваю объект
    // тогда нужно правильно перерисовавыть большую площадь
    std::vector<int> dimentions = {height+borderWidth, width+borderWidth, mousePressStartWidth+borderWidth, mousePressStartHeight+borderWidth};
    int extraSpace = ( *std::max_element(dimentions.begin(), dimentions.end()) ) / 2 ;

    // если элемент не активен, то этот прямоугольник должен быть примерно размером с объект и используется для захвата мыши
    if (globalData.activeGraphicsItem != this)
        extraSpace=2;

    // квадрат с самой большой стороной, куда гаратированно помещается прямоугольник с рамкой
    return frameRect().adjusted(-extraSpace,-extraSpace,extraSpace,extraSpace);
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


    if (globalData.activeGraphicsItem == this)
    { // рисую границы активного объекта
      painter->rotate(-angle);
      QPen pen(selectedItemBorderColor);
      pen.setStyle(selectedItemLineStyle);
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      QRectF frameRect=this->frameRect();
      painter->drawRect(frameRect);
      // рисую оси координат
      painter->drawLine(frameRect.left(),0,frameRect.width()/2,0);
      painter->drawLine(0,frameRect.top(),0,frameRect.height()/2);

      // рисую горячие точки для изменения размера
      qreal radius=hotPointVisibleRadius();
      pen.setColor(hotPointColor);
      pen.setStyle(hotPointLineStyle);
      painter->setPen(pen);
      painter->setBrush(hotPointColor); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
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
    if ( this->widthIsChangingNow)
      {
        this->width=2*vectorLength(event->pos());                           // меняю ширину
        leftBaseCorner=-width/2;                                            // обновляю координаты верхнего левого угла
        hotPoints.set(width,height,angle);                                  // обновляю горячие точки и frameRect
        update();                                                           // перерисовываю в сцене
        globalData.setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
    if ( this->heightIsChangingNow)
      {
        this->height=2*vectorLength(event->pos());                          // меняю высоту
        topBaseCorner=-height/2;                                            // обновляю координаты верхнего левого угла
        hotPoints.set(width,height,angle);                                  // обновляю горячие точки и frameRect
        update();                                                           // перерисовываю в сцене
        globalData.setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
    if ( this->isRotatingNow)
      { //вращаю
        this->angle=mousePressStartAngle+angleBetweenVectors(mousePressStartPos, event->pos()); // собственно вращаю
        hotPoints.set(width,height,angle);                                  // обновляю горячие точки и frameRect
        update();                                                           // перерисовываю в сцене
        globalData.setActiveGraphicsItem(this);                                        // обновляю данные в главном окне
      }
    else
      { // двигаю объект
        this->setPos(mapToScene(event->pos()-mousePressStartPos));
      }

}

void RectangleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //запоминаю начальное смещение точки в фигуре, ее ширину, высоту и начальный угол, когда была нажата клавиша мыши
    mousePressStartPos=event->pos();
    mousePressStartAngle=this->angle;
    mousePressStartWidth=this->width;
    mousePressStartHeight=this->height;

    qreal radius=hotPointRadius();
    // создаю "горячие точки" для изменения размера
    QRect A ( hotPoints.Ax-radius, hotPoints.Ay-radius,2*radius,2*radius);
    QRect A1(-hotPoints.Ax-radius,-hotPoints.Ay-radius,2*radius,2*radius);
    QRect D ( hotPoints.Dx-radius, hotPoints.Dy-radius,2*radius,2*radius);
    QRect D1(-hotPoints.Dx-radius,-hotPoints.Dy-radius,2*radius,2*radius);

    // создаю "горячие точки" для вращения
    QRect C ( hotPoints.Cx-radius, hotPoints.Cy-radius,2*radius,2*radius);
    QRect C1(-hotPoints.Cx-radius,-hotPoints.Cy-radius,2*radius,2*radius);
    QRect B ( hotPoints.Bx-radius, hotPoints.By-radius,2*radius,2*radius);
    QRect B1(-hotPoints.Bx-radius,-hotPoints.By-radius,2*radius,2*radius);

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
        globalData.setActiveGraphicsItem(this);
      }
    Q_UNUSED(event);
}



void RectangleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{   // восстанавливаю курсор и сбрасываю флаги изменения размера и вращения
    this->setCursor(QCursor(Qt::ArrowCursor));
    isRotatingNow=false;
    widthIsChangingNow=false;
    heightIsChangingNow=false;

    Q_UNUSED(event);
}

void RectangleItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    qreal radius=hotPointRadius();
    // создаю "горячие точки" для изменения размера
    QRect A ( hotPoints.Ax-radius, hotPoints.Ay-radius,2*radius,2*radius);
    QRect A1(-hotPoints.Ax-radius,-hotPoints.Ay-radius,2*radius,2*radius);
    QRect D ( hotPoints.Dx-radius, hotPoints.Dy-radius,2*radius,2*radius);
    QRect D1(-hotPoints.Dx-radius,-hotPoints.Dy-radius,2*radius,2*radius);

    // создаю "горячие точки" для вращения
    QRect C ( hotPoints.Cx-radius, hotPoints.Cy-radius,2*radius,2*radius);
    QRect C1(-hotPoints.Cx-radius,-hotPoints.Cy-radius,2*radius,2*radius);
    QRect B ( hotPoints.Bx-radius, hotPoints.By-radius,2*radius,2*radius);
    QRect B1(-hotPoints.Bx-radius,-hotPoints.By-radius,2*radius,2*radius);

    if ( // проверяю попадание в зоны захвата мышкой для изменения размера
            A.contains(event->pos().x(), event->pos().y()) || A1.contains(event->pos().x(), event->pos().y()) ||
            D.contains(event->pos().x(), event->pos().y()) || D1.contains(event->pos().x(), event->pos().y())
       )
      {
        this->setCursor(QCursor(Qt::SizeAllCursor));
      }
    else
    if ( // проверяю попадание в зоны захвата мышкой для вращения
            C.contains(event->pos().x(), event->pos().y()) || C1.contains(event->pos().x(), event->pos().y()) ||
            B.contains(event->pos().x(), event->pos().y()) || B1.contains(event->pos().x(), event->pos().y())
       )
      {
        this->setCursor(QCursor(Qt::CrossCursor));
      }
    else
      {
        this->setCursor(QCursor(Qt::ArrowCursor));
      }
    Q_UNUSED(event);
}

void RectangleItem::setLineWidth(int lineWidth)
{
    borderWidth=lineWidth;
    hotPoints.set(width,height,angle);
}

void RectangleItem::setAngle(int newAngle)
{
    angle=newAngle;
    hotPoints.set(width,height,angle);
}
