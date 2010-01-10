#ifndef CALCOPERATIONMODEL_H
#define CALCOPERATIONMODEL_H

#include <QAbstractItemModel>
#include "netmodel.h"

class CalcOperationModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CalcOperationModel(NetModel &, QObject *parent = 0);
    /*~CalcOperationModel();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
private:
    NetModel *netmodel;
    QList<QVariant[]> *data;
private slots:
    void update();*/
};

#endif // CALCOPERATIONMODEL_H
