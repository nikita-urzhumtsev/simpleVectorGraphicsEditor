#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>

class HotPoints
{
public:
    qreal Ax; // Точки А и (-А) - середина одной стороны (и параллельной ей)
    qreal Ay; // точка A = (Ax, Ay) - за нее можно менять размер фигуры (width)
    qreal Dx; // Точка D и (-D) - середина второй стороны (и параллельной ей)
    qreal Dy; // точка D = (Dx, Dy) - за нее можно менять размер фигуры (height)

    qreal Bx; // Точка B - один угол, (-B) - угол по диагонали от первого
    qreal By; // точка B = (Bx, By) - за нее можно крутить фигуру
    qreal Cx; // Точка C - второй угол, (-С) - угол по диагонали от второго
    qreal Cy; // точка C = (Cx, Cy) - за нее можно крутить фигуру

    qreal radius; // радиус горячей точки
    qreal visibleRadius; // видимый радиус горячей точки

    void set(int width, int height, int angle, int borderWidth);
};

class MoveItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
  public:
      explicit MoveItem(QObject *parent = 0);
      ~MoveItem();


  signals:

  private:
      int leftBaseCorner; // левый/верхний угол привязки прямоугольника
      int topBaseCorner;
      int width;          // ширина прямоугольника
      int height;         // высота прямоугольника
      int angle;          // угол поворта прямоугольника в градусах
      QColor fillColor;   // цвет заливки
      int    borderWidth; // полщина линии
      QColor borderColor; // цвет линии

      bool isRotatingNow;         // флаг состояния вращения в настоящий момент
      bool widthIsChangingNow;    // флаг состояния изменения ширины в настоящий момент
      bool heightIsChangingNow;   // флаг состояния изменения высоты в настоящий момент
      QPointF mousePressStartPos; // начальная позиция нажатия кнопки мыши
      HotPoints hotPoints;        // горячие точки для управления размером и поворота фигуры
      int mousePressStartAngle;   // начальный угол, когда была нажата кнопка мыши
      int mousePressStartWidth;   // начальный ширина, когда была нажата кнопка мыши
      int mousePressStartHeight;  // начальный высота, когда была нажата кнопка мыши

      QRectF boundingRect() const;
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
      void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      void mousePressEvent(QGraphicsSceneMouseEvent *event);
      void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

   public:
      QColor getFillColor(){return fillColor;};
      QColor getLineColor(){return borderColor;};
      int getLineWidth(){return borderWidth;};
      int getAngle() {return angle;};

      void setFillColor(QColor color);
      void setLineColor(QColor color);
      void setLineWidth(int lineWidth);
      void setAngle(int newAngle);

  public slots:

};

#endif // MOVEITEM_H
