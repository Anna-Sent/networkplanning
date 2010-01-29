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
    aboutDialog = new AboutDialog(this);
    // setup interface signals and slots
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
    connect(ui->actionPrintModel, SIGNAL(triggered()), this, SLOT(printModel()));
    connect(ui->actionPrintTables, SIGNAL(triggered()), this, SLOT(printTables()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    // register types
    qRegisterMetaType<Event*>("Event*");
    qRegisterMetaType<Operation*>("Operation*");
    // setup test netmodel
    netmodel.addEvent();
    Event *e1 = netmodel.last();
    netmodel.addEvent();
    Event *e2 = netmodel.last();
    netmodel.addEvent();
    Event *e3 = netmodel.last();
    Operation *o1 = new Operation(1);
    Operation *o2 = new Operation(2);
    Operation *o3 = new Operation(3);
    netmodel.connect(e1,o2,e2);
    netmodel.connect(e1,o3,e3);
    netmodel.connect(e2,o1,e3);
    netmodel.addOperation(o1);
    netmodel.addOperation(o2);
    netmodel.addOperation(o3);
    // setup position of netmodel
    Position *pos = new PlanarPosition;
    pos->position(&netmodel);
    delete pos;
    // setup treemodel
    treemodel = new TreeModel(netmodel);
    ui->treeView->setModel(treemodel);
    ui->treeView->setItemDelegate(new OperationDelegate(2));
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(currentChanged(QModelIndex, QModelIndex)));
    // setup grapic scene
    scene=new DiagramScene(0);
    ui->graphView->setScene(scene);
    scene->setModel(&netmodel);
    connect(scene, SIGNAL(itemInserted(DiagramItem*)), this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(selected(Event*)), this, SLOT(setSelected(Event*)));
    connect(scene, SIGNAL(selected(Operation*)), this, SLOT(setSelected(Operation*)));
    connect(this, SIGNAL(selected(Event*)), scene, SLOT(setSelected(Event*)));
    connect(this, SIGNAL(selected(Operation*)), scene, SLOT(setSelected(Operation*)));
    // create tool bar after scene
    createToolbar();
    // setup dialog
    dialog = new Dialog(netmodel, this);
    // setup file name and caption
    setFileName("");
}

void MainWindow::about()
{
    aboutDialog->show();
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

void MainWindow::itemInserted(DiagramItem */*item*/)
{
    buttonGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(buttonGroup->checkedId()));
}

void MainWindow::buttonGroupClicked(int id)
{
    scene->setMode(DiagramScene::Mode(id));//buttonGroup->checkedId()));
}

void MainWindow::createToolbar()
{
    QToolBar *toolBar = addToolBar(QString::fromUtf8("Редактирование сетевой модели"));

    QToolButton *pointerButton = new QToolButton(toolBar);
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    pointerButton->setStatusTip(QString::fromUtf8("Режим указателя мыши"));
    pointerButton->setToolTip(QString::fromUtf8("Режим указателя мыши"));

    QToolButton *linePointerButton = new QToolButton(toolBar);
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));
    linePointerButton->setStatusTip(QString::fromUtf8("Режим создания коннектора (стрелки)"));
    linePointerButton->setToolTip(QString::fromUtf8("Режим создания коннектора (стрелки)"));

    QToolButton *itemButton = new QToolButton(toolBar);
    DiagramItem item(DiagramItem::Circle,0,0,0,0);
    QIcon icon(item.image());
    itemButton->setIcon(icon);
    itemButton->setIconSize(QSize(50,50));
    itemButton->setCheckable(true);
    itemButton->setStatusTip(QString::fromUtf8("Создать событие"));
    itemButton->setToolTip(QString::fromUtf8("Создать событие"));

    QComboBox *sceneScaleCombo = new QComboBox(toolBar);
    QStringList scales;
    scales << "50%" << "75%" << "100%" << "125%" << "150%";
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    sceneScaleCombo->setStatusTip(QString::fromUtf8("Масштаб рисунка"));
    sceneScaleCombo->setToolTip(QString::fromUtf8("Масштаб рисунка"));
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    QAction *deleteAction = new QAction (QIcon(":/images/delete.png"),
            QString::fromUtf8("Удалить выбранный элемент"), toolBar);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(QString::fromUtf8("Удалить выбранный элемент"));
    connect(deleteAction,SIGNAL(triggered()),this,SLOT(deleteItem()));
    connect(scene,SIGNAL(actionsEnabled(bool)),deleteAction,SLOT(setEnabled(bool)));

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    buttonGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    buttonGroup->addButton(itemButton, int(DiagramScene::InsertItem));
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));

    toolBar->addWidget(pointerButton);
    toolBar->addWidget(linePointerButton);
    toolBar->addWidget(itemButton);
    toolBar->addWidget(sceneScaleCombo);
    toolBar->addAction(deleteAction);
}

void MainWindow::newModel()
{
    setFileName("");
    netmodel.clear();
    treemodel->setModel(netmodel);
    dialog->setModel(netmodel);
    scene->setModel(&netmodel);
}

