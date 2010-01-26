#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "netmodel.h"
//#include "fullpathesmodel.h"
//#include "calceventmodel.h"
//#include "calcoperationmodel.h"
#include <QTextCursor>
#include "ui_dialog.h"
#include "diagramscene.h"
#include <QPrinter>
#include <QPrintDialog>

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(NetModel &, DiagramScene *, QWidget *parent = 0);
    ~Dialog();
    void setModel(NetModel &);
    //QTextBrowser *getTextBrowser() {return ui->textBrowser;}
    void print()
    {
    #ifndef QT_NO_PRINTER
        //QPrinter printer;
        QPrintDialog *printDialog = new QPrintDialog(&printer, this);
        printDialog->setWindowTitle(QString::fromUtf8("Печать сетевой модели"));
        if (ui->textBrowser->textCursor().hasSelection())
            printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
        if (printDialog->exec() != QDialog::Accepted)
            return ;
        //printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
        ui->textBrowser->print(&printer);
    #endif
    }
private:
    Ui::Dialog *ui;
    QPrinter printer;
    NetModel *netmodel;
    DiagramScene *scene;
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
            //delete pathes;
            pathes = NULL;
        }
    }
};

#endif // DIALOG_H
