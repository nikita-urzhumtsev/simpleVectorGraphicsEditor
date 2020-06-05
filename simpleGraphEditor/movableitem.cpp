#include "movableitem.h"
#include "mainwindow.h"


MovableItem::MovableItem(QObject *parent) :
    QObject(parent), QGraphicsItem(), mousePressStartPos(0,0)
{
    fillColor=currentFillColor;   // цвет заливки
    borderWidth=currentLineWidth; // полщина линии
    borderColor=currentLineColor; // цвет линии

    // меняю активный элемент
    // TODO сравнить с функцией void  setActiveGraphicsItem(QGraphicsItem * newActiveItem) - отрефакторить
    if (activeGraphicsItem)
    { QGraphicsItem * deactivatedItem=activeGraphicsItem;
      activeGraphicsItem=this;
      deactivatedItem->setAcceptHoverEvents(false);
      deactivatedItem->update();
    }
    else
    {
      activeGraphicsItem=this;
      setAcceptHoverEvents(true);
    }
}

MovableItem::~MovableItem()
{

}


void MovableItem::setFillColor(QColor color)
{
    fillColor=color;
}

void MovableItem::setLineColor(QColor color)
{
    borderColor=color;
}

void MovableItem::setLineWidth(int lineWidth)
{
    borderWidth=lineWidth;
}


qreal MovableItem::hotPointVisibleRadius()
{
    qreal visibleRadius = borderWidth<fixedMark ? borderWidth : fixedMark; // горячая точка не должна быть слишком большой
    if (visibleRadius==1) visibleRadius=2;

    return visibleRadius;
}

qreal MovableItem::hotPointRadius()
{
    qreal radius=10;//2*fixedMark; // зона захвата мыши пусть будет в 2 раза больше, чем видимая точка, так ее будет легче поймать
    return radius;
}
