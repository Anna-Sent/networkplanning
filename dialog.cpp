#include "dialog.h"
#include "ui_dialog.h"
#include <QTextFrame>
#include <QTextTableCell>

Dialog::Dialog(NetModel &netmodel, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    _setModel(netmodel);
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
    disconnect(this, SLOT(display()));
    disconnect(this, SLOT(beforeClear()));
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
        if (netmodel->isCorrect())
            s += "Сетевая модель корректна\n";
        else
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
    cursor.beginEditBlock();
    cursor.insertText(getLabelText());
    cursor.setPosition(topFrame->lastPosition());
    QList<QVariant> header;
    QList< QList<QVariant> > data;
    fillFullPathesData(header, data);
    cursor.insertText(QString::fromUtf8("Расчет полных путей"));
    displayTable(cursor, header, data);
    cursor.setPosition(topFrame->lastPosition());
    fillEventsData(header, data);
    cursor.insertText(QString::fromUtf8("Расчет событий"));
    displayTable(cursor, header, data);
    cursor.setPosition(topFrame->lastPosition());
    fillOperationsData(header, data);
    cursor.insertText(QString::fromUtf8("Расчет работ"));
    displayTable(cursor, header, data);
    cursor.setPosition(topFrame->lastPosition());
    cursor.endEditBlock();
}

void Dialog::fillFullPathesData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "L" << "t(L)" << "R(L)";
    data.clear();
    if (netmodel && netmodel->isCorrect())
    {
        QList<Path> *pathes = netmodel->getFullPathes();
        foreach (Path p, *pathes)
        {
            QList<QVariant> row;
            row << p.code() << p.weight() << netmodel->getReserveTime(p);
            data << row;
        }
        delete pathes;
    }
}

void Dialog::fillEventsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i" << QString::fromUtf8("t р.(i)") << QString::fromUtf8("t п.(i)") << "R(i)";
    data.clear();
    if (netmodel && netmodel->isCorrect())
    {
        foreach (Event *e, *netmodel->getEvents())
        {
            QList<QVariant> row;
            row << e->getN() << netmodel->getEarlyEndTime(e) << netmodel->getLaterEndTime(e) << netmodel->getReserveTime(e);
            data << row;
        }
    }
}

void Dialog::fillOperationsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i-j" << "t(i-j)" << QString::fromUtf8("t р.н.(i-j)")
            << QString::fromUtf8("t п.н.(i-j)") << QString::fromUtf8("t р.о.(i-j)")
            << QString::fromUtf8("t п.о.(i-j)") << QString::fromUtf8("t п.(i-j)")
            << QString::fromUtf8("t с.(i-j)");
    data.clear();
    if (netmodel && netmodel->isCorrect())
    {
        foreach (Operation *o, *netmodel->getOperations())
        {
            QList<QVariant> row;
            row << o->getCode() << o->getWaitTime() << netmodel->getEarlyStartTime(o) << netmodel->getLaterStartTime(o)
                    << netmodel->getEarlyEndTime(o) << netmodel->getLaterEndTime(o) << netmodel->getFullReserveTime(o)
                    << netmodel->getFreeReserveTime(o);
            data << row;
        }
    }
}

void Dialog::displayTable(QTextCursor &cursor, QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    int colcount = header.count();
    if (colcount<1)
        return ;
    foreach (QList<QVariant> row, data)
        if (row.count()!=colcount)
            return ;
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignHCenter);
    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(4);

    QVector<QTextLength> constraints;
    for (int i=0; i<colcount; ++i)
        constraints.append(QTextLength(QTextLength::PercentageLength, 14));
    tableFormat.setColumnWidthConstraints(constraints);

    QTextTable *table = cursor.insertTable(1, colcount, tableFormat);

    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(12);//fontSize);

    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat highlightedFormat = boldFormat;
    highlightedFormat.setBackground(Qt::yellow);

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
