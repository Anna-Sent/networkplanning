#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(close()));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
