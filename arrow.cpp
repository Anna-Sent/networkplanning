#include <QtGui>

#include "arrow.h"
#include "diagramtextitem.h"
#include "netmodel.h"
#include "diagramscene.h"
#include <math.h>
#include <assert.h>

const qreal Pi = 3.14;

Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem,
         QGraphicsItem *parent, DiagramScene *scene)
    : QGraphicsLineItem(parent, scene)
{
    assert(scene);
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    myCritColor = Qt::red;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    editing=false;
}

void Arrow::setEndItem(DiagramItem* di)
{
    myEndItem = di;
    updatePosition();
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width()+5) ;
    qreal mx = qMin(line().p1().x(),line().p2().x());
    qreal my = qMin(line().p1().y(),line().p2().y());
    QSizeF size(qAbs(line().p2().x() - line().p1().x())+1,
                                      qAbs(line().p2().y() - line().p1().y())+1);
    QRectF result = QRectF(QPointF(mx,my), size)
        .normalized().united(textLabelR).united(arrowHead.boundingRect())
        .adjusted(-extra, -extra, extra, extra);
    return result;
}

QPainterPath Arrow::shape() const
{
    QPen p = pen();
    QPen op = pen();
    p.setWidth(10);
    QGraphicsLineItem * me = const_cast<Arrow *>(this);
    me->setPen(p);
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    path.addPolygon(textLabel);
    me->setPen(op);
    return path;
}

void Arrow::updatePosition()
{
    if (myStartItem==0||myEndItem==0) return;
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (myStartItem==0||myEndItem==0||_op==0) return;
    QLineF centerLine(myStartItem->pos(), myEndItem->pos());
    if (centerLine.length()<myStartItem->radius()+myEndItem->radius()) return;

    QPen myPen = pen();
    qreal arrowSize = 20;
    if (isSelected()&&static_cast<DiagramScene*>(scene())->getRenderSelection()) myPen.setWidth(myPen.width()*2);
    if (_op->inCriticalPath())
    {
        painter->setBrush(myCritColor);
        myPen.setColor(myCritColor);
    }
    else
    {
        painter->setBrush(myColor);
        myPen.setColor(myColor);
    }
    if (_op->getWaitTime()==0) myPen.setStyle(Qt::DashLine);
    painter->setPen(myPen);

    centerLine.setLength(centerLine.length()-endItem()->radius());

    QPointF mid = centerLine.pointAt(0.5);
    QLineF diff = centerLine.normalVector().unitVector();
    diff.setLength(20.0);
    qreal ddy = diff.dy();
    qreal ddx = diff.dx();
    if (centerLine.normalVector().dy()>1)
    {
        ddx=-ddx;
        ddy=-ddy;
    }
    textLabel.clear();
    if (!editing)
        painter->drawText(QRectF(mid+QPointF(ddx,ddy),QSizeF(200.0,200.0)),Qt::AlignLeft||Qt::AlignTop,QString::number(_op->getWaitTime()),&textLabelR);
    QPainterPath path;
    path.addPolygon(textLabelR);
    textLabel=path.toFillPolygon();

    QLineF resLine = QLineF(centerLine.p2(), myStartItem->pos());
    setLine(resLine);

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
	myPen.setStyle(Qt::SolidLine);
	painter->setPen(myPen);

    painter->drawPolygon(arrowHead);
}

void Arrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        DiagramTextItem * f = new DiagramTextItem ( this, scene() );
        f->setPlainText(QString::number(_op->getWaitTime()));
        f->setZValue ( 1000.0 );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setPos ( textLabelR.topLeft() );
        f->setTextInteractionFlags ( Qt::TextEditorInteraction );
        f->setFocus();
        DiagramScene *ds = static_cast<DiagramScene*>(scene());
        editing=true;
        ds->editing(true);
    }
}

void Arrow::setValue(QString &str)
{
        bool ok=false;
        double num = str.toDouble(&ok);
        DiagramScene *ds = static_cast<DiagramScene*>(scene());
        editing=false;
        if (ok) ds->model()->setOperationWaitTime(_op,num);
        ds->editing(false);
}
