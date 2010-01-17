/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "diagramscene.h"
#include "arrow.h"
#include <assert.h>

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
}

void DiagramScene::setModel(NetModel* model)
{

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
    foreach(Event* ev,*events)
    {
        /*EventWidget *ic = new EventWidget(ev,model,this);
                //ic->setText(QString::number(ev->getN()));
                ic->move(ev->getPoint());
                ic->show();
                ic->setAttribute(Qt::WA_DeleteOnClose);*/
        EventAdd(0,ev);
    }
    foreach(Operation* op,*_model->getOperations())
    {
        /*EventWidget *ic = new EventWidget(ev,model,this);
                //ic->setText(QString::number(ev->getN()));
                ic->move(ev->getPoint());
                ic->show();
                ic->setAttribute(Qt::WA_DeleteOnClose);*/
        ArrowAdd(0,op);
    }

    connect(model, SIGNAL(eventNameChanged(QObject *, Event *, const QString &)), this, SLOT(eventNameChanged(QObject *, Event *, const QString &)));
    connect(model, SIGNAL(eventIdChanged (QObject *, Event *, const int)), this, SLOT(NChanged(QObject*,Event*,int)));
    connect(model, SIGNAL(afterEventAdd(QObject*,Event*)), this, SLOT(EventAdd(QObject*,Event*)));
    connect(model, SIGNAL(afterEventInsert(QObject*,Event*,int)), this, SLOT(EventAdd(QObject*,Event*,int)));
    connect(model, SIGNAL(beforeEventDelete(QObject*,Event*)),this, SLOT(DeleteEvent(QObject*,Event*)));
    connect(model, SIGNAL(updated()),this,SLOT(update()));
    connect(model, SIGNAL(beforeClear()),this,SLOT(clearModel()));
    connect(model, SIGNAL(afterOperationInsert(QObject*,Operation*,int)), this, SLOT(ArrowAdd(QObject*,Operation*,int)));
    connect(model, SIGNAL(afterOperationInsert(QObject*,Operation*)), this, SLOT(ArrowAdd(QObject*,Operation*)));
    connect(model, SIGNAL(beforeOperationDelete(QObject*,Operation*)), this, SLOT(ArrowDel(QObject*,Operation*)));
    connect(model, SIGNAL(beforeEventDelete(QObject*,Event*)), this, SLOT(EventDel(QObject*,Event*)));
    connect(model, SIGNAL(operationEndEventChanged(QObject*,Operation**,Event*)), this, SLOT(OperationRedirect(QObject*,Operation**,Event*)));
}

void DiagramScene::EventAdd(QObject *, Event * ev,int index)
{
    /*EventWidget *ic = new EventWidget(ev,_model,this);
    //ic->setText(QString::number(ev->getN()));
    ic->move(ev->getPoint());
    ic->show();
    ic->setAttribute(Qt::WA_DeleteOnClose);
    recreatePoints();
    update();*/
    DiagramItem *item;
    item = new DiagramItem(DiagramItem::Circle,ev,0,0,this);
    item->setPos(ev->getPoint());
    item->setZValue(0);
    devents.insert(index,item);
    assert(devents.indexOf(item)==index);
    assert(devents.indexOf(item)==_model->getEvents()->indexOf(ev));
}

void DiagramScene::EventAdd(QObject *o,Event *ev)
{
    EventAdd(o,ev,devents.size());
}

void DiagramScene::ArrowAdd(QObject *, Operation * ev,int index)
{
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
    addItem(arrow);
    arrow->updatePosition();
    //startItem->update(startItem->boundingRect());
    darrows.insert(index,arrow);
    assert(darrows.indexOf(arrow)==index);
    assert(darrows.indexOf(arrow)==_model->getOperations()->indexOf(ev));
/*    DiagramItem *item;
    item = new DiagramItem(DiagramItem::Circle,ev,0,0,this);
    item->setPos(ev->getPoint());
    devents.insert(index,item);*/
}

void DiagramScene::ArrowAdd(QObject *o, Operation * ev)
{
    ArrowAdd(o,ev,darrows.size());
}

void DiagramScene::ArrowDel(QObject *o, Operation *op)
{
    int aid = _model->getOperations()->indexOf(op);
    if (aid<0) return;
    Arrow *arr = darrows.at(aid);
    assert(arr->startItem()->wrapsEvent(op->getBeginEvent()));
    if (arr->endItem()) assert(arr->endItem()->wrapsEvent(op->getEndEvent()));
    removeArrow(arr);
    delete arr;
}

void DiagramScene::EventDel(QObject *,Event *ev)
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


void DiagramScene::OperationRedirect(QObject*o, Operation **op, Event *ev)
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
/*
    DiagramItem *item;
    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item->setBrush(myItemColor);
            addItem(item);
            item->setPos(mouseEvent->scenePos());
            emit itemInserted(item);
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
    }*/
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
            Arrow *arrow = new Arrow(startItem, endItem,0,this);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();
        }
    }
//! [12] //! [13]
    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//! [13]

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
void DiagramScene::NChanged(QObject *o, Event *ev, int id)
{
    int idx = _model->getEvents()->indexOf(ev);
    DiagramItem *di = devents.at(idx);
    assert(di->wrapsEvent(ev));
    di->updateText();
}
