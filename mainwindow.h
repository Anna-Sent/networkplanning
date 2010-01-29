#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "treemodel.h"
#include "netmodel.h"
#include "dialog.h"
#include "diagramscene.h"
#include "aboutdialog.h"

namespace Ui
{
    class MainWindow;
}

const QString modelSuffix = ".mdl";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void sceneScaleChanged(const QString &scale);
    void buttonGroupClicked(int);
    void itemInserted(DiagramItem *item);
    void deleteItem();
private:
    Ui::MainWindow *ui;
    AboutDialog *aboutDialog;
    TreeModel *treemodel;
    NetModel netmodel;
    Dialog *dialog;
    QString filename;
    DiagramScene *scene;

    QButtonGroup *buttonGroup;

    void setFileName(const QString &fn)
    {
        filename = fn;
        setWindowTitle(QString::fromUtf8("Сетевая модель ")+filename);
    }
    void doSave();
    void createToolbar();
private slots:
    void about();
    void addEvent();
    void insertEvent();
    void deleteEvent();
    void addOperation();
    void insertOperation();
    void deleteOperation();
    void calc();
    void currentChanged(const QModelIndex &, const QModelIndex &);

    void newModel();
    void open();
    void save();
    void saveAs();
    void printModel();
    void printTables();
    void exit();

    void setSelected(Event *);
    void setSelected(Operation *);
signals:
    void selected(Event *);
    void selected(Operation *);
};

#endif // MAINWINDOW_H

















