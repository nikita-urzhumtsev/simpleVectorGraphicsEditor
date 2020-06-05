#include "linearitem.h"
#include "mainwindow.h"
#include <QtMath>

LinearItem::LinearItem(QObject *parent):
    MovableItem(parent)
{
    lineNodes.append(QPointF(  0,  0));
    lineNodes.append(QPointF(100, 10));
    lineNodes.append(QPointF(100, 140));
    lineNodes.append(QPointF( 10, 140));
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
    //TODO подумать как надо пересчитывать прямоугольник, если уменьшается одна из граничных координат
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

    { // двигаю объект
      this->setPos(mapToScene(event->pos()-mousePressStartPos));
    }

}

void LinearItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ // TODO
    //запоминаю начальное смещение точки в фигуре, ее ширину, высоту и начальный угол, когда была нажата клавиша мыши
    mousePressStartPos=event->pos();
    qDebug() << "line.mousePressEvent:   pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << 0 <<", y:"<< 0 << ") ";

    { // двигаю объект
      // готовлю мышку к перетаскиванию
      this->setCursor(QCursor(Qt::ClosedHandCursor));

      // меняю активный элемент
      setActiveGraphicsItem(this);
    }

    Q_UNUSED(event);
}



void LinearItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}

void LinearItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << "\nline.hoverMoveEvent:    pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), A=(x:" << 0 <<", y:"<< 0 << ") ";
    qreal distance;

    // проверяю достаточно ли близко курсор мыши к горячей точке
    bool closeToHotPoint=false;
    for (auto aPoint : lineNodes)
    {   // если мышка близко к горячей точке
        distance=distanceToPoint(event->pos(),aPoint);
        qDebug() << "line.hoverMoveEvent: P  pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), dist=" << distance << " hotPointRadius()="<<hotPointRadius();

        if (distance<=hotPointRadius()/2)
        {
            closeToHotPoint=true;
            setCursor(QCursor(Qt::CrossCursor));
            break; // другие можно не искать
        }
    }

    if (closeToHotPoint) return; // Преимущества у горячих точке, если мышка рядом с ней, то работаем с горячей точкой

    // проверяю достаточно ли близко курсор мыши к линии
    bool closeToLine=false;
    QPointF prevPoint=lineNodes.first();
    for (auto aPoint : lineNodes)
    {
        if (aPoint==prevPoint) continue; // вырожденные отрезки пропускаю

        distance=distanceToLine(event->pos(),aPoint,prevPoint);
        // прямоугольник с диагональю равной данному отрезку (и слегка увеличенный по ширине и высоте)
        QRectF lineRect=QRectF(prevPoint, aPoint);
        lineRect.adjust(-hotPointRadius(),-hotPointRadius(),hotPointRadius(),hotPointRadius());

        //TODO иногда при переключении между фигурами перестает ловить вертикальные отрезки - не понятно почему
        qDebug() << "line.hoverMoveEvent: L  pos=(x:" << event->pos().x()<<", y:"<<event->pos().y()<< "), dist=" << distance << " radius="<<hotPointRadius()<< "A=(x:" << aPoint.x()<<", y:"<<aPoint.y()<< "), B=(x:" << prevPoint.x()<<", y:"<<prevPoint.y()<< ")"<< "rect.contains="<<lineRect.contains(event->pos());

        // если мышка близко к линии и попадает в минимальный прямоугольник, содержащий этот отрезок прямой
        if (  distance<=hotPointRadius() && lineRect.contains(event->pos())   )
        {
            closeToLine=true;
            setCursor(QCursor(Qt::UpArrowCursor));
            break;               // нашел первую близкую линию, другие можно не проверять
        }
        prevPoint=aPoint;
    }

    if (closeToLine) return; // нашел близкую линию, дальше буду работать с ней

    // во всех остальных случаях
    this->setCursor(QCursor(Qt::ArrowCursor));


    Q_UNUSED(event);

}
