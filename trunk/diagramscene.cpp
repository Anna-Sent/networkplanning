#include "diagramscene.h"
#include "arrow.h"
#include <assert.h>
#include <QtGui>

//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    //myItemType = DiagramItem::Step;
    line = 0;
    textItem = 0;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}
//! [0]

void DiagramScene::clearModel()
{
    foreach(Arrow *ar,darrows)
    {
        removeArrow(ar);
    }
    foreach(DiagramItem *ar,devents)
    {
        removeEvent(ar);
    }
    qDeleteAll(darrows);
    qDeleteAll(devents);
    darrows.clear();
    devents.clear();
    disconnect(_model, 0, this, 0);
}

void DiagramScene::setModel(NetModel* model)
{
    zval=0;

    if (_model!=0)
    {
        foreach(QObject* o,children())
        {
            delete o;
        }
        disconnect(0,this,0);
    }
    _model=model;
    if (!model) return;

    QList<Event*>* events = _model->getEvents();
    //for(int evi=0;evi<events->size();++evi)
    for (int i=0; i < events->count(); ++i)
    {
        /*EventWidget *ic = new EventWidget(ev,model,this);
                //ic->setText(QString::number(ev->getN()));
                ic->move(ev->getPoint());
                ic->show();
                ic->setAttribute(Qt::WA_DeleteOnClose);*/
        EventAdd(i);
    }
    foreach(Operation* op,*_model->getOperations())
    {
        /*EventWidget *ic = new EventWidget(ev,model,this);
                //ic->setText(QString::number(ev->getN()));
                ic->move(ev->getPoint());
                ic->show();
                ic->setAttribute(Qt::WA_DeleteOnClose);*/
        ArrowAdd(op);
    }

    //connect(model, SIGNAL(eventNameChanged(QObject *, Event *, const QString &)), this, SLOT(eventNameChanged(QObject *, Event *, const QString &)));
    connect(model, SIGNAL(eventIdChanged (Event *, const int)), this, SLOT(NChanged(Event*,int)));
    connect(model, SIGNAL(afterEventAdd()), this, SLOT(EventAdd()));
    connect(model, SIGNAL(afterEventInsert(int)), this, SLOT(EventAdd(int)));
    connect(model, SIGNAL(updated()),this,SLOT(update()));
    connect(model, SIGNAL(beforeClear()),this,SLOT(clearModel()));
    connect(model, SIGNAL(afterOperationInsert(Operation*,int)), this, SLOT(ArrowAdd(Operation*,int)));
    connect(model, SIGNAL(afterOperationAdd(Operation*)), this, SLOT(ArrowAdd(Operation*)));
    connect(model, SIGNAL(beforeOperationDelete(Operation*)), this, SLOT(ArrowDel(Operation*)));
    connect(model, SIGNAL(beforeEventDelete(Event*)), this, SLOT(EventDel(Event*)));
    connect(model, SIGNAL(operationEndEventChanged(Operation**,Event*)), this, SLOT(OperationRedirect(Operation**,Event*)));
    //connect(this, SIGNAL(itemInserted(DiagramItem*)), this, SLOT(bringToFront(DiagramItem*)));
}

void DiagramScene::EventAdd(int index)
{
    /*EventWidget *ic = new EventWidget(ev,_model,this);
    //ic->setText(QString::number(ev->getN()));
    ic->move(ev->getPoint());
    ic->show();
    ic->setAttribute(Qt::WA_DeleteOnClose);
    recreatePoints();
    update();*/
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
    emit itemInserted(item);
}

void DiagramScene::EventAdd()
{
    EventAdd(devents.size());
}

