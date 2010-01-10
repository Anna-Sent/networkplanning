#ifndef CALCEVENTMODEL_H
#define CALCEVENTMODEL_H

#include "calcmodel.h"

class CalcEventModel : public CalcModel
{
    Q_OBJECT
public:
    CalcEventModel(NetModel &);
protected:
    void setupModelData();
    void setupHeader();
    int colCount() const;
};

#endif // CALCEVENTMODEL_H
