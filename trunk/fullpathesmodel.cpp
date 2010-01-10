#include "fullpathesmodel.h"

FullPathesModel::FullPathesModel(NetModel &netmodel, QObject *parent)
    : QAbstractItemModel(parent), netmodel(&netmodel)
{
    header[0] = QString::fromUtf8("Шифр полного пути");
    header[1] = QString::fromUtf8("Продолжительность пути");
    header[2] = QString::fromUtf8("Резерв времени пути");
    setupModelData();
    connect(this->netmodel, SIGNAL(updated()), this, SLOT(update()));
}

void FullPathesModel::update()
{
    clearModelData();
    setupModelData();
    reset();
}

void FullPathesModel::clearModelData()
{
    foreach(QList<QVariant> row, modelData)
        row.clear();
    modelData.clear();
}

void FullPathesModel::setupModelData()
{
    if (netmodel->isCorrect())
    {
        QList<Path> *pathes = netmodel->getFullPathes();
        foreach (Path path, *pathes)
        {
            QList<QVariant> row;
            row << path.code();
            row << path.weight();
            row << netmodel->getReserveTime(path);
            modelData << row;
        }
        delete pathes;
    }
}

FullPathesModel::~FullPathesModel()
{
    clearModelData();
}

int FullPathesModel::columnCount(const QModelIndex &) const
{
    return colCount;
}

QVariant FullPathesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    if (modelData.count()>0)
        return modelData[index.row()][index.column()];
    else
        return QVariant();
}

Qt::ItemFlags FullPathesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FullPathesModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole
        && section>-1 && section < colCount)
        return header[section];
    return QVariant();
}

QModelIndex FullPathesModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    return createIndex(row, column);
}

QModelIndex FullPathesModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}

int FullPathesModel::rowCount(const QModelIndex &/*parent*/) const
{
    return modelData.count();
}















