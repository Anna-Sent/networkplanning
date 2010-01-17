#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "treemodel.h"
#include "netmodel.h"
#include "dialog.h"
#include "diagramscene.h"

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
public slots:
    void sceneScaleChanged(const QString &scale);
    void pointerGroupClicked(int);
    void buttonGroupClicked(int id);
    void itemInserted(DiagramItem *item);
    void deleteItem();

private:
    Ui::MainWindow *ui;
    TreeModel *treemodel;
    NetModel netmodel;
    Dialog *dialog;
    QString filename;
    DiagramScene *scene;
    QToolBar * actions;
    QToolBar * pointerToolbar;
    QAction * deleteAction;
    QButtonGroup * pointerTypeGroup;
    QButtonGroup * buttonGroup;
    QComboBox * sceneScaleCombo;
    void setFileName(const QString &fn)
    {
        filename = fn;
        setWindowTitle(QString::fromUtf8("Сетевая модель ")+filename);
    }
    void doSave();
    void createToolbar();
    QWidget * createBtnWidget(const QString &text, DiagramItem::DiagramType type);
private slots:
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
    void print();
    void exit();
};

#endif // MAINWINDOW_H

















