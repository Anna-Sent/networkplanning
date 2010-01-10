#ifndef CALCOPERATIONMODEL_H
#define CALCOPERATIONMODEL_H

#include "calcmodel.h"

class CalcOperationModel : public CalcModel
{
    Q_OBJECT
public:
    CalcOperationModel(NetModel &);
protected:
    void setupModelData();
    void setupHeader();
    int colCount() const;
};

#endif // CALCOPERATIONMODEL_H
