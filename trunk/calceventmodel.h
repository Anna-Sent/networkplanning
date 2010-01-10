#ifndef CALCEVENTMODEL_H
#define CALCEVENTMODEL_H

#include <QAbstractItemModel>
#include "netmodel.h"

class CalcEventModel : public QAbstractItemModel
{
public:
    CalcEventModel(NetModel &, QObject *parent = 0);
};

#endif // CALCEVENTMODEL_H
