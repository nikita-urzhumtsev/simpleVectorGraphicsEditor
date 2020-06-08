#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
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


    void set(int width, int height, int angle);
};

class MovableItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
  public:
      explicit MovableItem(QObject *parent = nullptr);
      ~MovableItem();


  signals:

  protected:
      QColor fillColor;   // цвет заливки
      int    borderWidth; // полщина линии
      QColor borderColor; // цвет линии

      QPointF mousePressStartPos; // начальная позиция нажатия кнопки мыши

      virtual QRectF frameRect() const =0; // вычисление рамки фигуры
      qreal hotPointVisibleRadius(); //видимый радиус горячей точки
      qreal hotPointRadius();        //радиус зоны активности горячей точки

      const int fixedMark=3;                                   //размер кружка для горячей точки
      const Qt::GlobalColor selectedItemBorderColor=Qt::gray;  // цвет рамки выделенной фигуры
      const Qt::PenStyle    selectedItemLineStyle=Qt::DotLine; // тип линии для отрисовки рамки вокруг выделенной фигуры
      const Qt::GlobalColor hotPointColor=Qt::darkBlue;        // цвет горячей точки (за ктоорую можно перемещать узел, поворачивать или изменять размер)
      const Qt::PenStyle    hotPointLineStyle=Qt::SolidLine;   // тип линии горячей точки

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
