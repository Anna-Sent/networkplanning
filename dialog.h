#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
/*
#include <QtGui>
#include "treemodel.h"
#include "netmodel.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TreeModel *treemodel;
    NetModel netmodel;
    QDialog dialog;
    void insertEvent(int index);
    void insertOperation(int index);
private slots:
    void addEvent();
    void insertEvent();
    void deleteEvent();
    void addOperation();
    void insertOperation();
    void deleteOperation();
    void check();
    void calc();
    void currentChanged(const QModelIndex &, const QModelIndex &);
};

#endif // MAINWINDOW_H




*/












