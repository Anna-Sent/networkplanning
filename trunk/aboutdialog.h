#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_aboutdialog.h"

namespace Ui
{
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
