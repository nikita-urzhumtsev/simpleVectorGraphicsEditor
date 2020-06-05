#ifndef LINEARITEM_H
#define LINEARITEM_H

#include <QObject>
#include "movableitem.h"

class LinearItem : public MovableItem
{
    Q_OBJECT
public:
    explicit LinearItem(QObject *parent = nullptr);
    ~LinearItem();

private:
    QPolygonF lineNodes;// узлы полилинии
    bool nodeIsChangingNow;    // флаг состояния изменения ширины в настоящий момент
    QPointF mousePressStartNode; // узел, который сейчас двигаю

    virtual QRectF frameRect() const; // вычисление рамки фигуры
    QRectF boundingRect() const; // вычисление обрамляющего прямоугольника

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

public:
    virtual int getAngle() {return 0;};
    virtual void setAngle(int newAngle);
    virtual void setLineWidth(int lineWidth);
};

#endif // LINEARITEM_H