void MainWindow::open()
{
    QString fn = QFileDialog::getOpenFileName(this,
                                              QString::fromUtf8("Открыть модель"),
                                              "",
                                              QString::fromUtf8("Сетевые модели (*.mdl)"));
    if (fn!=NULL)
    {
        if (fn.lastIndexOf(modelSuffix)!=fn.length()-modelSuffix.length())
            fn += modelSuffix;
        setFileName(fn);
        netmodel.clear(); // clear netmodel and all its views connected to it
        QFile file(filename);
        if (file.exists())
        {
            if (!file.open(QIODevice::ReadOnly))
            {
                QMessageBox::critical(this,
                                      QString::fromUtf8("Ошибка чтения"),
                                      QString::fromUtf8("Не удалось открыть файл ")
                                      +filename+
                                      QString::fromUtf8(" для чтения"));
                return ;
            }
            QDataStream in(&file);
            netmodel.readFrom(in);
            if (in.status()!=QDataStream::Ok)
            {
                netmodel.clear();
                QMessageBox::critical(this,
                                      QString::fromUtf8("Ошибка чтения"),
                                      QString::fromUtf8("При чтении модели произошла ошибка"));
            }
            else
            {
                treemodel->setModel(netmodel);
                // scene first
                scene->setModel(&netmodel);
                // then dialog. order is important
                dialog->setModel(netmodel);
                //netmodel.update();
            }
        }
        else
        {
            QMessageBox::critical(this,
                                  QString::fromUtf8("Ошибка чтения"),
                                  QString::fromUtf8("Файл ")
                                  +filename+
                                  QString::fromUtf8(" не существует"));
        }
    }
}

void MainWindow::doSave()
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this,
                              QString::fromUtf8("Ошибка записи"),
                              QString::fromUtf8("Не удалось открыть файл ")
                              +filename+
                              QString::fromUtf8(" для записи"));
        return ;
    }
    QDataStream out(&file);
    netmodel.writeTo(out);
    if (out.status()!=QDataStream::Ok)
        QMessageBox::critical(this,
                              QString::fromUtf8("Ошибка записи"),
                              QString::fromUtf8("При записи модели произошла ошибка"));
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
        {
            if (fn.lastIndexOf(modelSuffix)!=fn.length()-modelSuffix.length())
                fn += modelSuffix;
            setFileName(fn);
        }
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
        if (fn.lastIndexOf(modelSuffix)!=fn.length()-modelSuffix.length())
            fn += modelSuffix;
        setFileName(fn);
        doSave();
    }
}

void MainWindow::printModel()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    printDialog->setWindowTitle(QString::fromUtf8("Печать сетевой модели"));
    if (printDialog->exec() != QDialog::Accepted)
        return ;
    QPainter pn(&printer);
    QRectF sceneRect = scene->itemsBoundingRect();
    QRectF printRect = printer.pageRect();
    QRectF toRect(0, 0,
                  qMin(printRect.width(), sceneRect.width()),
                  qMin(printRect.height(), sceneRect.height()));
    scene->setRenderSelection(false);
    scene->render(&pn, toRect, sceneRect);
    scene->setRenderSelection(true);
#endif
}

void MainWindow::printTables()
{
    dialog->printTables();
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
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->insertEvent(selected);
}

void MainWindow::addEvent()
{
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->addEvent(selected);
}

void MainWindow::deleteEvent()
{
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->removeEvent(selected);
}

void MainWindow::insertOperation()
{
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->insertOperation(selected);
}

void MainWindow::addOperation()
{
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->addOperation(selected);
}

void MainWindow::deleteOperation()
{
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    treemodel->removeOperation(selected);
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
            emit selected(item->getEvent());
        }
        else if (item->getOperation())
        {
            ui->btnAddEvent->setEnabled(true);
            ui->btnInsertEvent->setEnabled(false);
            ui->btnDeleteEvent->setEnabled(false);
            ui->btnAddOperation->setEnabled(true);
            ui->btnInsertOperation->setEnabled(true);
            ui->btnDeleteOperation->setEnabled(true);
            emit selected(item->getOperation());
        }
        else
        {
            ui->btnAddEvent->setEnabled(true);
            ui->btnInsertEvent->setEnabled(true);
            ui->btnDeleteEvent->setEnabled(false);
            ui->btnAddOperation->setEnabled(false);
            ui->btnInsertOperation->setEnabled(false);
            ui->btnDeleteOperation->setEnabled(false);
        }
    }
}

void MainWindow::setSelected(Event *e)
{
    ui->treeView->selectionModel()->clearSelection();
    QItemSelection selection(treemodel->getTopLeftModelIndex(e), treemodel->getBottomRightModelIndex(e));
    ui->treeView->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void MainWindow::setSelected(Operation *o)
{
    ui->treeView->selectionModel()->clearSelection();
    QItemSelection selection(treemodel->getTopLeftModelIndex(o), treemodel->getBottomRightModelIndex(o));
    ui->treeView->selectionModel()->select(selection, QItemSelectionModel::Select);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete aboutDialog;
    delete treemodel;
    delete dialog;
    delete scene;
}



















