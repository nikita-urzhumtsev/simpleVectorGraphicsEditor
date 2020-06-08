#include "linearitem.h"
#include <QtMath>
#include "globaldata.h"

LinearItem::LinearItem(QObject *parent):
    MovableItem(parent)
{
    nodeIsChangingNow=false;
}

LinearItem::~LinearItem()
{
}

void LinearItem::appendPoint(QPointF point)
{
    lineNodes.append(point);
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
{   // большой размер нужен тогда, когда я меняю размер или двигаю или поворачиваю объект
    // тогда нужно правильно перерисовавыть большую площадь
    QRectF frameRect=this->frameRect();
    std::vector<qreal> dimentions = {frameRect.height()+borderWidth, frameRect.width()+borderWidth};
    int extraSpace = ( *std::max_element(dimentions.begin(), dimentions.end()) ) / 2 ;

    // если элемент не активен, то этот прямоугольник должен быть примерно размером с объект и используется для захвата мыши
    if (globalData.activeGraphicsItem != this)
            extraSpace=2;

    // квадрат с самой большой стороной, куда гарантированно помещается прямоугольник с рамкой
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
    if (globalData.activeGraphicsItem == this)
    {
        // рисую границы активного объекта
        QPen pen(selectedItemBorderColor );
        pen.setStyle(selectedItemLineStyle );
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        QRectF frameRect=this->frameRect();
        painter->drawRect(frameRect);

        qreal radius=hotPointVisibleRadius();
        pen.setColor(hotPointColor);
        pen.setStyle(hotPointLineStyle);
        painter->setPen(pen);
        painter->setBrush(hotPointColor); // TODO цвет горячих точек нужно сделать контрастным с цветом рамки
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
{   // вначале проверяю вырожденные случаи
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

// функция для вычисления расстояния от заданной точки M(Хм,Ум) до прямой, проходящей через точки A(х1,у1) и B(х2,у2)
qreal distanceToPoint(QPointF M, QPointF A)
{   //считаю расстояние между точками М и A
    qreal distance, dx,dy;
    dx=A.x()-M.x();
    dy=A.y()-M.y();
    distance=qSqrt( dx*dx + dy*dy );
    return distance;

}

// функция для вычисления расстояния от заданной точки M(Хм,Ум) до прямой, проходящей через точки A(х1,у1) и B(х2,у2)
qreal distanceToLine(QPointF M, QPointF A, QPointF B)
{   // Получаю координаты проекции на линию
    QPointF projectionPoint=projectionToLine(M,A,B);
    // считаю расстояние между проекцией и точкой М
    return distanceToPoint(projectionPoint,M);
}

void LinearItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (nodeIsChangingNow)
    {  // если сейчас происходит перемещение узла
       int i=lineNodes.indexOf(this->mousePressStartNode);
       if (i>=0) // на вскяий случай проверю, что точка найдена
       {
         lineNodes[i]=event->pos();
         mousePressStartNode=event->pos();
         update();
       }
    }
    else
    { // двигаю объект
      this->setPos(mapToScene(event->pos()-mousePressStartPos));
    }

}

void LinearItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{   // TODO сравнить с hover - возможно, что можно все превратить в простые и маленькие функции
    //      оставил это намеренно, не стал делать в этом случае шаблонную функцию (а для линий сделал), чтобы показать
    //      процесс появления подобных шаблонных функций (фактически это вторая фаза рефакторинга, когда все в целом работает и потом
    //      привожу код либо к читабельному виду, либо его усовершенствую, чтобы не нужно было одни и те же дейстивя делать несколько раз)

    //запоминаю начальное смещение точки в фигуре, ее ширину, высоту и начальный угол, когда была нажата клавиша мыши
    mousePressStartPos=event->pos();

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
        return; // если мышка рядом с горячей точкой, то работаем с ней и не двигаем фигуру
    }


    { // двигаю объект
      // готовлю мышку к перетаскиванию
      this->setCursor(QCursor(Qt::ClosedHandCursor));

      // меняю активный элемент
      globalData.setActiveGraphicsItem(this);
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
               (i<lineNodes.count()-1 && distanceToPoint(lineNodes[i], lineNodes[i+1])<2)
          )
       { // удалю активную точку
         lineNodes.remove(i);
       }
       update();
    }

    this->setCursor(QCursor(Qt::ArrowCursor));
    nodeIsChangingNow=false;
}

// функция для работы с линиями (как минимум для определения активного ребра и вставки нового узла)
template< typename Func >
bool LinearItem::findSuitableLineAndDo(QPointF pos, Func func )
{
    qreal distance;
    for (int i=0; i<lineNodes.count()-1;++i)
    {
        distance=distanceToLine(pos,lineNodes[i], lineNodes[i+1]);
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
    { // да, курсор рядом с линией, меняю вид указателя мыши
      this->setCursor(QCursor(Qt::UpArrowCursor));
      Q_UNUSED(event);
      Q_UNUSED(i);
    } );

    if (closeToLine) return; // нашел близкую линию, дальше буду работать с ней

    // во всех остальных случаях
    this->setCursor(QCursor(Qt::ArrowCursor));
}
