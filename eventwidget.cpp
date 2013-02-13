#include "eventwidget.h"
#include <QBitmap>
#include <QDebug>
#include <QDropEvent>
#include "arrowpointwidget.h"

EventWidget::EventWidget(Event *event, NetModel *model, QWidget *parent):QLabel(parent)
{
    _event=(event);
    _model=(model);
    QPixmap icon(30,30);
    QPainter p;
    p.begin(&icon);
    p.fillRect(icon.rect(),QColor(255,255,255));
    p.drawEllipse(icon.rect().adjusted(0,0,-1,-1));
    p.end();
    icon.setMask(icon.createMaskFromColor(QColor(255,255,255)));
    this->setPixmap(icon);
    setAcceptDrops(true);
}

QPoint EventWidget::center() const
{
    return QPoint(15,15);
}

void EventWidget::paintEvent(QPaintEvent *)
{
    QPainter p;
    p.begin(this);
    p.drawPixmap(0,0,*this->pixmap());
    p.drawText(QRect(0,0,30,30),Qt::AlignCenter,QString::number(_event->getN()));
    p.end();
}

void EventWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-arrowpointdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-arrowpointdata");
        qDebug() << "olala";

        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;
        Event * chev = 0;
        Operation * chop = 0;
        arrowpointwidget::EndRole role;
        int rol;
        dataStream >> rol;
        role=static_cast<arrowpointwidget::EndRole>(rol);
        dataStream.readRawData(reinterpret_cast<char*>(&chev  ),sizeof(chev));
        dataStream.readRawData(reinterpret_cast<char*>(&chop  ),sizeof(chop));
        switch(role) {
        case(arrowpointwidget::EBegin):{
                _model->disconnect(chev,chop);
                _model->connect(_event,chop);
                break;
            };
        case(arrowpointwidget::EEnd):{
               _model->disconnect(chop,chev);
               _model->connect(chop,_event);
               break;
            };
        };

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else
        if (event->mimeData()->hasFormat("application/x-arrowpointdata")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else
    {
        event->ignore();
    }
    update();
}
