#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QBrush>
#include <QPen>
#include <QObject>
#include <assert.h>

class Event;
class DiagramTextItem;
QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE
class DiagramScene;
class Arrow;

class DiagramItem : public QObject,public QGraphicsItem
{
public:
    enum { Type = UserType + 15 };
    enum DiagramType { Circle };

    DiagramItem(DiagramType diagramType, Event* ev, QMenu *contextMenu,
        QGraphicsItem *parent = 0, DiagramScene *scene = 0);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    DiagramType diagramType() const
        { return myDiagramType; }
    QPolygonF polygon() const
        { return myPolygon; }
    void addArrow(Arrow *arrow);
    QPixmap image();
    int type() const
        { return Type; }
    bool wrapsEvent(Event* ev) { return _event==ev; }
    Event * event() const {return _event;}
    void updateText();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);
    void setValue(QString &val);
    QRectF boundingRect() const;
    QBrush brush() const { return _brush; }
    QPen pen() const { return _pen; }
    void setBrush(const QBrush& br) {_brush=br;}
    double radius() const {return 25.0;}
    void invalidate() {_event=0;}

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    DiagramType myDiagramType;
    QPolygonF myPolygon;
    QMenu *myContextMenu;
    QList<Arrow *> arrows;
    DiagramTextItem *text;
    Event *_event;
    QRectF textBox;
    bool editing;
    QBrush _brush;
    QPen _pen;
};

#endif
