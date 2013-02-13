#ifndef DIALOG_H
#define DIALOG_H

#include "netmodel.h"
#include "ui_dialog.h"
#include <QDialog>
#include <QTextCursor>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>

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
    void printModel()
    {
    }
    void printTables()
    {
    #ifndef QT_NO_PRINTER
        QPrinter printer;
        QPrintDialog *printDialog = new QPrintDialog(&printer, this);
        printDialog->setWindowTitle(QString::fromUtf8("Печать таблиц с расчетами"));
        if (printDialog->exec() != QDialog::Accepted)
            return ;
        ui->textBrowser->print(&printer);
    #endif
    }
private:
    Ui::Dialog *ui;
    NetModel *netmodel;
    QList<Event*> *eventsList;
    QList<Operation*> *operationsList;
    QList<Path> *pathes;

    void fillFullPathesData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void fillEventsData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void fillOperationsData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void displayTable(QTextCursor &cursor,
                      const QList<QVariant> &header, const QList< QList<QVariant> > &data);
    void _clearModel();
    void _setModel(NetModel &);
private slots:
    void beforeClear();
    void display();
    void clearCache()
    {
        if (eventsList)
        {
            delete eventsList;
            eventsList = NULL;
        }
        if (operationsList)
        {
            delete operationsList;
            operationsList = NULL;
        }
        if (pathes)
        {
            pathes = NULL;
        }
    }
};

#endif // DIALOG_H