void DiagramScene::ArrowAdd(Operation *ev, int /*index*/)
{
    //index = darrows.size();
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
    //startItem->update(startItem->boundingRect());
    darrows.append(arrow);// insert(index, arrow);
    //assert(darrows.indexOf(arrow)==index);
    //qDebug() << darrows.indexOf(arrow) <<" "<< _model->getOperations()->indexOf(ev);
    assert(darrows.indexOf(arrow)==_model->getOperations()->indexOf(ev));
/*    DiagramItem *item;
    item = new DiagramItem(DiagramItem::Circle,ev,0,0,this);
    item->setPos(ev->getPoint());
    devents.insert(index,item);*/
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
    if (arr->endItem()) assert(arr->endItem()->wrapsEvent(op->getEndEvent()));
    arr->startItem()->removeArrow(arr);
    removeArrow(arr);
    delete arr;
}

void DiagramScene::EventDel(Event *ev)
{
    int eid = _model->getEvents()->indexOf(ev);
    if (eid<0) return;
    DiagramItem *di = devents.at(eid);
    assert(di->wrapsEvent(ev));
    //devents.at(eid)->removeArrows();
    di->removeArrows();
    removeEvent(di);
    delete di;
    //devents.removeAt(eid);
}

void DiagramScene::removeArrow(Arrow *arr)
{
    if (arr)
    {
        int count = darrows.count();
        darrows.removeAll(arr);
        removeItem(arr);
        assert(count-darrows.count()==1);
    }
}

void DiagramScene::removeEvent(DiagramItem *di)
{
    if (di)
    {
        devents.removeAll(di);
        removeItem(di);
    }
}


void DiagramScene::OperationRedirect(Operation **op, Event *ev)
{
    /*ArrowDel(o,*op);
    ArrowAdd(*/
    int aid = _model->getOperations()->indexOf(*op);
    if (aid<0) return;
    Arrow* arr = darrows.at(aid);
    DiagramItem *di = arr->endItem();
    if (di)
    {
        di->removeArrow(arr);
    }
    int nid = _model->getEvents()->indexOf(ev);
    if (nid>=0) {
        DiagramItem * ndi = devents.at(nid);
        arr->setEndItem(ndi);
        ndi->addArrow(arr);
        arr->updatePosition();
    } else arr->setEndItem(0);

}

void DiagramScene::deleteItem()
{
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

//! [1]
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
//! [1]

//! [2]
void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type)) {
        DiagramTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item =
            qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}
//! [3]

//! [4]
void DiagramScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type)) {
        QGraphicsTextItem *item =
            qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}
//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    /*if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }*/
}
//! [5]

//! [6]
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item;
    //Event *ev;
    switch (myMode) {
        case InsertItem:
            /*item = new DiagramItem(myItemType, myItemMenu);
            item->setBrush(myItemColor);
            addItem(item);
            item->setPos(mouseEvent->scenePos());*/
            //
            //ev = new Event();
            //ev->setN(_model->generateId());
            if (_model->addEvent())
            {
                Event *ev = _model->last();
                ev->getPoint()=mouseEvent->scenePos().toPoint();
                devents.last()->setPos(mouseEvent->scenePos());
            }
            break;
//! [6] //! [7]
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;
//! [7] //! [8]
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
//! [8] //! [9]
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
//! [9]

//! [10]
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
//! [10]

//! [11]
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
//! [11] //! [12]

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem =
                qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem =
                qgraphicsitem_cast<DiagramItem *>(endItems.first());
/*            Arrow *arrow = new Arrow(startItem, endItem,0,this);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();*/
            Event * se = startItem->event();
            Event * ee = endItem->event();
            Operation *op = _model->getOperationByEvents(se,ee);
            if (op==NULL)
            {
                Operation *op = new Operation();
                //op->setBeginEvent(se);
                //op->setEndEvent(ee);
                _model->connect(se, op, ee);
                _model->addOperation(op);
            }
        }
    }
//! [12] //! [13]
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

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

//! [14]
bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
//! [14]
void DiagramScene::NChanged(Event *ev, int id)
{
    int idx = _model->getEvents()->indexOf(ev);
    DiagramItem *di = devents.at(idx);
    assert(di->wrapsEvent(ev));
    di->updateText();
}
