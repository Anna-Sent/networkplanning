#include <QtGui>

#include "diagramitem.h"
#include "arrow.h"
#include "diagramtextitem.h"
#include "diagramscene.h"
#include <assert.h>

DiagramItem::DiagramItem(DiagramType diagramType, Event *ev, QMenu *contextMenu,
             QGraphicsItem *parent, DiagramScene *scene)
    : QGraphicsItem(parent, scene)
{
    myDiagramType = diagramType;
    myContextMenu = contextMenu;
    _event = ev;
    if (ev) setPos(ev->getPoint());
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    editing=false;
}

QRectF DiagramItem::boundingRect() const
{
    return QRectF(-radius(),-radius(),2*radius(),2*radius());
}

void DiagramItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * )
{
    QBrush brush = this->brush();
    QPen pen = this->pen();
    if (isSelected()&&static_cast<DiagramScene*>(scene())->getRenderSelection())
        pen.setWidth(4);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(boundingRect().adjusted(1,1,-1,-1));
    if (!_event) return;
    switch (myDiagramType) {
        case Circle:
            if (!editing)
                painter->drawText(boundingRect(),Qt::AlignCenter,QString::number(_event->getN()) ,&textBox);
            break;
    }
}

void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
    {
        arrows.removeAt(index);
    }
}

void DiagramItem::removeArrows()
{
    assert(arrows.count()==0);
}

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

QPixmap DiagramItem::image()
{
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.translate(30, 30);
    _pen.setColor(Qt::black);
    _pen.setWidth(2);
    paint(&painter,NULL,NULL);

    return pixmap;
}

void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    if (myContextMenu) myContextMenu->exec(event->screenPos());
}

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

void DiagramItem::updateText()
{
    update();
}

void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        DiagramTextItem *f = new DiagramTextItem ( this, scene() );
        f->setPlainText(QString::number(_event->getN()));
        f->setZValue ( 1000.0 );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setPos ( textBox.topLeft() );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setFocus();
        editing=true;
        DiagramScene *ds = static_cast<DiagramScene*>(scene());
        ds->editing(true);
    }
}

void DiagramItem::setValue(QString& val)
{
	if (!_event) return;
    bool ok=false;
    int num = val.toInt(&ok,10);
    DiagramScene *ds = static_cast<DiagramScene*>(scene());
    if (ok) ds->model()->setN(_event,num);
    ds->editing(false);
    editing=false;
}
