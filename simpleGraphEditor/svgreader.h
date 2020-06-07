#ifndef SVGREADER_H
#define SVGREADER_H

#include <QList>
#include <QDomDocument>
#include <QFile>
#include "movableitem.h"
#include "rectangleitem.h"
#include "linearitem.h"

class SvgReader
{
private:
    QString filename;
    QDomDocument doc;
    QFile file;

public:
    SvgReader(const QString & fileName);
    ~SvgReader();

    QList<MovableItem *> getElements();
    QRectF getSizes();
    void parseRectangle(RectangleItem *rect, QDomNode & gNode, QDomElement & rectangle );
    void parseMultiline(LinearItem *line, QDomNode & gNode, QDomElement & rectangle );
    bool open();
    void close();
};
#endif // SVGREADER_H
