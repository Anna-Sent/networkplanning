#ifndef FULLPATHESMODEL_H
#define FULLPATHESMODEL_H

#include "calcmodel.h"

class FullPathesModel : public CalcModel
{
//    Q_OBJECT
public:
    FullPathesModel(NetModel &);
protected:
    void setupModelData();
    void setupHeader();
    int colCount() const;
};

#endif // FULLPATHESMODEL_H










