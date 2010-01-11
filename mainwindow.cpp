#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operationdelegate.h"
#include "treeitem.h"
#include "positioning.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnAddEvent, SIGNAL(clicked()), this, SLOT(addEvent()));
    connect(ui->btnInsertEvent, SIGNAL(clicked()), this, SLOT(insertEvent()));
    connect(ui->btnDeleteEvent, SIGNAL(clicked()), this, SLOT(deleteEvent()));
    connect(ui->btnAddOperation,  SIGNAL(clicked()), this, SLOT(addOperation()));
    connect(ui->btnInsertOperation, SIGNAL(clicked()), this, SLOT(insertOperation()));
    connect(ui->btnDeleteOperation, SIGNAL(clicked()), this, SLOT(deleteOperation()));
    connect(ui->btnCalc, SIGNAL(clicked()), this, SLOT(calc()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newModel()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

    qRegisterMetaType<Event*>("Event*");
    qRegisterMetaType<Operation*>("Operation*");

    Event *e1 = new Event(0);
    Event *e2 = new Event(1);
    Event *e3 = new Event(2);
    //Event *e4 = new Event(3);
    //Event *e5 = new Event(4);
    //Event *e6 = new Event(5);
    Operation *o1 = new Operation(1);
    Operation *o2 = new Operation(2);
    Operation *o3 = new Operation(3);
    //Operation *o4 = new Operation(4);
    //Operation *o5 = new Operation(5);
    //Operation *o6 = new Operation(6);
    netmodel.connect(e1,o2,e2);
    netmodel.connect(e1,o3,e3);
    netmodel.connect(e2,o1,e3);
    //netmodel.disconnect(e2,o1);
    //netmodel.disconnect(e5,o2);
    treemodel = new TreeModel(netmodel);
    ui->treeView->setModel(treemodel);
    ui->treeView->setItemDelegate(new OperationDelegate(2));
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);
    Position *pos = new PlanarPosition;
    pos->position(&netmodel);
    delete pos;
    ui->graphView->setModel(&netmodel);

    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(currentChanged(QModelIndex, QModelIndex)));
    dialog = new Dialog(netmodel, this);
    setFileName("");
}

void MainWindow::newModel()
{
    setFileName("");
    netmodel.clear();
    // clear netmodel and all views
    // create new model?
}

void MainWindow::open()
{
    newModel();
    setFileName(QFileDialog::getOpenFileName(this,
                                            QString::fromUtf8("Открыть модель"),
                                            "",
                                            QString::fromUtf8("Сетевые модели (*.mdl)")));
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QDataStream in(&file);
    netmodel.readFrom(in);
    if (in.status()!=QDataStream::Ok)
        QMessageBox::critical(this, "Ошибка чтения", "При чтении модели произошла ошибка");
    else
    {
        treemodel->setModel(netmodel);
        dialog->setModel(netmodel);
        ui->graphView->setModel(&netmodel);
    }
    // clear netmodel and all views
    // load netmodel
}

void MainWindow::save()
{
    if (filename=="")
    {
        setFileName(QFileDialog::getSaveFileName(this,
                                                QString::fromUtf8("Сохранить модель"),
                                                "",
                                                QString::fromUtf8("Сетевые модели (*.mdl)")));
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QDataStream out(&file);
    netmodel.writeTo(out);
    if (out.status()!=QDataStream::Ok)
        QMessageBox::critical(this, "Ошибка записи", "При записи модели произошла ошибка");
}

void MainWindow::saveAs()
{
    setFileName("");
    save();
}

void MainWindow::print()
{
}

void MainWindow::exit()
{
    this->close();
}

void MainWindow::calc()
{
    dialog->show();
}

void MainWindow::insertEvent()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
        {
            model->insertRow(selected.row(), selected.parent());
        }
    }
    else
    {
        model->insertRow(0, selected);
        ui->treeView->selectionModel()->setCurrentIndex(
                    model->index(0,0),
                    QItemSelectionModel::Select);
        ui->treeView->selectionModel()->setCurrentIndex(
                    model->index(0,1),
                    QItemSelectionModel::Select);
    }
}

void MainWindow::addEvent()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
        {
            model->insertRow(item->parent()->childCount(), selected.parent());
        }
        else if (item->getOperation())
        {
            model->insertRow(model->getRootItem()->childCount(), selected.parent().parent());
        }
    }
    else
    {
        model->insertRow(0, selected);
        ui->treeView->selectionModel()->setCurrentIndex(
                    model->index(0,0),
                    QItemSelectionModel::Select);
        ui->treeView->selectionModel()->setCurrentIndex(
                    model->index(0,1),
                    QItemSelectionModel::Select);
    }
}

void MainWindow::deleteEvent()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
            model->removeRow(selected.row(), selected.parent());
    }
}

void MainWindow::insertOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
            model->insertRow(0, selected);
        else if (item->getOperation())
            model->insertRow(selected.row(), selected.parent());
    }
}

void MainWindow::addOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
            model->insertRow(item->childCount(), selected);
        else if (item->getOperation())
            model->insertRow(item->parent()->childCount(), selected.parent());
    }
}

void MainWindow::deleteOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getOperation())
            model->removeRow(selected.row(), selected.parent());
    }
}

void MainWindow::currentChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if (current.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(current.internalPointer());
        if (item->getEvent())
        {
            ui->btnAddEvent->setEnabled(true);
            ui->btnInsertEvent->setEnabled(true);
            ui->btnDeleteEvent->setEnabled(true);
            ui->btnAddOperation->setEnabled(true);
            ui->btnInsertOperation->setEnabled(true);
            ui->btnDeleteOperation->setEnabled(false);
            //ui->btnCheck->setEnabled(true);
        }
        else if (item->getOperation())
        {
            ui->btnAddEvent->setEnabled(true);
            ui->btnInsertEvent->setEnabled(false);
            ui->btnDeleteEvent->setEnabled(false);
            ui->btnAddOperation->setEnabled(true);
            ui->btnInsertOperation->setEnabled(true);
            ui->btnDeleteOperation->setEnabled(true);
            //ui->btnCheck->setEnabled(true);
        }
        else
        {
            //ui->btnAddEvent->setEnabled(true);
            //ui->btnAddOperation->setEnabled(false);
            //ui->btnCheck->setEnabled(true);
            //ui->btnDeleteEvent->setEnabled(false);
            //ui->btnDeleteOperation->setEnabled(false);
        }
    }
    else
    {
        ui->btnAddEvent->setEnabled(true);
        ui->btnInsertEvent->setEnabled(true);
        ui->btnDeleteEvent->setEnabled(false);
        ui->btnAddOperation->setEnabled(false);
        ui->btnInsertOperation->setEnabled(false);
        ui->btnDeleteOperation->setEnabled(false);
        //ui->btnCheck->setEnabled(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete treemodel;
    delete dialog;
}
