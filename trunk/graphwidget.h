#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "eventwidget.h"
#include <QFrame>
#include <QMap>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class GraphWidget : public QFrame
{
    Q_OBJECT

    NetModel *_model;

public:
    GraphWidget(QWidget *parent=0);
    void setModel(NetModel *model);

public slots:
    void updatePositions();
    void eventNameChanged(QObject *, Event *, const QString &);
    void eventIdChanged(QObject *, Event *, const int id);
    void EventAdd(QObject *, Event *);
    void EventAdd(QObject *, Event *, int);
    void DeleteEvent(QObject *, Event *);
    void clearModel();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QLine adjustLine(QPoint p1, QPoint p2, int margin);
    void drawArrow(QPainter &p, QLine l);
    void recreatePoints();
};

#endif
