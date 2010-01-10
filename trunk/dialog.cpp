#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(NetModel &netmodel, TreeModel *treemodel, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    fpmodel = new FullPathesModel(netmodel, this);
    ui->tableView->setModel(fpmodel);
}

Dialog::~Dialog()
{
    delete ui;
    delete fpmodel;
}
