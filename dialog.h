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
    QTextBrowser *getTextBrowser() {return ui->textBrowser;}
private:
    Ui::Dialog *ui;
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
    void clearCashe()
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
