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

#include "diagramitem.h"
#include "arrow.h"
#include "diagramtextitem.h"
#include "diagramscene.h"
#include <assert.h>
//! [0]
DiagramItem::DiagramItem(DiagramType diagramType, Event * ev, QMenu *contextMenu,
             QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPolygonItem(parent, scene)
{
    //assert(scene);
    myDiagramType = diagramType;
    myContextMenu = contextMenu;
    _event = ev;
    if (ev) setPos(ev->getPoint());

    QPainterPath path;
    switch (myDiagramType) {
        /*case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional:
            myPolygon << QPointF(-100, 0) << QPointF(0, 100)
                      << QPointF(100, 0) << QPointF(0, -100)
                      << QPointF(-100, 0);
            break;
        case Step:
            myPolygon << QPointF(-100, -100) << QPointF(100, -100)
                      << QPointF(100, 100) << QPointF(-100, 100)
                      << QPointF(-100, -100);
            break;*/
        case Circle:
            path.addEllipse(QPointF(0,0),25,25);
            myPolygon = path.toFillPolygon();
            if (ev) {
                /*text = new DiagramTextItem(this,scene);
                text->setPlainText(QString::number(ev->getN()));*/
                //text->setPos(pos());
                //text->setPos(pos());
                //text->setPos(
            }
            if (scene) {
                //QList<QGraphicsItem*> toGroup;
                //toGroup.append(this);
                //toGroup.append(text);
                //QGraphicsItemGroup *grp =  scene->createItemGroup(toGroup);
                //grp->setFlags(QGraphicsItem::ItemIsMovable);
                //grp->setFlags(QGraphicsItem::ItemIsSelectable);
            }
            break;
        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                      << QPointF(120, 80) << QPointF(70, -80)
                      << QPointF(-120, -80);
            break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}
//! [0]

void DiagramItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget ) {
    QGraphicsPolygonItem::paint(painter,style,widget);
    QBrush brush;
    switch (myDiagramType) {
        case Circle:
            if (!editing)
                painter->drawText(QRect(-30,-30,60,60),Qt::AlignCenter,QString::number(_event->getN()) ,&textBox);
            break;
    }
}

//QString::number(_event->getN())

//! [1]
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);


    if (index != -1)
    {
        if (arrow->startItem()==this)
        {
            if (arrow->endItem()) {
                arrow->endItem()->removeArrow(arrow);
            }
            /*QGraphicsScene *qgs=scene();
            DiagramScene *ds = dynamic_cast<DiagramScene*> (qgs);
            assert(ds);
            if (ds) ds->removeArrow(arrow);
            delete arrow;*/
        } else
        {
            arrow->setEndItem(0);
        }
        arrows.removeAt(index);
    }
}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        //arrow->endItem()->removeArrow(arrow);
        //arrow->startItem()->removeArrow(arrow);
        removeArrow(arrow);
    }
    assert(arrows.count()==0);
}
//! [2]

//! [3]
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 2));
    painter.translate(30, 30);
    painter.drawPolyline(myPolygon);

    return pixmap;
}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    if (myContextMenu) myContextMenu->exec(event->screenPos());
}
//! [5]

//! [6]
QVariant DiagramItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }

        _event->getPoint()=value.toPointF().toPoint();
    }

    return value;
}
//! [6]
void DiagramItem::updateText()
{
     //text->setPlainText(QString::number(_event->getN()));
    update();
}

void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        DiagramTextItem * f = new DiagramTextItem ( this, scene() );
        f->setPlainText(QString::number(_event->getN()));
        f->setZValue ( 1000.0 );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setPos ( /*event->pos()*/textBox.topLeft() );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setFocus();
        editing=true;

        DiagramScene *ds = dynamic_cast<DiagramScene*>(scene());
        if (ds)
        {
            ds->editing(true);
        }
        //QObject::connect(f,SIGNAL(changeN(Event*,int)), dynamic_cast<DiagramScene*>(scene())->model(),SLOT(setN(Event*,int)));
    }
}

void DiagramItem::setValue(QString& val)
{
        bool ok=false;
        int num = val.toInt(&ok,10);
        if (!ok) return;
        //emit changeN(i->event(),num);
        DiagramScene *ds = dynamic_cast<DiagramScene*>(scene());
        if (ds)
        {
            ds->model()->setN(_event,num);
            ds->editing(false);
        }
        editing=false;
}
