#include "svgreader.h"
#include <QPen>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QtMath>

SvgReader::SvgReader(const QString & fileName):
    filename(fileName), file(filename)
{

}

SvgReader::~SvgReader()
{
  close();
}


bool SvgReader::open()
{
    // Если файл не открылся или не удалось его связать с DOM-ом
    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file)) return true;
        file.close();
    }
    return false;

}

void SvgReader::close()
{
   if (file.isOpen()) file.close();
   doc.clear();

}

//*******************************************************************************************************************
// нужные элементы в файле выглядят примерно так:
/* НЕПОВЕРНУТЫЙ ПРЯМОУГОЛЬНИК:
<g fill="#ffffff" fill-opacity="1" stroke="#00ffff" stroke-opacity="1" stroke-width="1" stroke-linecap="square" stroke-linejoin="bevel" transform="matrix(1,0,0,1,100,100)"
 font-family="MS Shell Dlg 2" font-size="8.25" font-weight="400" font-style="normal"
 >
<rect x="-60" y="-30" width="120" height="60"/>
</g>

   ПОВЕРНУТЫЙ ПРЯМОУГОЛЬНИК:
<g fill="#ffffff" fill-opacity="1" stroke="#00ffff" stroke-opacity="1" stroke-width="1" stroke-linecap="square" stroke-linejoin="bevel" transform="matrix(0.707107,0.707107,-0.707107,0.707107,100,100)"
font-family="MS Shell Dlg 2" font-size="8.25" font-weight="400" font-style="normal"
>
<rect x="-60" y="-30" width="120" height="60"/>
</g>

   ЛОМАНАЯ ИЗ ОДНОГО ОТРЕЗКА:
<g fill="#ffffff" fill-opacity="1" stroke="#00ffff" stroke-opacity="1" stroke-width="1" stroke-linecap="square" stroke-linejoin="bevel" transform="matrix(1,0,0,1,100,100)"
font-family="MS Shell Dlg 2" font-size="8.25" font-weight="400" font-style="normal"
>
<polyline fill="none" vector-effect="none" points="0,0 100,100 " />
</g>

   ЛОМАНАЯ ИЗ НЕСКОЛЬКИХ ОТРЕЗКОВ (больше одного):
<g fill="#ffffff" fill-opacity="1" stroke="#00ffff" stroke-opacity="1" stroke-width="1" stroke-linecap="square" stroke-linejoin="bevel" transform="matrix(1,0,0,1,100,100)"
font-family="MS Shell Dlg 2" font-size="8.25" font-weight="400" font-style="normal"
>
<polyline fill="none" vector-effect="none" points="0,0 66,23 32,74 100,100 " />
</g>
**********************************************************************************************************************/

void SvgReader::parseRectangle(RectangleItem *rect, QDomNode & gNode, QDomElement & rectangle )
{
    // читаю размеры из тега rect
    int height, width;
    height=rectangle.attribute("height").toInt();
    width=rectangle.attribute("width").toInt();
    rect->setHeightWidth(height, width);

    // цвета заливки, линии, и все остальное надо брать из тега g
    QDomElement gElement = gNode.toElement();

    QColor fillColor(gElement.attribute("fill", "#ffffff"));    // цвет заливки
    rect->setFillColor(fillColor);

    // цвет и толщина линии
    QColor lineColor(gElement.attribute("stroke", "#000000"));
    int lineWidth=gElement.attribute("stroke-width", "0").toInt();
    rect->setLineColor(lineColor);
    rect->setLineWidth(lineWidth);


    // надо получить смещение и угол поворота
    // из атрибута transform="matrix(cosAngle,sinAngle, zzz , zzz , смещение по оси Х, смещение по оси Y)"
    QString str=gElement.attribute("transform", "0");
    // TODO защита от неправильных данных и вынести в отдельную процедуру
    int posLeftBracket=str.indexOf("(");
    int posRightBracket=str.indexOf(")");
    str=str.mid(posLeftBracket+1, posRightBracket-posLeftBracket-1);
    QStringList transformList=str.split(",");
    qreal cosAngle=transformList[0].toFloat();
    rect->setAngle(qRadiansToDegrees(qAcos(cosAngle)));
    int posX=transformList[4].toInt();
    int posY=transformList[5].toInt();
    rect->setPos(posX,posY);

}

