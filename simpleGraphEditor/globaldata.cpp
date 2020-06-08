#include "globaldata.h"

GlobalData globalData;

GlobalData::GlobalData():
    currentLineColor(Qt::cyan),
    currentFillColor(Qt::white),
    currentLineWidth(1),
    currentAngle(0),
    documentFileName("newFile.svg")
{
    activeGraphicsItem = nullptr;
    mainWindow = nullptr;

}

void  GlobalData::setActiveGraphicsItem(MovableItem * newActiveItem)
{
    if (activeGraphicsItem && activeGraphicsItem!=newActiveItem)
    { QGraphicsItem * deactivatedItem=activeGraphicsItem;
      activeGraphicsItem=newActiveItem;
      deactivatedItem->setAcceptHoverEvents(false);
      deactivatedItem->update();
    }
    else
    {
      activeGraphicsItem=newActiveItem;
    }

    if (activeGraphicsItem)
    {   MovableItem * grItem=dynamic_cast<MovableItem *>(activeGraphicsItem);
        currentFillColor=grItem->getFillColor();
        currentLineColor=grItem->getLineColor();
        currentLineWidth=grItem->getLineWidth();
        currentAngle=grItem->getAngle();
        activeGraphicsItem->setAcceptHoverEvents(true);
        activeGraphicsItem->update();
        mainWindow->updateBottons();
    }

}
