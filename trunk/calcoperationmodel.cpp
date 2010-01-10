#include "calcoperationmodel.h"

CalcOperationModel::CalcOperationModel(NetModel &netmodel)
{
    setup(netmodel);
}

void CalcOperationModel::setupHeader()
{
    header.clear();
    header << QString::fromUtf8("i-j");//Код работы");
    header << QString::fromUtf8("t(i-j)");//Продолжительность работы");
    header << QString::fromUtf8("t р.н.(i-j)");//Ранний срок начала работы");
    header << QString::fromUtf8("t п.н.(i-j)");//Поздний срок начала работы");
    header << QString::fromUtf8("t р.о.(i-j)");//Ранний срок окончания работы");
    header << QString::fromUtf8("t п.о.(i-j)");//Поздний срок окончания работы");
    header << QString::fromUtf8("t п.(i-j)");//Полный резерв времени");
    header << QString::fromUtf8("t с.(i-j)");//Свободный резерв времени");
}

int CalcOperationModel::colCount() const
{
    return 8;
}

void CalcOperationModel::setupModelData()
{
    if (netmodel->isCorrect())
    {
        QList<Operation*> *operations = netmodel->getOperations();
        foreach (Operation *operation, *operations)
        {
            QList<QVariant> row;
            row << operation->getCode();
            row << operation->getWaitTime();
            row << netmodel->getEarlyStartTime(operation);
            row << netmodel->getLaterStartTime(operation);
            row << netmodel->getEarlyEndTime(operation);
            row << netmodel->getLaterEndTime(operation);
            row << netmodel->getFullReserveTime(operation);
            row << netmodel->getFreeReserveTime(operation);
            modelData << row;
        }
    }
}















