#ifndef RECTANGLEITEM_H
#define RECTANGLEITEM_H

#include <QObject>
#include "movableitem.h"

class RectangleItem : public MovableItem
{
    Q_OBJECT
public:
    explicit RectangleItem(QObject *parent = nullptr);
    ~RectangleItem();


protected:
    int leftBaseCorner; // левый/верхний угол привязки прямоугольника
    int topBaseCorner;
    int width;          // ширина прямоугольника
    int height;         // высота прямоугольника
    int angle;          // угол поворта прямоугольника в градусах

    virtual QRectF frameRect() const; // вычисление рамки фигуры
    QRectF boundingRect() const; // вычисление обрамляющего прямоугольника

protected:
    bool isRotatingNow;         // флаг состояния вращения в настоящий момент
    bool widthIsChangingNow;    // флаг состояния изменения ширины в настоящий момент
    bool heightIsChangingNow;   // флаг состояния изменения высоты в настоящий момент
    HotPoints hotPoints;        // горячие точки для управления размером и поворота фигуры
    int mousePressStartAngle;   // начальный угол, когда была нажата кнопка мыши
    int mousePressStartWidth;   // начальный ширина, когда была нажата кнопка мыши
    int mousePressStartHeight;  // начальный высота, когда была нажата кнопка мыши


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

public:
    virtual int getAngle() {return angle;};
    virtual void setAngle(int newAngle);
    virtual void setLineWidth(int lineWidth);
    void setHeightWidth(int height, int width);

};

#endif // RECTANGLEITEM_H
