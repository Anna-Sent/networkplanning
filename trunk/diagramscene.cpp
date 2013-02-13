#include "diagramscene.h"
#include "arrow.h"
#include <assert.h>
#include <QtGui>

DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    line = 0;
    textItem = 0;
    _model=0;
    _renderSelection=true;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
    connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(onChange(QList<QRectF>)));
}

void DiagramScene::clearModel()
{
    qDeleteAll(darrows);
    qDeleteAll(devents);
    darrows.clear();
    devents.clear();
    disconnect(_model, 0, this, 0);
    qDebug() << " children " << items().count();
}

void DiagramScene::setModel(NetModel* model)
{
    zval=0;

    if (_model!=0)
    {
        clearModel();
        disconnect(0,this,0);
    }
    _model=model;
    if (!model) return;

    QList<Event*>* events = _model->getEvents();
    for (int i=0; i < events->count(); ++i)
    {
        EventAdd(i);
    }
    foreach(Operation* op, *_model->getOperations())
    {
        ArrowAdd(op);
    }

    connect(model, SIGNAL(eventIdChanged (Event *, const int)), this, SLOT(NChanged(Event *, int)));
    connect(model, SIGNAL(afterEventAdd()), this, SLOT(EventAdd()));
    connect(model, SIGNAL(afterEventInsert(int)), this, SLOT(EventAdd(int)));
    connect(model, SIGNAL(updated()),this,SLOT(update()));
    connect(model, SIGNAL(beforeClear()),this,SLOT(clearModel()));
    connect(model, SIGNAL(afterOperationInsert(Operation*,int)), this, SLOT(ArrowAdd(Operation*,int)));
    connect(model, SIGNAL(afterOperationAdd(Operation*)), this, SLOT(ArrowAdd(Operation*)));
    connect(model, SIGNAL(beforeOperationDelete(Operation*)), this, SLOT(ArrowDel(Operation*)));
    connect(model, SIGNAL(beforeEventDelete(Event*)), this, SLOT(EventDel(Event*)));
    connect(model, SIGNAL(operationEndEventChanged(Operation*,Event*)), this, SLOT(OperationRedirect(Operation*,Event*)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChange()));
}

void DiagramScene::EventAdd(int index)
{
    Event *ev = _model->event(index);
    DiagramItem *item;
    item = new DiagramItem(DiagramItem::Circle,ev,0,0,this);
    item->setBrush(myItemColor);
    item->setPos(ev->getPoint());
    item->setZValue(zval);
    zval+=0.1;
    devents.insert(index,item);
    assert(devents.indexOf(item)==index);
    assert(devents.indexOf(item)==_model->getEvents()->indexOf(ev));
    update();
    emit itemInserted(item);
}

void DiagramScene::EventAdd()
{
    EventAdd(devents.size());
}

void DiagramScene::ArrowAdd(Operation *ev, int index)
{
    Q_UNUSED(index);
    DiagramItem *startItem,*endItem;
    int sid = _model->getEvents()->indexOf(ev->getBeginEvent());
    int eid = _model->getEvents()->indexOf(ev->getEndEvent());
    if (sid<0) return;
    startItem = devents.at(sid);
    if (eid>=0)
        endItem = devents.at(eid);
    else endItem=0;
    Arrow *arrow = new Arrow(startItem, endItem,0,this);
    arrow->setColor(myLineColor);
    startItem->addArrow(arrow);
    if (endItem) endItem->addArrow(arrow);
    arrow->setZValue(-1000.0);
    arrow->updatePosition();
    arrow->setOperation(ev);
    darrows.append(arrow);
    assert(darrows.indexOf(arrow)==_model->getOperations()->indexOf(ev));
    update();
}

void DiagramScene::ArrowAdd(Operation * ev)
{
    ArrowAdd(ev,darrows.size());
}

void DiagramScene::ArrowDel(Operation *op)
{
    int aid = _model->getOperations()->indexOf(op);
    if (aid<0) return;
    Arrow *arr = darrows.at(aid);
    assert(arr->startItem()->wrapsEvent(op->getBeginEvent()));
    removeArrow(arr);
}

void DiagramScene::EventDel(Event *ev)
{
    int eid = _model->getEvents()->indexOf(ev);
    if (eid<0) return;
    DiagramItem *di = devents.at(eid);
    assert(di->wrapsEvent(ev));
    di->removeArrows();
    removeEvent(di);
}

void DiagramScene::debugDump()
{
    qDebug() << "children" << items().count();
    int arrows=0;
    foreach(QGraphicsItem *gr,items())
    {
        Arrow *ar;
        if ((ar=dynamic_cast<Arrow*>(gr))) {
            qDebug() <<"arrow " << ar->startItem()->event()->getN() <<
                    ar->endItem()->event()->getN();
            ++arrows;
        }
    }
    qDebug() << "arrows" << arrows;
}

