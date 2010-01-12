#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(NetModel &netmodel, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), netmodel(&netmodel)
{
    ui->setupUi(this);
    fpmodel = new FullPathesModel(netmodel);
    cemodel = new CalcEventModel(netmodel);
    comodel = new CalcOperationModel(netmodel);
    ui->label1->setText(QString::fromUtf8("Расчет полных путей"));
    ui->tableView1->setModel(fpmodel);
    ui->label2->setText(QString::fromUtf8("Расчет событий"));
    ui->tableView2->setModel(cemodel);
    ui->label3->setText(QString::fromUtf8("Расчет работ"));
    ui->tableView3->setModel(comodel);
    setupLabelText();
    connect(&netmodel, SIGNAL(updated()), this, SLOT(setupLabelText()));
    connect(&netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
}

void Dialog::setModel(NetModel &netmodel)
{
    fpmodel->setModel(netmodel);
    cemodel->setModel(netmodel);
    comodel->setModel(netmodel);
    this->netmodel = &netmodel;
    setupLabelText();
    connect(&netmodel, SIGNAL(updated()), this, SLOT(setupLabelText()));
    connect(&netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
}

void Dialog::beforeClear()
{
    disconnect(this, SLOT(setupLabelText()));
    disconnect(this, SLOT(beforeClear()));
    netmodel = NULL;
    ui->label->setText(QString::fromUtf8("Сетевая модель не задана"));
}

void Dialog::setupLabelText()
{
    QString s = netmodel->print();
    if (netmodel->hasLoops())
        s += "Имеются циклы\n";
    if (netmodel->hasMultiEdges())
        s += "Имеются работы с одинаковыми кодами\n";
    if (!netmodel->hasOneBeginEvent())
        s += "Исходное событие не определено\n";
    if (!netmodel->hasOneEndEvent())
        s += "Завершающее событие не определено\n";
    if (netmodel->hasUnconnectedEvents())
        s += "Некоторые события не соединены с работами\n";
    if (netmodel->hasUnconnectedOperations())
        s += "Некоторые работы не соединены с событиями\n";
    if (netmodel->isCorrect())
    {
        s += "Сетевая модель корректна\n";
        QList<Path> *pathes = netmodel->getFullPathes();
        s += "Полные пути:\n";
        int i=0;
        foreach(Path path, *pathes)
        {
            s += QString::number(++i) + ": ";
            s += path.code() + "\n";
        }
        delete pathes;
        QList<Path> *criticPathes = netmodel->getCriticalPathes();
        s += "Критические пути\n";
        i=0;
        foreach(Path path, *criticPathes)
        {
            s += QString::number(++i) + ": ";
            s += path.code() + "\n";
        }
        delete criticPathes;
    }
    else
        s += "Сетевая модель некорректна";
    //QString caption = "Сетевая модель";
    //QMessageBox::information(this, QString::fromUtf8(caption.toAscii()), QString::fromUtf8(s.toAscii()));
    ui->label->setText(QString::fromUtf8(s.toAscii()));
}

Dialog::~Dialog()
{
    delete ui;
    delete fpmodel;
    delete cemodel;
    delete comodel;
}
