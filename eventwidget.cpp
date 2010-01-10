#include "eventwidget.h"
#include <QBitmap>
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
    }
    QPoint EventWidget::center() const
    {
        return QPoint(15,15);
    }

    void EventWidget::paintEvent(QPaintEvent */*event*/)
    {
        QPainter p;
        p.begin(this);
        p.drawPixmap(0,0,*this->pixmap());
        p.drawText(QRect(0,0,30,30),Qt::AlignCenter,QString::number(_event->getN()) );
        p.end();
    }