void DiagramScene::removeArrow(Arrow *arr)
{
    if (arr)
    {
        arr->startItem()->removeArrow(arr);
        if (arr->endItem()) arr->endItem()->removeArrow(arr);
        int count = darrows.count();
        darrows.removeAll(arr);
        assert(count-darrows.count()==1);
        arr->invalidate();
        arr->deleteLater();
    }
}

void DiagramScene::removeEvent(DiagramItem *di)
{
    if (di)
    {
        devents.removeAll(di);
        di->invalidate();
        di->deleteLater();
    }
}


void DiagramScene::OperationRedirect(Operation *op, Event *)
{
    int aid = _model->getOperations()->indexOf(op);
    if (aid<0) return;
    Arrow *arr = darrows.at(aid);
    DiagramItem *di = arr->endItem();
    if (di)
    {
        di->removeArrow(arr);
    }
    int nid = _model->getEvents()->indexOf(op->getEndEvent());
    if (nid>=0) {
        DiagramItem * ndi = devents.at(nid);
        arr->setEndItem(ndi);
        ndi->addArrow(arr);
        arr->updatePosition();
    } else arr->setEndItem(0);
}

void DiagramScene::editing(bool r)
{
    emit actionsEnabled(!r);
}

void DiagramScene::deleteItem()
{
    QGraphicsItem* gi = focusItem();
    if (dynamic_cast<DiagramTextItem*>(gi)) return;
     foreach (QGraphicsItem *item, selectedItems()) {
         if (item->type()==DiagramItem::Type)
         {
             DiagramItem* di =  qgraphicsitem_cast<DiagramItem *>(item);
             if (di->diagramType()==DiagramItem::Circle) {
                 Event * eve = di->event();
                 foreach(Operation *op,eve->getOutOperations())
                 {
                     _model->removeOperation(op);
                 }
                 foreach(Operation *op,eve->getInOperations())
                 {
                     _model->removeOperation(op);
                 }
                 _model->removeEvent(di->event());
             }
         } else
         if (item->type()==Arrow::Type)
         {
             Arrow* di =  qgraphicsitem_cast<Arrow *>(item);
             _model->removeOperation(di->getOperation());
         }
    }
}

void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item =
            qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}

void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}

void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        // At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    switch (myMode) {
        case InsertItem:
            if (_model->addEvent())
            {
                Event *ev = _model->last();
                ev->getPoint()=mouseEvent->scenePos().toPoint();
                devents.last()->setPos(mouseEvent->scenePos());
            }
            break;
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;
        case InsertText:
            textItem = new DiagramTextItem();
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem *)),
                    this, SLOT(editorLostFocus(DiagramTextItem *)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
                    this, SIGNAL(itemSelected(QGraphicsItem *)));
            addItem(textItem);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
        default:
            ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != 0 && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem =
                qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem =
                qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Event * se = startItem->event();
            Event * ee = endItem->event();
            Operation *op = _model->getOperationByEvents(se,ee);
            if (op==NULL)
            {
                Operation *op = new Operation();
                _model->connect(se, op, ee);
                _model->addOperation(op);
            }
        }
    }

    line = 0;
    emit changed();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::bringToFront(DiagramItem *ditem)
{
    QList<QGraphicsItem *> overlapItems = ditem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    ditem->setZValue(zValue);
   
}

bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

void DiagramScene::NChanged(Event *ev, int)
{
    int idx = _model->getEvents()->indexOf(ev);
    DiagramItem *di = devents.at(idx);
    assert(di->wrapsEvent(ev));
    di->updateText();
}

void DiagramScene::onChange(const QList<QRectF> & region)
{
    Q_UNUSED(region);
}

void DiagramScene::setSelected(Event * e)
{
	if (!_model) return;
	clearSelection();
	int idx = _model->getEvents()->indexOf(e);
	DiagramItem *di = devents.at(idx);
	di->setSelected(true);
}

void DiagramScene::setSelected(Operation * o)
{
	if (!_model) return;
	clearSelection();
	int idx = _model->getOperations()->indexOf(o);
	qDebug() << "set select " << idx;
	Arrow *a = darrows.at(idx);
	a->setSelected(true);
}

void DiagramScene::onSelectionChange()
{
	QList<QGraphicsItem *> li = selectedItems();
	if (li.empty()) return;
	QGraphicsItem* si=li.first();
	if (si->type()==DiagramItem::Type)
	{
		emit selected(static_cast<DiagramItem *>(si)->event());
	} else
	if (si->type()==Arrow::Type)
	{
		emit selected(static_cast<Arrow *>(si)->getOperation());
	}
	
}
