#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operationdelegate.h"
#include "treeitem.h"
#include "positioning.h"
#include <QFileDialog>
#include "diagramscene.h"

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

    netmodel.addEvent(this);
    Event *e1 = netmodel.last(); //new Event(0);
    netmodel.addEvent(this);
    Event *e2 = netmodel.last(); //new Event(1);
    netmodel.addEvent(this);
    Event *e3 = netmodel.last();// new Event(2);
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
    //ui->treeView->setDragEnabled(true);
    //ui->treeView->setAcceptDrops(true);
    //ui->treeView->setDropIndicatorShown(true);
    //ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);
    Position *pos = new PlanarPosition;
    pos->position(&netmodel);
    delete pos;
    scene=new DiagramScene(0);
    ui->graphView->setScene(scene);
    scene->setModel(&netmodel);
    connect(scene, SIGNAL(itemInserted(DiagramItem*)), this, SLOT(itemInserted(DiagramItem*)));
    /*ui->graphView->setModel(&netmodel);

    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(currentChanged(QModelIndex, QModelIndex)));*/
    dialog = new Dialog(netmodel, this);

    setFileName("");
    createToolbar();
}

void MainWindow::deleteItem()
{
    scene->deleteItem();
}


void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = ui->graphView->matrix();
    ui->graphView->resetMatrix();
    ui->graphView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->graphView->scale(newScale, newScale);
}

QWidget* MainWindow::createBtnWidget(const QString& text, DiagramItem::DiagramType type)
{
    QToolButton *itemButton = new QToolButton;
    DiagramItem item(type ,0,0,0,0);
    QIcon icon(item.image());

    buttonGroup->addButton(itemButton, type);
    itemButton->setIcon(icon);
    itemButton->setIconSize(QSize(50,50));
    itemButton->setCheckable(true);
    return itemButton;
}

void MainWindow::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
    if (buttonGroup->button(id) != button)
        button->setChecked(false);
    }
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(DiagramScene::InsertItem);

}

void MainWindow::itemInserted(DiagramItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    QToolButton *tb = static_cast<QToolButton*>(buttonGroup->button(int(item->diagramType())));
    tb->setChecked(false);
    tb->update();
}


void MainWindow::createToolbar()
{
    actions=addToolBar("Actions");
    deleteAction = new QAction(QIcon(":/images/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()),
        this, SLOT(deleteItem()));
    actions->addAction(deleteAction);
    buttonGroup = new QButtonGroup;
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupClicked(int)));
    actions->addWidget(createBtnWidget(tr(""), DiagramItem::Circle));
    buttonGroup->setExclusive(false);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup;
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton,
                                int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);

}

void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::newModel()
{
    setFileName("");
    netmodel.clear();
    treemodel->setModel(netmodel);
    dialog->setModel(netmodel);
    //ui->graphView->setModel(&netmodel);
}

void MainWindow::open()
{
    QString fn = QFileDialog::getOpenFileName(this,
                                            QString::fromUtf8("Открыть модель"),
                                            "",
                                            QString::fromUtf8("Сетевые модели (*.mdl)"));
    if (fn!=NULL)
    {
        setFileName(fn);
        netmodel.clear(); // clear netmodel and all its views connected to it
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
            scene->setModel(&netmodel);
        }
    }
}

void MainWindow::doSave()
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QDataStream out(&file);
    netmodel.writeTo(out);
    if (out.status()!=QDataStream::Ok)
        QMessageBox::critical(this, "Ошибка записи", "При записи модели произошла ошибка");
}

void MainWindow::save()
{
    if (filename=="")
    {
        QString fn = QFileDialog::getSaveFileName(this,
                                                QString::fromUtf8("Сохранить модель"),
                                                "",
                                                QString::fromUtf8("Сетевые модели (*.mdl)"));
        if (fn!=NULL)
            setFileName(fn);
        else
            return ;
    }
    doSave();
}

void MainWindow::saveAs()
{
    QString fn = QFileDialog::getSaveFileName(this,
                                              QString::fromUtf8("Сохранить модель"),
                                              "",
                                              QString::fromUtf8("Сетевые модели (*.mdl)"));
    if (fn!=NULL)
    {
        setFileName(fn);
        doSave();
    }
}

void MainWindow::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    printDialog->setWindowTitle(QString::fromUtf8("Печать сетевой модели"));
    if (dialog->getTextBrowser()->textCursor().hasSelection())
        printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (printDialog->exec() != QDialog::Accepted)
        return;
    dialog->getTextBrowser()->print(&printer);
#endif
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
            model->insertRow(netmodel.getEvents()->count(), selected.parent());
        }
        else if (item->getOperation())
        {
            model->insertRow(netmodel.getEvents()->count(), selected.parent().parent());
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
            model->insertRow(item->getEvent()->getOutOperations().count(), selected);
        else if (item->getOperation())
            model->insertRow(
                    item->getOperation()->getBeginEvent()->getOutOperations().count(),
                    selected.parent());
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
