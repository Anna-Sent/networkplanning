#include "fullpathesmodel.h"

FullPathesModel::FullPathesModel(NetModel &netmodel, QObject *parent)
    : QAbstractItemModel(parent), netmodel(&netmodel)
{
    if (this->netmodel->isCorrect())
        pathes = this->netmodel->getFullPathes();
    else
        pathes = NULL;
    connect(this->netmodel, SIGNAL(updated()), this, SLOT(update()));
}

void FullPathesModel::update()
{
    if (pathes)
        delete pathes;
    if (netmodel->isCorrect())
        pathes = netmodel->getFullPathes();
    else
        pathes = NULL;
    reset();
}

FullPathesModel::~FullPathesModel()
{
    if (pathes)
        delete pathes;
}

int FullPathesModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant FullPathesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();
    if (pathes)
        switch (index.column())
        {
            case 0:
            {return pathes->at(index.row()).code();}
            case 1:
            {return pathes->at(index.row()).weight();}
            case 2:
            {return netmodel->getReserveTime(pathes->at(index.row()));}
            default:
            {return QVariant();}
        }
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
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section)
        {
            case 0: return QString::fromUtf8("Шифр полного пути");
            case 1: return QString::fromUtf8("Продолжительность пути");
            case 2: return QString::fromUtf8("Резерв времени пути");
            default: return QVariant();
        };
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
    if (pathes)
        return pathes->count();
    else
        return 0;
}















