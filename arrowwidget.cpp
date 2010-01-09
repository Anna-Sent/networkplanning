#include "arrowwidget.h"
#include <QPainter>

void ArrowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), Qt::white);
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
    painter.end();
}


