#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QColor>
#include <QColorDialog>
#include "moveitem.h"

QColor currentLineColor(Qt::cyan);
QColor currentFillColor(Qt::white);
int    currentLineWidth=1;
int    currentAngle=10;
QGraphicsItem * activeGraphicsItem = nullptr;
MainWindow * mainWindow = nullptr;

void  setActiveGraphicsItem(QGraphicsItem * newActiveItem)
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
    {   MoveItem * grItem=dynamic_cast<MoveItem *>(activeGraphicsItem);
        currentFillColor=grItem->getFillColor();
        currentLineColor=grItem->getLineColor();
        currentLineWidth=grItem->getLineWidth();
        currentAngle=grItem->getAngle();
        activeGraphicsItem->setAcceptHoverEvents(true);
        activeGraphicsItem->update();
        mainWindow->updateBottons();
    }

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //setFixedSize(800, 600); // Фиксированный размер окна
    ui->setupUi(this);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    updateBottons();

    mainWindow=this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonCreateRectangle_clicked()
{
    ui->graphicsView->createMovableRectangle();
}

void MainWindow::on_buttonCreatePolyline_clicked()
{
    // TODO: создать ломаную
}

void MainWindow::on_buttonDeleteGraphicsItem_clicked()
{
    if (activeGraphicsItem)
    {
       ui->graphicsView->removeGraphicsItem(activeGraphicsItem);
       activeGraphicsItem=nullptr;
    }
}

void MainWindow::on_buttonSelectColor_clicked()
{
  selectColor(ui->buttonSelectColor, currentFillColor);
}


void MainWindow::on_buttonSelectLineColor_clicked()
{
  selectColor(ui->buttonSelectLineColor, currentLineColor);
}

void MainWindow::selectColor(QPushButton * button, QColor & changedColor)
{ // TODO привести к правильной модели данных
    QColor newColor = QColorDialog::getColor(changedColor,parentWidget());
    if ( newColor != changedColor )
    {
        changedColor=newColor;
        setButtonColor(button, changedColor);
    }
}


void MainWindow::setButtonColor(QPushButton * button, const QColor & backgroundColor)
{   // делаю контрастную надпись - хитрая формула подсчета контрастности
    qreal R, G, B;
    R = 0.3 * backgroundColor.redF();
    G = 0.59 * backgroundColor.greenF();
    B = 0.11 * backgroundColor.blueF();

    QColor textColor ((R+G+B+1.0) < 1.280 ? Qt::white : Qt::black); // вычисляю цвет текста, контрастный фону
    // и задаю цвета текста и фона
    button->setStyleSheet("color: " + textColor.name() + "; background-color: " + backgroundColor.name());
}

void MainWindow::on_spinBoxLineWidth_valueChanged(int newWidth)
{
   currentLineWidth=newWidth;
}

void MainWindow::on_spinBoxAngle_valueChanged(int newAngle)
{
    currentAngle=newAngle;
}

void MainWindow::updateBottons()
{
    setButtonColor(ui->buttonSelectLineColor, currentLineColor);
    setButtonColor(ui->buttonSelectColor, currentFillColor);
    ui->spinBoxLineWidth->setValue(currentLineWidth);
    ui->spinBoxAngle->setValue(currentAngle);
}



void MainWindow::on_buttonChangeGraphicsItem_clicked()
{
   if (activeGraphicsItem)
   {
       MoveItem * item = dynamic_cast<MoveItem *>(activeGraphicsItem);
       item->setFillColor(currentFillColor);
       item->setLineColor(currentLineColor);
       item->setLineWidth(currentLineWidth);
       item->setAngle(currentAngle);
       item->update();// TODO может быть имеет смсыл перерисовать всю сцену целиком иначе возникают интересные эффекты при резких поворатах узких прямоугольников
   }

}
