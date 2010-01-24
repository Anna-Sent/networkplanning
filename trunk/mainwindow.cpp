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
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    // register types
    qRegisterMetaType<Event*>("Event*");
    qRegisterMetaType<Operation*>("Operation*");
    // setup test netmodel
    netmodel.addEvent();
    Event *e1 = netmodel.last(); //new Event(0);
    netmodel.addEvent();
    Event *e2 = netmodel.last(); //new Event(1);
    netmodel.addEvent();
    Event *e3 = netmodel.last();// new Event(2);
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
    // create tool bar after scene
    createToolbar();
    // setup dialog
    dialog = new Dialog(netmodel, scene, this);
    // call netmodel update to update all views (treemodel, scene, dialog)
    //netmodel.update();
    // setup file name and caption
    setFileName("");
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       QString::fromUtf8("О программе..."),
                       QString::fromUtf8("(c) 2010 Сентякова А. В.\nanna.sent@gmail.com"));
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
    deleteAction = new QAction(QIcon(":/images/delete.png"), QString::fromUtf8("Удалить"), scene);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(QString::fromUtf8("Удалить выбранный элемент"));
    connect(deleteAction, SIGNAL(triggered()),
            this, SLOT(deleteItem()));
    connect(scene,SIGNAL(actionsEnabled(bool)),deleteAction,SLOT(setEnabled(bool)));
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
    scales << "50%" << "75%" << "100%" << "125%" << "150%";
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
    scene->setModel(&netmodel);
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
    model->insertEvent(selected);
}

void MainWindow::addEvent()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    model->addEvent(selected);
}

void MainWindow::deleteEvent()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    model->removeEvent(selected);
}

void MainWindow::insertOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    model->insertOperation(selected);
}

void MainWindow::addOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    model->addOperation(selected);
}

void MainWindow::deleteOperation()
{
    TreeModel *model = static_cast<TreeModel*>(ui->treeView->model());
    QModelIndex selected = ui->treeView->selectionModel()->currentIndex();
    model->removeOperation(selected);
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
        }
        else if (item->getOperation())
        {
            ui->btnAddEvent->setEnabled(true);
            ui->btnInsertEvent->setEnabled(false);
            ui->btnDeleteEvent->setEnabled(false);
            ui->btnAddOperation->setEnabled(true);
            ui->btnInsertOperation->setEnabled(true);
            ui->btnDeleteOperation->setEnabled(true);
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

MainWindow::~MainWindow()
{
    delete ui;
    delete treemodel;
    delete dialog;
}
