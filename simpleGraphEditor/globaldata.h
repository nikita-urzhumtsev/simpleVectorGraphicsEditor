#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QObject>
#include <QColor>
#include "movableitem.h"
#include "mainwindow.h"

class GlobalData
{
public:
    GlobalData();

    QColor currentLineColor;
    QColor currentFillColor;
    int    currentLineWidth;
    int    currentAngle;

    const int sceneWidth=3000;
    const int sceneHeight=4000;

    MovableItem * activeGraphicsItem;
    MainWindow * mainWindow;
    QString documentFileName;

    void  setActiveGraphicsItem(MovableItem * newActiveItem);

};

extern GlobalData globalData;

#endif // GLOBALDATA_H
