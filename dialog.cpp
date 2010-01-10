#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(NetModel &netmodel, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    fpmodel = new FullPathesModel(netmodel, this);
    ui->label1->setText(QString::fromUtf8("Расчет полных путей"));
    ui->tableView1->setModel(fpmodel);
}

Dialog::~Dialog()
{
    delete ui;
    delete fpmodel;
}
