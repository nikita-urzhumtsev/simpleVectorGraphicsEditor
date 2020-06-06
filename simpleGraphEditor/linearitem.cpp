#include "linearitem.h"
#include "mainwindow.h"
#include <QtMath>

LinearItem::LinearItem(QObject *parent):
    MovableItem(parent)
{
    lineNodes.append(QPointF(  0,  0));
    lineNodes.append(QPointF(100, 10));
    lineNodes.append(QPointF(100, 140));
    lineNodes.append(QPointF(110, 240));
    lineNodes.append(QPointF( 50, 170));
    lineNodes.append(QPointF( 90, 200));
    lineNodes.append(QPointF( 10, 140));

    nodeIsChangingNow=false;
}

LinearItem::~LinearItem()
{
}

void LinearItem::setLineWidth(int lineWidth)
{
    borderWidth=lineWidth;
}

void LinearItem::setAngle(int)
{
   // do nothing
}

QRectF LinearItem::frameRect() const
{
    return this->lineNodes.boundingRect();
}

QRectF LinearItem::boundingRect() const
{   QRectF frameRect=this->frameRect();
    std::vector<qreal> dimentions = {frameRect.height()+borderWidth, frameRect.width()+borderWidth};
    int extraSpace = ( *std::max_element(dimentions.begin(), dimentions.end()) ) / 2 ;

    // квадрат с самой большой стороной, куда гаратированно помещается прямоугольник с рамкой
    return frameRect.adjusted(-extraSpace,-extraSpace,extraSpace,extraSpace);
}


void LinearItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // рисую ломаную линию с заданными цветами, толщиной линии
    QPen pen(getLineColor());
    pen.setWidth(getLineWidth());
    painter->setPen(pen);
    painter->setBrush(getFillColor());
    painter->drawPolyline(this->lineNodes);

    // если эта фигура сейчас активна, то обрамляю ее рамкой
    // и выделяю "горячие точки" - вершины углов и начало и конец линии
    if (activeGraphicsItem == this)
    {
        // рисую границы активного объекта
        QPen pen(Qt::gray );       //TODO цвет и стил рамки выделить в общие константы для всех фигур
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        QRectF frameRect=this->frameRect();
        painter->drawRect(frameRect);

        qreal radius=hotPointVisibleRadius();
        pen.setColor(Qt::darkBlue); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->setBrush(Qt::darkBlue); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
        // бегу по вершинам полилинии и выделяю горячие точки
        for (auto aPoint : lineNodes)
        {
            painter->drawEllipse( aPoint,radius,radius);
        }
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

// функция для вычисления точки проекции заданной точки M(Хм,Ум) до прямой, проходящей через точки A(х1,у1) и B(х2,у2)
QPointF projectionToLine(QPointF M, QPointF A, QPointF B)
{   // вначале проверяю вырожденные случаю
    if (A.x()==B.x()) {return QPointF(A.x(),M.y());};
    if (A.y()==B.y()) {return QPointF(M.x(),A.y());};

    // пошел невырожденный случай - считаем по формуле

    // прямая A-B задана по формуле y=k1*x+z1:
    // точки А и В принадлежат ей - имеем систему двух линейных уравнений
    qreal k1, z1;
    k1=(A.y()-B.y())/(A.x()-B.x());
    z1=A.y()-k1*A.x();

    // прямая, перпендикулярная первой задана уравнением y = -x/k1 + z2
    // и она проходит через точку M (т.к. мы ищем ее проекцию на первую прямую)
    qreal z2;
    z2=M.y()+M.x()/k1;

    // проекция - это точка пересечения двух предыдущих прямых - решаем систему уравнений
    qreal x, y;
    x=(z2-z1)*k1/(k1*k1+1);
    y=k1*x+z1;

    return QPointF(x,y);

}

// функция для вычисления Расстояния от заданной точки M(Хм,Ум) до прямой, проходящей через точки A(х1,у1) и B(х2,у2)
qreal distanceToPoint(QPointF M, QPointF A)
{   //считаю расстояние между точками М и A
    qreal distance, dx,dy;
    dx=A.x()-M.x();
    dy=A.y()-M.y();
    distance=qSqrt( dx*dx + dy*dy );
    return distance;

}

// функция для вычисления Расстояния от заданной точки M(Хм,Ум) до прямой, проходящей через точки A(х1,у1) и B(х2,у2)
qreal distanceToLine(QPointF M, QPointF A, QPointF B)
{   // Получаю координаты проекции на линию
    QPointF projectionPoint=projectionToLine(M,A,B);
    // считаю расстояние между проекцией и точкой М
    return distanceToPoint(projectionPoint,M);
}

void LinearItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{ // TODO
    qDebug() << "line.mouseMoveEvent:    pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << 0 <<", y:"<< 0 << ") ";

    if (nodeIsChangingNow)
    {
       int i=lineNodes.indexOf(this->mousePressStartNode);
       lineNodes[i]=event->pos();
       mousePressStartNode=event->pos();
       update();
    }
    else
    { // двигаю объект
      this->setPos(mapToScene(event->pos()-mousePressStartPos));
    }

}

void LinearItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ // TODO сравнить с hover - возмоно, что можно все превратить в простые и маленькие функции

    //запоминаю начальное смещение точки в фигуре, ее ширину, высоту и начальный угол, когда была нажата клавиша мыши
    mousePressStartPos=event->pos();

    qDebug() << "line.mousePressEvent:   pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << 0 <<", y:"<< 0 << ") ";

    qreal distance;

    // проверяю достаточно ли близко курсор мыши к горячей точке
    bool closeToHotPoint=false;
    for (auto aPoint : lineNodes)
    {   // если мышка близко к горячей точке
        distance=distanceToPoint(event->pos(),aPoint);

        if (distance<=hotPointRadius()/2)
        {
            closeToHotPoint=true;
            mousePressStartNode=aPoint;
            break; // другие можно не искать
        }
    }

    if (closeToHotPoint)
    {
        nodeIsChangingNow=true;
        return; // Преимущества у горячих точке, если мышка рядом с ней, то работаем с горячей точкой
    }


    { // двигаю объект
      // готовлю мышку к перетаскиванию
      this->setCursor(QCursor(Qt::ClosedHandCursor));

      // меняю активный элемент
      setActiveGraphicsItem(this);
    }

}



void LinearItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if (nodeIsChangingNow)
    { //схлопывание близких точек
       int i=lineNodes.indexOf(this->mousePressStartNode);
       // если предыдущая точка по отношению к активной точке лежит слишком близко
       // или следующая точка за активной точкой лежит слишком близко
       if (
               (i>0 && distanceToPoint(lineNodes[i], lineNodes[i-1])<2) ||
               (i<lineNodes.count() && distanceToPoint(lineNodes[i], lineNodes[i+1])<2)
          )
       { // удалю активную точку
           qDebug()<<"\n\n============================== remove point number"<<i;
           lineNodes.remove(i);
       }
       update();
    }

    this->setCursor(QCursor(Qt::ArrowCursor));
    nodeIsChangingNow=false;
}

// функция для работы с линями (как минимум для определения активного ребра и вставки нового узла)
template< typename Func >
bool LinearItem::findSuitableLineAndDo(QPointF pos, Func func )
{
    qreal distance;
    for (int i=0; i<lineNodes.count()-1;++i)
    {   distance=distanceToLine(pos,lineNodes[i], lineNodes[i+1]);
        // прямоугольник с диагональю равной данному отрезку (и слегка увеличенный по ширине и высоте)
        QRectF lineRect=QRectF(lineNodes[i], lineNodes[i+1]);
        lineRect.adjust(-hotPointRadius(),-hotPointRadius(),hotPointRadius(),hotPointRadius());

        // если мышка близко к линии и попадает в минимальный прямоугольник, содержащий этот отрезок прямой
        if (  distance<=hotPointRadius() && lineRect.contains(pos)   )
        { // нашел нужное ребро - выполняю над ним требуемую операцию
            func(i);
            return true;  // всё - больше искать ничего не надо
        }
    }
    return false;
}


void LinearItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{   // создание новой точки
    findSuitableLineAndDo( event->pos(), [this, event](int i)
    { // вставляю новую вершину в полилинию
        lineNodes.insert(i+1, event->pos());
        this->update();
    });

    this->setCursor(QCursor(Qt::ArrowCursor));
}

void LinearItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    qreal distance;

    // проверяю достаточно ли близко курсор мыши к горячей точке
    bool closeToHotPoint=false;
    for (auto aPoint : lineNodes)
    {   // если мышка близко к горячей точке
        distance=distanceToPoint(event->pos(),aPoint);

        if (distance<=hotPointRadius()/2)
        {
            closeToHotPoint=true;
            setCursor(QCursor(Qt::CrossCursor));
            break; // другие можно не искать
        }
    }

    if (closeToHotPoint) return; // Преимущества у горячих точке, если мышка рядом с ней, то работаем с горячей точкой

    // проверяю достаточно ли близко курсор мыши к линии
    bool closeToLine=findSuitableLineAndDo( event->pos(),[this, event](int i)
    { this->setCursor(QCursor(Qt::UpArrowCursor));
      Q_UNUSED(event);
      Q_UNUSED(i);
    } );


    if (closeToLine) return; // нашел близкую линию, дальше буду работать с ней

    // во всех остальных случаях
    this->setCursor(QCursor(Qt::ArrowCursor));
}
