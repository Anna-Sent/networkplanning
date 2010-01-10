#include "fullpathesmodel.h"

FullPathesModel::FullPathesModel(NetModel &netmodel)
{
    setup(netmodel);
}

void FullPathesModel::setupHeader()
{
    header.clear();
    header << QString::fromUtf8("L");//Шифр полного пути");
    header << QString::fromUtf8("t(L)");//Продолжительность пути");
    header << QString::fromUtf8("R(L)");//Резерв времени пути");
}

int FullPathesModel::colCount() const
{
    return 3;
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










