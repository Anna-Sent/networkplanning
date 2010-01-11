#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "netmodel.h"
#include "fullpathesmodel.h"
#include "calceventmodel.h"
#include "calcoperationmodel.h"

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(NetModel &, QWidget *parent = 0);
    ~Dialog();
    void setModel(NetModel &);
private:
    Ui::Dialog *ui;
    FullPathesModel *fpmodel;
    CalcEventModel *cemodel;
    CalcOperationModel *comodel;
    NetModel *netmodel;
private slots:
    void setupLabelText();
    void beforeClear();
};

#endif // DIALOG_H
