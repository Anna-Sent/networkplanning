#ifndef POSITIONING_H
#define POSITIONING_H

#include "netmodel.h"
#include <QWidget>

class Position {
public:
	virtual void position(NetModel* model) = 0;
};

class PlanarPosition : public Position {
QWidget* _widget;
public:
	PlanarPosition(QWidget* widget) {_widget=widget; }
	virtual void position(NetModel* model);
};

#endif
