#include "calcmodel.h"

void CalcModel::setup(NetModel &netmodel)
{
    setupHeader();
    setModel(netmodel);
}

void CalcModel::setModel(NetModel &netmodel)
{
    this->netmodel = &netmodel;
    setupModelData();
    connect(this->netmodel, SIGNAL(updated()), this, SLOT(update()));
    connect(this->netmodel, SIGNAL(beforeClear()), this, SLOT(beforeUpdate()));
    reset();
}

void CalcModel::beforeClear()
{
    clearModelData();
    disconnect(this, SLOT(beforeClear()));
    disconnect(this, SLOT(update()));
    netmodel = NULL;
    reset();
}

void CalcModel::update()
{
    clearModelData();
    setupModelData();
    reset();
}

void CalcModel::clearModelData()
{
    foreach(QList<QVariant> row, modelData)
        row.clear();
    modelData.clear();
}

int CalcModel::columnCount(const QModelIndex &) const
{
    return colCount();
}

QVariant CalcModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    if (modelData.count()>0)
        return modelData[index.row()][index.column()];
    else
        return QVariant();
}

Qt::ItemFlags CalcModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CalcModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole
        && section>-1 && section < colCount())
        return header[section];
    return QVariant();
}

QModelIndex CalcModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    return createIndex(row, column);
}

QModelIndex CalcModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}

int CalcModel::rowCount(const QModelIndex &/*parent*/) const
{
    return modelData.count();
}















