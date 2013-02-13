#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "netmodel.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    DiagramScene(QMenu *itemMenu, QObject *parent = 0);
    QFont font() const
    { return myFont; }
    QColor textColor() const
    { return myTextColor; }
    QColor itemColor() const
    { return myItemColor; }
    QColor lineColor() const
    { return myLineColor; }
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);
    void setModel(NetModel * model);
    void removeArrow(Arrow *arr);
    void removeEvent(DiagramItem *di);
    NetModel *model() const {return _model;}
    void editing(bool);

public slots:
    void clearModel();
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);
    void editorLostFocus(DiagramTextItem *item);
    void EventAdd();
    void EventAdd(int);
    void EventDel(Event*e);
    void ArrowAdd(Operation* op,int);
    void ArrowAdd(Operation* op);
    void ArrowDel(Operation* op);
    void OperationRedirect(Operation *op, Event* ev);
    void NChanged(Event *ev, int id);
    void deleteItem();
    void bringToFront(DiagramItem *item);
    void setRenderSelection(bool v) {_renderSelection=v;}
    bool getRenderSelection() {return _renderSelection;}
    void setSelected(Event *);
    void setSelected(Operation *);

signals:
    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);
    void actionsEnabled(bool);
    void changed();
    void selected(Event *);
    void selected(Operation *);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private slots:
    void debugDump();
    void onChange(const QList<QRectF> & region);
    void onSelectionChange();
private:
    bool isItemChange(int type);

    DiagramItem::DiagramType myItemType;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;
    DiagramTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;
    NetModel* _model;
    QList<DiagramItem*> devents;
    QList<Arrow*> darrows;
    qreal zval;
    bool _renderSelection;
};

#endif
