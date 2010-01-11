#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include "netmodel.h"

class EventWidget : public QLabel
{
    Q_OBJECT
    Event *_event;
    NetModel *_model;
public:
    EventWidget(): _event(0),_model(0) {}
    EventWidget(Event *event, NetModel *model, QWidget *parent);
    Event *event() {return _event;}
    NetModel *model() {return _model;}
    void paintEvent(QPaintEvent *event);
    int x() {return _event->getPoint().x();}
    int y() {return _event->getPoint().y();}
    QPoint center() const;
    void move(const QPoint& pos) { _event->getPoint()=pos; QLabel::move(pos); }
    bool wrapsEvent(Event* ev) { return _event==ev; }
    void dropEvent(QDropEvent *event);
};

#endif
