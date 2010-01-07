#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void currentChanged(const QModelIndex &, const QModelIndex &);
};

#endif // MAINWINDOW_H

















