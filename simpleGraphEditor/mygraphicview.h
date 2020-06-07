#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>
#include "movableitem.h"

class MyGraphicView : public QGraphicsView
{
public:

    Q_OBJECT


   public:
       explicit MyGraphicView(QWidget *parent = 0);
       ~MyGraphicView();


    void createMovableRectangle(); // создание перетаскиваемого прямоугольника (по нажатию кнопки)
    void createMovableLine();      // создание лиманой линии (по нажатию кнопки)
    void removeGraphicsItem(MovableItem * removedItem); //удаление графического объекта
    void saveToFile(const QString & documentFileName);  // сохраняю сцену в файл
    void readFromFile(const QString & documentFileName);// загружаю сцену из файл

signals:

private slots:

private:
    QGraphicsScene      *scene;     // сцена, где будет происходить все действие :-)

private:
    //void deleteItemsFromGroup(QGraphicsItemGroup *group_1);
};

#endif // MYGRAPHICVIEW_H
