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

class MovableItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
  public:
      explicit MovableItem(QObject *parent = nullptr);
      ~MovableItem();


  signals:

  protected:
      QColor fillColor;   // цвет заливки
      int    borderWidth; // полщина линии
      QColor borderColor; // цвет линии

   public:
      virtual QColor getFillColor(){return fillColor;};
      virtual QColor getLineColor(){return borderColor;};
      virtual int getLineWidth(){return borderWidth;};
      virtual int getAngle() =0 ;

      virtual void setFillColor(QColor color);
      virtual void setLineColor(QColor color);
      virtual void setLineWidth(int lineWidth);
      virtual void setAngle(int newAngle)=0;

  public slots:

};

#endif // MOVEITEM_H
