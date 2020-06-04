#include "movableitem.h"
#include "mainwindow.h"


MovableItem::MovableItem(QObject *parent) :
    QObject(parent), QGraphicsItem()
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


