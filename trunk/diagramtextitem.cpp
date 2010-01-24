#include <QtGui>

#include "diagramtextitem.h"
#include "arrow.h"
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
        i->setValue(text);
        break;
    };
    case Arrow::Type : {
        a=static_cast<Arrow*>(parentItem());
        a->setValue(text);
        break;
    }
    default:
      qDebug() << __FUNCTION__ << "ERROR, case not handled yet";
      break;
    }
  }
      this->deleteLater();
}

void DiagramTextItem::keyPressEvent ( QKeyEvent * event )
{
    //qDebug() << event->key() << Qt::Key_Enter;
    if (event->key()==Qt::Key_Return) editingFinished();
    QGraphicsTextItem::keyPressEvent(event);
}
