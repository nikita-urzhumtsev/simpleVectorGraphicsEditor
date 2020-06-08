#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QColor>
#include <QGraphicsItem>
#include "movableitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateBottons();

private:
    Ui::MainWindow *ui;

private slots:
    void on_buttonCreateRectangle_clicked();
    void on_buttonCreatePolyline_clicked();
    void on_buttonSelectColor_clicked();
    void on_buttonSelectLineColor_clicked();
    void setButtonColor(QPushButton * button, const QColor & backgroundColor);
    void selectColor(QPushButton * button, QColor & changedColor);
    void on_spinBoxLineWidth_valueChanged(int arg1);
    void on_spinBoxAngle_valueChanged(int arg1);
    void on_buttonDeleteGraphicsItem_clicked();
    void on_buttonChangeGraphicsItem_clicked();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();
};
#endif // MAINWINDOW_H
