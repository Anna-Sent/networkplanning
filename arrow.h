#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>
#include <QObject>
#include "diagramitem.h"
#include "netmodel.h"

QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE
class DiagramScene;

class Arrow : public QObject,public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    Arrow(DiagramItem *startItem, DiagramItem *endItem,
      QGraphicsItem *parent = 0, DiagramScene *scene = 0);
    Arrow(Operation * op,QGraphicsItem *parent = 0, DiagramScene *scene = 0);

    int type() const
        { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)
        { myColor = color; }
    DiagramItem *startItem() const
        { return myStartItem; }
    DiagramItem *endItem() const
        { return myEndItem; }
    void setEndItem(DiagramItem *);
    Operation * getOperation() {return _op;}
    void setOperation(Operation *op) {_op=op;}
    void setValue(QString &str);
    void invalidate() {myStartItem=0;myEndItem=0;_op=0;}

public:
    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    DiagramItem *myStartItem;
    DiagramItem *myEndItem;
    QColor myColor;
    QColor myCritColor;
    QPolygonF arrowHead;
    QPolygonF textLabel;
    QRectF textLabelR;
    Operation * _op;
    bool editing;
};

#endif
