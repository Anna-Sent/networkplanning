#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QLabel>
#include <netmodel.h>

class EventWidget : public QLabel
{
	Event *_event;
	NetModel *_model;
public:
	EventWidget() _event(0),_model(0){}
	EventWidget(const EventWidget& o) _event(o._event), _model(o._model) {}
	EventWidget(Event *event, NetModel *model) _event(event),_model(model) {}
	Event *event() {return _event;}
	NetModel *model() {return _model;}	
};

#endif
