#include "linearitem.h"

LinearItem::LinearItem(QObject *parent):
    MovableItem(parent)
{

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

QRectF LinearItem::boundingRect() const
{   // // TODO вычисляю минимальный (по координатам) угол прямоугольника
    return QRectF (0,0,200,200);
}

void LinearItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // рисую ломаную линию с заданными цветами, толщиной линии
    QPen pen(getLineColor());
    pen.setWidth(getLineWidth());
    painter->setPen(pen);
    painter->setBrush(getFillColor());
    painter->drawLine(10,10,100,10);

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void LinearItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{ // TODO

}

void LinearItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ // TODO
    Q_UNUSED(event);
}



void LinearItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{   // TODO
    this->setCursor(QCursor(Qt::ArrowCursor));

    Q_UNUSED(event);
}

void LinearItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{ // TODO

    Q_UNUSED(event);

}
