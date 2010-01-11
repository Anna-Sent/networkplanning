#ifndef ARROWPOINTWIDGET_H
#define ARROWPOINTWIDGET_H

#include <QLabel>
#include "netmodel.h"

class arrowpointwidget : public QLabel
{
        Q_OBJECT
public:
    enum EndRole {EBegin,EEnd};
    EndRole role;
    Operation *op;
    Event * ev;
    arrowpointwidget(EndRole role,Operation* op,Event * ev,QWidget *parent=0);
};

#endif // ARROWPOINTWIDGET_H
