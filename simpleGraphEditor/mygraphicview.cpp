#include "mygraphicview.h"
#include "movableitem.h"
#include "linearitem.h"
#include "rectangleitem.h"
#include "mainwindow.h"

//TODO все magic numbers надо вынести в константы (по всему проекту)

MyGraphicView::MyGraphicView(QWidget *parent)
         : QGraphicsView(parent)
{
       // Настрою отображение виджета и его содержимого
       this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
       this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
       this->setAlignment(Qt::AlignCenter);
       this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Растягиваю содержимое по виджету
       setDragMode(ScrollHandDrag);

       // минимальные размеры виджета
       this->setMinimumHeight(50);
       this->setMinimumWidth(50);

       scene = new QGraphicsScene();       // Инициализяция сцену для отрисовки
       this->setScene(scene);              // Устанавливаю сцену в виджет
       scene->setSceneRect(0,0,3000,4000); // размер сцены
}

MyGraphicView::~MyGraphicView()
{ //TODO почистить все нарисанные объекты (в том числе при создании нового файла)

}
// Метод для удаления всех элементов из группы
void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    //по всем элементы сцены, и если они принадлежат группе
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
       }
    }
}

void MyGraphicView::createMovableLine()
{
  LinearItem *item = new LinearItem(); // перетаскиваемый прямоугольник
  item->setPos(100,100);           // координаты для нового объекта по умолчанию (TODO по хорошему его нужно рисовать мышкой)
  scene->addItem(item);            // вставляю элемент в графическую сцену

}

void MyGraphicView::createMovableRectangle()
{
  RectangleItem *item = new RectangleItem(); // перетаскиваемый прямоугольник
  item->setPos(100,100);           // координаты для нового объекта по умолчанию (TODO по хорошему его нужно рисовать мышкой)
  scene->addItem(item);            // вставляю элемент в графическую сцену

}

void MyGraphicView::removeGraphicsItem(MovableItem * removedItem)
{ if (removedItem)
  { scene->removeItem(removedItem);
    delete removedItem;
  }

}

