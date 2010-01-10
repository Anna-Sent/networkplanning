#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "netmodel.h"
#include "fullpathesmodel.h"
#include "treemodel.h"

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
public:
    Dialog(NetModel &, QWidget *parent = 0);
    ~Dialog();
private:
    Ui::Dialog *ui;
    FullPathesModel *fpmodel;
};

#endif // DIALOG_H
