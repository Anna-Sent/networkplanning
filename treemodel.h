#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QComboBox>
#include "netmodel.h"

class TreeItem;

//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(NetModel&, QObject *parent = 0);//const QString &data, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::DropActions supportedDropActions() const
    {
        return /*Qt::CopyAction |*/ Qt::MoveAction;
    }
    void fill(QComboBox *, const QModelIndex &) const;
    TreeItem *getRootItem() {return rootItem;}

private:
    void setupModelData(TreeItem *parent);
    void process(Event *, TreeItem *, QSet<int> &);
    void process(Operation*,TreeItem*);

    TreeItem *rootItem;
    NetModel *netmodel;
};
//! [0]

#endif

















