#include "arrowpointwidget.h"

arrowpointwidget::arrowpointwidget(EndRole role,Operation* op,Event * ev,QWidget*parent):QLabel(parent),role(role),op(op),ev(ev)
{
    QPixmap px(5,5);
    px.fill(QColor(0,255,0));
    setPixmap(px);
    setAttribute(Qt::WA_DeleteOnClose);
}
