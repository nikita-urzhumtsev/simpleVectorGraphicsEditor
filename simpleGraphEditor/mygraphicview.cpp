#include "mygraphicview.h"
#include "movableitem.h"
#include "linearitem.h"
#include "rectangleitem.h"
#include "mainwindow.h"
#include <QtSvg/QtSvg>

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

       scene = new QGraphicsScene();       // Инициализация сцены для отрисовки
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

void MyGraphicView::saveToFile(const QString & documentFileName)
{
    QSvgGenerator generator;
    generator.setFileName(documentFileName);    // путь к SVG-файлу (векторная графика)
    generator.setSize(QSize(scene->width(), scene->height()));          // Устанавливаем размеры рабочей области документа
    generator.setViewBox(QRect(0, 0, scene->width(), scene->height())); // Устанавливаем рабочую область в координатах
    generator.setTitle("SVG file");                                     // Титульное название документа
    generator.setDescription("Пример картинки, созданной программой Н.С. Уржумцева");    // Описание документа

    QPainter painter;

    MovableItem * savedItem=activeGraphicsItem; // активный элемент в афйле показывать не надо
    activeGraphicsItem = nullptr;               // поэому его запомню и обнулю

    painter.begin(&generator);  // Устанавливаю устройство, где буду рисовать
    scene->render(&painter);    // Сцена сама себя умеет рисовать
    painter.end();              // Закрываю устройство вывода

    activeGraphicsItem=savedItem; // восстанавливаю активный элемент на экране
}

