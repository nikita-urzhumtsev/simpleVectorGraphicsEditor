#ifndef SELECTCOLORBUTTON_H
#define SELECTCOLORBUTTON_H


#include <QPushButton>
#include <QColor>

class SelectColorButton : public QPushButton
{
    Q_OBJECT
public:
    SelectColorButton(QWidget*);

    void setColor( const QColor& color );
    const QColor& getColor();

public slots:
    void updateColor();
    void changeColor();

private:
    QColor color;
};

#endif // SELECTCOLORBUTTON_H
