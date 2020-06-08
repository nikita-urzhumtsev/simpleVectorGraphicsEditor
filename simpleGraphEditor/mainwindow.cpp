#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globaldata.h"
#include <QPainter>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include "movableitem.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setFixedSize(800, 600);
    ui->setupUi(this);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsView->setFixedSize(720,578);
    updateBottons();
    globalData.mainWindow=this;
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
    ui->graphicsView->createMovableLine();
}

void MainWindow::on_buttonDeleteGraphicsItem_clicked()
{
    if (globalData.activeGraphicsItem)
    {
       ui->graphicsView->removeGraphicsItem(globalData.activeGraphicsItem);
       globalData.activeGraphicsItem=nullptr;
    }
}

void MainWindow::on_buttonSelectColor_clicked()
{
  selectColor(ui->buttonSelectColor, globalData.currentFillColor);
}


void MainWindow::on_buttonSelectLineColor_clicked()
{
  selectColor(ui->buttonSelectLineColor, globalData.currentLineColor);
}

void MainWindow::selectColor(QPushButton * button, QColor & changedColor)
{
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
   globalData.currentLineWidth=newWidth;
}

void MainWindow::on_spinBoxAngle_valueChanged(int newAngle)
{
    globalData.currentAngle=newAngle;
}

void MainWindow::updateBottons()
{
    setButtonColor(ui->buttonSelectLineColor, globalData.currentLineColor);
    setButtonColor(ui->buttonSelectColor, globalData.currentFillColor);
    ui->spinBoxLineWidth->setValue(globalData.currentLineWidth);
    ui->spinBoxAngle->setValue(globalData.currentAngle);
}



void MainWindow::on_buttonChangeGraphicsItem_clicked()
{
   if (globalData.activeGraphicsItem)
   {
       MovableItem * item = dynamic_cast<MovableItem *>(globalData.activeGraphicsItem);
       item->setFillColor(globalData.currentFillColor);
       item->setLineColor(globalData.currentLineColor);
       item->setLineWidth(globalData.currentLineWidth);
       item->setAngle(globalData.currentAngle);
       item->update();
   }

}

void MainWindow::on_actionOpen_triggered()
{ // открыть файл
    QString fileToOpen = QFileDialog::getOpenFileName(this, "Open SVG",  globalData.documentFileName, "SVG files (*.svg)");

    if (fileToOpen.isEmpty())
           return;

    globalData.documentFileName = fileToOpen;

    ui->graphicsView->readFromFile(globalData.documentFileName);
}

void MainWindow::on_actionSave_triggered()
{
  ui->graphicsView->saveToFile(globalData.documentFileName);
}

void MainWindow::on_actionSave_As_triggered()
{
   QString newPath = QFileDialog::getSaveFileName(this, "Save SVG",  globalData.documentFileName, "SVG files (*.svg)");

   if (newPath.isEmpty())
          return;

   globalData.documentFileName = newPath;

   ui->graphicsView->saveToFile(globalData.documentFileName);
}

void MainWindow::on_actionExit_triggered()
{ // вежливо было бы поставить защиту от выхода с несохраненными данными
  // но в ТЗ в явной форме не указано, точка для возможного уточнения спецификации, но не в рамках тестовой задачки
  this->close();
}
