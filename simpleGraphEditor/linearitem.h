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
    QPolygonF lineNodes;                       // узлы полилинии
    bool nodeIsChangingNow;                    // флаг перемещения узла в настоящий момент
    QPointF mousePressStartNode;               // узел, который сейчас двигаю

    virtual QRectF frameRect() const;          // вычисление рамки фигуры
    QRectF boundingRect() const;               // вычисление обрамляющего прямоугольника

    // выполняет операцию, заданную в func над ребром из lineNodes, близким к позиции pos
    // возвращает true, если такое ребро было найдено
    template< typename Func > bool findSuitableLineAndDo(QPointF pos, Func func );

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);        // обработка перемещения мышки (на узле - его перемещение, и на фигуре целиком - ее перемещение)
    void mousePressEvent(QGraphicsSceneMouseEvent *event);       // здесь живет начало перемещения узла или начало перемещения фигуры целиком
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);     // здесь живет окончание перемещения узла и обработка "склеивания" узлов
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);        // здесь живет изменение курсора мыши в зависимости от того, что можно сделать
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event); // здесь живет создание новго узла

public:
    virtual int getAngle() {return 0;};  // пустая функцая, в принципе можно было бы сделать еще вращение фигуры
    virtual void setAngle(int newAngle); // пустая функцая, в принципе можно было бы сделать еще вращение фигуры
    virtual void setLineWidth(int lineWidth);
};

#endif // LINEARITEM_H
