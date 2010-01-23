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

#include "arrow.h"
#include <math.h>
#include <assert.h>
#include "diagramtextitem.h"
#include "netmodel.h"
#include "diagramscene.h"

const qreal Pi = 3.14;

//! [0]
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem,
         QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene)
{
    assert(scene);
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}
//! [0]

/*Arrow::Arrow(Operation * op,QGraphicsItem *parent = 0, QGraphicsScene *scene = 0)
{
    my
}*/

void Arrow::setEndItem(DiagramItem* di)
{
    myEndItem = di;
    updatePosition();
}


//! [1]
QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
    QPen p = pen();
    QPen op = pen();
    p.setWidth(10);
    QGraphicsLineItem * me = const_cast<Arrow *>(this);
    me->setPen(p);
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    me->setPen(op);
    return path;
}
//! [2]

//! [3]
void Arrow::updatePosition()
{
    if (myStartItem==0||myEndItem==0) return;
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}
//! [3]

//! [4]
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (myStartItem==0||myEndItem==0) return;
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    if (isSelected()) myPen.setWidth(myPen.width()*2);
    painter->setPen(myPen);
    painter->setBrush(myColor);
//! [4] //! [5]

    QLineF centerLine(myStartItem->pos(), myEndItem->pos());
    QPolygonF endPolygon = myEndItem->polygon();
    QPointF p1 = endPolygon.first() + myEndItem->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < endPolygon.count(); ++i) {
    p2 = endPolygon.at(i) + myEndItem->pos();
    polyLine = QLineF(p1, p2);
    QLineF::IntersectType intersectType =
        polyLine.intersect(centerLine, &intersectPoint);
    if (intersectType == QLineF::BoundedIntersection)
        break;
        p1 = p2;
    }

    QPointF mid = (intersectPoint + myStartItem->pos())/2;

    painter->drawText(mid,QString::number(_op->getWaitTime()));

    setLine(QLineF(intersectPoint, myStartItem->pos()));
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;
//! [6] //! [7]
        painter->drawLine(line());
        painter->drawPolygon(arrowHead);
        /*if (isSelected()) {
            painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(4.0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(-8.0,-8.0);
        painter->drawLine(myLine);
    }*/
}
//! [7]

void Arrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        DiagramTextItem * f = new DiagramTextItem ( this, scene() );
        f->setPlainText(QString::number(_op->getWaitTime()));
        f->setZValue ( 1000.0 );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setPos ( event->pos() );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setFocus();
        DiagramScene *ds = dynamic_cast<DiagramScene*>(scene());
        if (ds)
        {
            ds->editing(true);
        }
        //QObject::connect(f,SIGNAL(changeN(Event*,int)), dynamic_cast<DiagramScene*>(scene())->model(),SLOT(setN(Event*,int)));
    }
}

void Arrow::setValue(QString &str)
{
        bool ok=false;
        double num = str.toDouble(&ok);
        if (!ok) return;
        //emit changeN(i->event(),num);
        DiagramScene *ds = dynamic_cast<DiagramScene*>(scene());
        if (ds)
        {
            ds->model()->setOperationWaitTime(_op,num);
            ds->editing(false);
        }
}
