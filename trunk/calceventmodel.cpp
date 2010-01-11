#include "calceventmodel.h"

CalcEventModel::CalcEventModel(NetModel &netmodel)
{
    setup(netmodel);
}

void CalcEventModel::setupHeader()
{
    header.clear();
    header << QString::fromUtf8("i");//Код события");
    header << QString::fromUtf8("t р.(i)");//Ранний срок свершения события");
    header << QString::fromUtf8("t п.(i)");//Поздний срок свершения события");
    header << QString::fromUtf8("R(i)");//Резерв времени");
}

int CalcEventModel::colCount() const
{
    return 4;
}

void CalcEventModel::setupModelData()
{
    if (netmodel && netmodel->isCorrect())
    {
        foreach (Event *event, *(netmodel->getEvents()))
        {
            QList<QVariant> row;
            row << event->getN();
            row << netmodel->getEarlyEndTime(event);
            row << netmodel->getLaterEndTime(event);
            row << netmodel->getReserveTime(event);
            modelData << row;
        }
    }
}















