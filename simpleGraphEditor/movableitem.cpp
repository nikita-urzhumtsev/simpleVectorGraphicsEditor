#include "movableitem.h"
#include "globaldata.h"


MovableItem::MovableItem(QObject *parent) :
    QObject(parent), QGraphicsItem(), mousePressStartPos(0,0)
{
    fillColor=globalData.currentFillColor;   // цвет заливки
    borderWidth=globalData.currentLineWidth; // полщина линии
    borderColor=globalData.currentLineColor; // цвет линии

    // меняю активный элемент
    if (globalData.activeGraphicsItem)
    { QGraphicsItem * deactivatedItem=globalData.activeGraphicsItem;
      globalData.activeGraphicsItem=this;
      deactivatedItem->setAcceptHoverEvents(false);
      deactivatedItem->update();
    }
    else
    {
      globalData.activeGraphicsItem=this;
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
