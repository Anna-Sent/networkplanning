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

#include "diagramtextitem.h"
#include "diagramscene.h"

//! [0]
DiagramTextItem::DiagramTextItem(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsTextItem(parent, scene)
{
   /* setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable); */
}
//! [0]

//! [1]
QVariant DiagramTextItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    /*if (change == QGraphicsItem::ItemPositionChange) {
    }*/
    return value;
}
//! [1]

//! [2]
void DiagramTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    editingFinished();
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}
//! [2]

//! [5]
void DiagramTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}
//! [5]


void DiagramTextItem::editingFinished()
{
    Arrow *a;
    DiagramItem *i;
  QString text = toPlainText().split( "\n" ).first();

  if ( parentItem() != NULL ) {
    switch ( parentItem()->type() ) {
    case DiagramItem::Type: {
        i=static_cast<DiagramItem*>(parentItem());
        bool ok;
        int num = text.toInt(&ok,10);
        //i->event()->setN(num);
        emit changeN(i->event(),num);
        break;
    }
/*    case SceneItem_NodeType:
      w = ( SceneItem_Node* )parentItem();
      if ( text != "" )
        (( GraphicsScene* )scene() )->setData( w->index, text, customRole::CustomLabelRole );
      break;*/
    default:
      qDebug() << __FUNCTION__ << "ERROR, case not handled yet";
      break;
    }
    this->deleteLater();
  }
}

void DiagramTextItem::keyPressEvent ( QKeyEvent * event )
{
    //qDebug() << event->key() << Qt::Key_Enter;
    if (event->key()==Qt::Key_Return) editingFinished();
    QGraphicsTextItem::keyPressEvent(event);
}
