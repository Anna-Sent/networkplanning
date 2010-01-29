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
             QGraphicsItem *parent, DiagramScene *scene)
    : QGraphicsItem(parent, scene)
{
    //assert(scene);
    myDiagramType = diagramType;
    myContextMenu = contextMenu;
    _event = ev;
    if (ev) setPos(ev->getPoint());

    //QPainterPath path;
    //switch (myDiagramType) {
    //    case Circle:
            //path.addEllipse(QPointF(0,0),25,25);
            //myPolygon = path.toFillPolygon();
            //break;
/*        default:
            myPolygon << QPointF(-120, -80) << QPointF(-70, 80)
                      << QPointF(120, 80) << QPointF(70, -80)
                      << QPointF(-120, -80);
            break;*/
    //}
    //setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    editing=false;
}
//! [0]
QRectF DiagramItem::boundingRect() const
{
    return QRectF(-radius(),-radius(),2*radius(),2*radius());
}

void DiagramItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget ) {
    //QGraphicsPolygonItem::paint(painter,style,widget);
    QBrush brush = this->brush();
    QPen pen = this->pen();
    if (isSelected()&&static_cast<DiagramScene*>(scene())->getRenderSelection())
        pen.setWidth(4);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(boundingRect().adjusted(1,1,-1,-1));
    /*QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::DashLine);
    painter->drawRect(boundingRect());*/
    if (!_event) return;
    switch (myDiagramType) {
        case Circle:
            if (!editing)
                painter->drawText(boundingRect(),Qt::AlignCenter,QString::number(_event->getN()) ,&textBox);
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
/*        if (arrow->endItem()!=this)
        {
            if (arrow->endItem()) {
                arrow->endItem()->removeArrow(arrow);
            }
        } else
        {
            arrow->setEndItem(0);
        }*/
        arrows.removeAt(index);
    }
}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    /*foreach (Arrow *arrow, arrows) {
        //removeArrow(arrow);
       //static_cast<DiagramScene*>(scene())->removeArrow(arrow);
    }*/
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
QPixmap DiagramItem::image()
{
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    //painter.setPen(QPen(Qt::black, 2));
    painter.translate(30, 30);
    //painter.drawPolyline(myPolygon);*/
    _pen.setColor(Qt::black);
    _pen.setWidth(2);
    paint(&painter,NULL,NULL);

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
    if (!_event) return QVariant();
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
        DiagramScene *ds = static_cast<DiagramScene*>(scene());
        ds->editing(true);
        //QObject::connect(f,SIGNAL(changeN(Event*,int)), dynamic_cast<DiagramScene*>(scene())->model(),SLOT(setN(Event*,int)));
    }
}

void DiagramItem::setValue(QString& val)
{
	if (!_event) return;
        bool ok=false;
        int num = val.toInt(&ok,10);
        //emit changeN(i->event(),num);
        DiagramScene *ds = static_cast<DiagramScene*>(scene());
        if (ok) ds->model()->setN(_event,num);
        ds->editing(false);
        editing=false;
}