void SvgReader::parseMultiline(LinearItem *line, QDomNode & gNode, QDomElement & polyline )
{
    // читаю список узлов из тега points
    QString points=polyline.attribute("points");
    QStringList strPointList=points.split(" "); // точки разделены пробелами
    for (auto point: strPointList)
    if (!point.isEmpty())
    {
        QStringList coordinates=point.split(","); // координаты в паре разделены запятой
        if (coordinates.count()==2)
        {
          qreal x=coordinates[0].toFloat();
          qreal y=coordinates[1].toFloat();
          line->appendPoint(QPointF(x,y));
        }
    }

    // цвета заливки, линии, и все остальное надо брать из тега g
    QDomElement gElement = gNode.toElement();
    // цвет и толщина линии
    QColor lineColor(gElement.attribute("stroke", "#000000"));
    int lineWidth=gElement.attribute("stroke-width", "0").toInt();
    line->setLineColor(lineColor);
    line->setLineWidth(lineWidth);


    // надо получить смещение
    // из атрибута transform="matrix(zzz,zzz, zzz , zzz , смещение по оси Х, смещение по оси Y)"
    QString str=gElement.attribute("transform", "0");
    // TODO защита от неправильных данных и вынести в отдельную процедуру
    int posLeftBracket=str.indexOf("(");
    int posRightBracket=str.indexOf(")");
    str=str.mid(posLeftBracket+1, posRightBracket-posLeftBracket-1);
    QStringList transformList=str.split(",");
    int posX=transformList[4].toInt();
    int posY=transformList[5].toInt();
    line->setPos(posX,posY);

}


QList<MovableItem *> SvgReader::getElements()
{
    QList<MovableItem *> movableItemList;    // сюда буду добавлять элементы из файла


    // Если файл не открыт
    if (!file.isOpen())
        return movableItemList;    // то список пустой


    // Ищу все объекты с тегом g
    QDomNodeList gList = doc.elementsByTagName("g");
    for (int i = 0; i < gList.size(); i++)
    {
        QDomNode gNode = gList.item(i);
        QDomElement rectangle = gNode.firstChildElement("rect");
        // есть элемент c тегом rect
        if (!rectangle.isNull())
        {   // создам новый прямоугольник
            RectangleItem *rect = new RectangleItem();
            parseRectangle(rect, gNode, rectangle);
            movableItemList.append(rect);
        }
        else
        { // может быть это мультилиния
            QDomElement polyline = gNode.firstChildElement("polyline");
            // Если полученный элементы не нулевой, то
            if (!polyline.isNull())
            {   // создам новую ломаную
                LinearItem *line = new LinearItem();
                parseMultiline(line, gNode, polyline);
                movableItemList.append(line);
            }

        }
    }

    file.close();
    return movableItemList;
}

QRectF SvgReader::getSizes()
{

    // Если файл не открыт
    if (!file.isOpen())
        return QRectF(0,0,3000,4000);       // то задам размер для сцены по умолчанию

    // пример тега из файла:
    //    <svg width="1058.33mm" height="1411.11mm"
    //      viewBox="0 0 3000 4000"
    //      xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"  version="1.2" baseProfile="tiny">
    //    <потом идет описание всех элементов ...>
    //    </svg>

    // Ищу тегом svg у него в аттрибуте viewbox живет размер сцены
    QDomNodeList list = doc.elementsByTagName("svg");
    if(list.length() > 0) {
        QDomElement svgElement = list.item(0).toElement();
        QStringList parameters = svgElement.attribute("viewBox").split(" ");
        return QRectF(parameters.at(0).toInt(),
                      parameters.at(1).toInt(),
                      parameters.at(2).toInt(),
                      parameters.at(3).toInt());
    }

    // что-то пошло не так - в файле нет тега svg, верну значения по умолчанию
    return QRectF(0,0,3000,4000); // TODO загнать эти цифры в константы (использую не меньше, чем в 3 местах)
}
