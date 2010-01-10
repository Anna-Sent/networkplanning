#ifndef FULLPATHESMODEL_H
#define FULLPATHESMODEL_H

#include <QAbstractItemModel>
#include "netmodel.h"

class FullPathesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    FullPathesModel(NetModel&, QObject *parent = 0);
    ~FullPathesModel();
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
    QList<Path> *pathes;
private slots:
    void update();
};

#endif // FULLPATHESMODEL_H
