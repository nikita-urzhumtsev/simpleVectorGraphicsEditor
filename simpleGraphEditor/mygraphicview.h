#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>

class MyGraphicView : public QGraphicsView
{
public:

    Q_OBJECT


   public:
       explicit MyGraphicView(QWidget *parent = 0);
       ~MyGraphicView();


    void createMovableRectangle(); // создание перетаскиваемого объекта (по нажатию кнопки)
    void removeGraphicsItem(QGraphicsItem * removedItem); //удаление графического объекта

signals:

private slots:

private:
    QGraphicsScene      *scene;     // сцена, где будет происходить все действие :-)

private:
    void deleteItemsFromGroup(QGraphicsItemGroup *group_1);
};

#endif // MYGRAPHICVIEW_H