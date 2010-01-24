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
    //bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex());
    //bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void fill(QComboBox *, const QModelIndex &) const;
    //TreeItem *getRootItem() {return rootItem;}
    void setModel(NetModel &);

    void addEvent(const QModelIndex &selected);
    void insertEvent(const QModelIndex &selected);
    void addOperation(const QModelIndex &selected);
    void insertOperation(const QModelIndex &selected);
    void removeEvent(const QModelIndex &selected);
    void removeOperation(const QModelIndex &selected);
private:
    void setupModelData(TreeItem *parent);
    //void process(Event *, TreeItem *);
    //void process(Operation*,TreeItem*);

    TreeItem *rootItem;
    NetModel *netmodel;
    QList<QVariant> header;
    int getIndex(Event *);
    int getIndex(Operation *);
private slots:
    void eventIdChanged(Event *, int);
    void eventNameChanged(Event *, const QString &);
    void operationEndEventChanged(Operation *, Event *);
    void operationNameChanged(Operation *, const QString &);
    void operationWaitTimeChanged(Operation *, double);
    void afterEventAdd();
    void beforeEventDelete(Event *);
    void afterOperationAdd(Operation *);
    void beforeOperationDelete(Operation *);
    void afterEventInsert(int);
    void afterOperationInsert(Operation *, int);
    void updated();
    void beforeClear();
};
//! [0]

#endif

















