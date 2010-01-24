#include "dialog.h"
#include <QTextFrame>
#include <QTextTableCell>
#include <QPainter>
#include <QImage>

Dialog::Dialog(NetModel &netmodel,DiagramScene *sc, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), scene(sc)
{
    ui->setupUi(this);
    _setModel(netmodel);
    //connect(sc,SIGNAL(changed()),this,SLOT(display()));
    display();
}

void Dialog::setModel(NetModel &netmodel)
{
    if (this->netmodel)
        _clearModel();
    _setModel(netmodel);
    display();
}

void Dialog::beforeClear()
{
    _clearModel();
    display();
}

void Dialog::_clearModel()
{
    connect(netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
    connect(netmodel, SIGNAL(updated()), this, SLOT(display()));
    netmodel = NULL;
}

void Dialog::_setModel(NetModel &netmodel)
{
    this->netmodel = &netmodel;
    connect(&netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
    connect(&netmodel, SIGNAL(updated()), this, SLOT(display()));
}

QString Dialog::getLabelText()
{
    QString s;
    if (netmodel)
    {
        s = netmodel->print();
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
        if (!netmodel->isCorrect())
            s += "Сетевая модель некорректна\n";
    }
    else
        s = "Сетевая модель не задана\n";
    return QString::fromUtf8(s.toAscii());
}

void Dialog::display()
{
    ui->textBrowser->clear();
    QTextCursor cursor = ui->textBrowser->textCursor();
    QTextFrame *topFrame = cursor.currentFrame();
    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(12);
    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::black);
    QTextCharFormat highlightedFormat = format;
    highlightedFormat.setForeground(Qt::red);
    QString error;
    cursor.beginEditBlock();

    QSize size(scene->itemsBoundingRect().size().toSize());
    QImage img(size, QImage::Format_ARGB32);
    img.fill(0);
    if (!img.isNull())
    {
        QPainter p(&img);
        scene->render(&p);
        cursor.insertImage(img, QString::fromUtf8("Сетевая модель"));
    }
    cursor.insertBlock();

    if (!netmodel)
    {
        cursor.insertText(QString::fromUtf8("Сетевая модель не задана\n"), highlightedFormat);
    }
    else if (!netmodel->isCorrect(error))
    {
        cursor.insertText(QString::fromUtf8(error.toAscii()), highlightedFormat);
    }
    else
    {
        QList<QVariant> header;
        QList< QList<QVariant> > data;

        fillFullPathesData(header, data);
        cursor.insertText(QString::fromUtf8("Расчет полных путей"), format);
        displayTable(cursor, header, data);

        cursor.setPosition(topFrame->lastPosition());

        fillEventsData(header, data);
        cursor.insertText(QString::fromUtf8("Расчет событий"), format);
        displayTable(cursor, header, data);

        cursor.setPosition(topFrame->lastPosition());

        fillOperationsData(header, data);
        cursor.insertText(QString::fromUtf8("Расчет работ"), format);
        displayTable(cursor, header, data);

        cursor.setPosition(topFrame->lastPosition());
    }
    cursor.endEditBlock();
}

/*Before call this function check the netmodel is not null and is correct.*/
void Dialog::fillFullPathesData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "L" << "t(L)" << "R(L)";
    data.clear();
    QList<Path> *pathes = netmodel->getFullPathes();
    //netmodel->sort(*pathes);
    foreach (Path p, *pathes)
    {
        QList<QVariant> row;
        row << p.code() << p.weight() << netmodel->getReserveTime(p);
        data << row;
    }
    delete pathes;
}

/*Before call this function check the netmodel is not null and is correct.*/
void Dialog::fillEventsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i" << QString::fromUtf8("t р.(i)") << QString::fromUtf8("t п.(i)") << "R(i)";
    data.clear();
    QList<Event*> *list = netmodel->getSortedEvents();
    foreach (Event *e, *list)
    {
        QList<QVariant> row;
        row << e->getN() << netmodel->getEarlyEndTime(e) << netmodel->getLaterEndTime(e) << netmodel->getReserveTime(e);
        data << row;
    }
    delete list;
}

/*Before call this function check the netmodel is not null and is correct.*/
void Dialog::fillOperationsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i-j" << "t(i-j)" << QString::fromUtf8("t р.н.(i-j)")
            << QString::fromUtf8("t п.н.(i-j)") << QString::fromUtf8("t р.о.(i-j)")
            << QString::fromUtf8("t п.о.(i-j)") << QString::fromUtf8("t п.(i-j)")
            << QString::fromUtf8("t с.(i-j)");
    data.clear();
    QList<Operation*> *list = netmodel->getSortedOperatioins();
    foreach (Operation *o, *list)
    {
        QList<QVariant> row;
        row << o->getCode() << o->getWaitTime() << netmodel->getEarlyStartTime(o) << netmodel->getLaterStartTime(o)
                << netmodel->getEarlyEndTime(o) << netmodel->getLaterEndTime(o) << netmodel->getFullReserveTime(o)
                << netmodel->getFreeReserveTime(o);
        data << row;
    }
    delete list;
}

void Dialog::displayTable(QTextCursor &cursor, QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    int colcount = header.count();
    if (colcount<1)
        return ;
    foreach (QList<QVariant> row, data)
        if (row.count()>colcount)
            return ;
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignHCenter);
    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellPadding(4);
    tableFormat.setCellSpacing(2);
    QTextTable *table = cursor.insertTable(1, colcount, tableFormat);
    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(12);
    format.setForeground(Qt::black);
    format.setFontWeight(QFont::Normal);
    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold);
    for (int i = 0; i < colcount; ++i)
    {
        QTextTableCell cell = table->cellAt(0, i);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(header[i].toString(), boldFormat);
    }
    table->insertRows(table->rows(), data.count());
    int i = 1;
    foreach (QList<QVariant> row, data)
    {
        for (int j = 0; j < row.count(); ++j)
        {
            QTextTableCell cell = table->cellAt(i, j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(data[i-1][j].toString(), format);
        }
        ++i;
    }
}

Dialog::~Dialog()
{
    delete ui;
}
