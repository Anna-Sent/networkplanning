#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "netmodel.h"
#include "fullpathesmodel.h"
#include "calceventmodel.h"
#include "calcoperationmodel.h"
#include <QTextCursor>

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
    void fillFullPathesData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void fillEventsData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void fillOperationsData(QList<QVariant> &header, QList< QList<QVariant> > &data);
    void displayTable(QTextCursor &cursor, QList<QVariant> &header, QList< QList<QVariant> > &data);
private slots:
    void setupLabelText();
    void beforeClear();
    void display();
};

#endif // DIALOG_H
