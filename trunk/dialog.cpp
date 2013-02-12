#include "dialog.h"
#include <QTextFrame>
#include <QTextTableCell>
#include <QPainter>
#include <QImage>

Dialog::Dialog(NetModel &netmodel, QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog),
    eventsList(NULL), operationsList(NULL), pathes(NULL)
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
    clearCache();
    disconnect(netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
    disconnect(netmodel, SIGNAL(updated()), this, SLOT(clearCache()));
    disconnect(netmodel, SIGNAL(updated()), this, SLOT(display()));
    netmodel = NULL;
}

void Dialog::_setModel(NetModel &netmodel)
{
    this->netmodel = &netmodel;
    connect(&netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
    connect(&netmodel, SIGNAL(updated()), this, SLOT(clearCache()));
    connect(&netmodel, SIGNAL(updated()), this, SLOT(display()));
}

void Dialog::display()
{
    ui->textBrowser->clear();
    QTextCursor cursor = ui->textBrowser->textCursor();
    QTextFrame *topFrame = cursor.currentFrame();
    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(12);
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    format.setFont(font);
    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::black);
    QTextCharFormat highlightedFormat = format;
    highlightedFormat.setForeground(Qt::red);
    QString error;
    cursor.beginEditBlock();

/*    QRectF sceneRect = scene->itemsBoundingRect();
    QSizeF size(sceneRect.size());
    QSize imgSize = size.toSize();
    QImage img(imgSize, QImage::Format_ARGB32);
    img.fill(0);
    if (!img.isNull())
    {
        QPainter p(&img);
        scene->render(&p,img.rect(),sceneRect);
        ui->textBrowser->setAlignment(Qt::AlignCenter);
        cursor.insertImage(img, QString::fromUtf8("Сетевая модель"));
    }
    cursor.insertBlock();*/

    if (!netmodel)
    {
        ui->textBrowser->setAlignment(Qt::AlignLeft);
        cursor.insertText(QString::fromUtf8("Сетевая модель не задана\n"), highlightedFormat);
    }
    else if (!netmodel->isCorrect(error))
    {
        ui->textBrowser->setAlignment(Qt::AlignLeft);
        cursor.insertText(QString::fromUtf8(error.toAscii()), highlightedFormat);
    }
    else
    {
        QList<QVariant> header;
        QList< QList<QVariant> > data;

        fillFullPathesData(header, data);
        ui->textBrowser->setAlignment(Qt::AlignCenter);
        cursor.insertText(QString::fromUtf8("Расчет полных путей"), format);
        displayTable(cursor, header, data);

        cursor.setPosition(topFrame->lastPosition());

        fillEventsData(header, data);
        ui->textBrowser->setAlignment(Qt::AlignCenter);
        cursor.insertText(QString::fromUtf8("Расчет событий"), format);
        displayTable(cursor, header, data);

        cursor.setPosition(topFrame->lastPosition());

        fillOperationsData(header, data);
        ui->textBrowser->setAlignment(Qt::AlignCenter);
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
    if (!pathes)
        pathes = netmodel->getFullPathes();
    //netmodel->sort(*pathes);
    foreach (Path p, *pathes)
    {
        QList<QVariant> row;
        row << p.code() << p.weight() << netmodel->getReserveTime(p);
        data << row;
    }
    //delete pathes;
}

/*Before call this function check the netmodel is not null and is correct.*/
void Dialog::fillEventsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i" << QString::fromUtf8("t р.(i)") << QString::fromUtf8("t п.(i)") << "R(i)";
    data.clear();
    if (!eventsList)
        eventsList = netmodel->getSortedEvents();
    foreach (Event *e, *eventsList)
    {
        QList<QVariant> row;
        row << e->getN() << netmodel->getEarlyEndTime(e) << netmodel->getLaterEndTime(e) << netmodel->getReserveTime(e);
        data << row;
    }
    //delete eventsList;
}

/*Before call this function check the netmodel is not null and is correct.*/
void Dialog::fillOperationsData(QList<QVariant> &header, QList< QList<QVariant> > &data)
{
    header.clear();
    header << "i-j" << "t(i-j)" << QString::fromUtf8("t р.н.(i-j)")
            << QString::fromUtf8("t п.н.(i-j)") << QString::fromUtf8("t р.о.(i-j)")
            << QString::fromUtf8("t п.о.(i-j)") << QString::fromUtf8("R п.(i-j)")
            << QString::fromUtf8("R с.(i-j)") << QString::fromUtf8("K н.(i-j)");
    data.clear();
    if (!operationsList)
        operationsList = netmodel->getSortedOperatioins();
    foreach (Operation *o, *operationsList)
    {
        QList<QVariant> row;
        row << o->getCode() << o->getWaitTime() << netmodel->getEarlyStartTime(o) << netmodel->getLaterStartTime(o)
                << netmodel->getEarlyEndTime(o) << netmodel->getLaterEndTime(o) << netmodel->getFullReserveTime(o)
                << netmodel->getFreeReserveTime(o) << netmodel->getIntensityFactor(o);
        data << row;
    }
    //delete operationsList;
}

void Dialog::displayTable(QTextCursor &cursor, const QList<QVariant> &header, const QList< QList<QVariant> > &data)
{
    int colcount = header.count();
    if (colcount<1)
        return ;
    foreach (QList<QVariant> row, data)
        if (row.count()>colcount)
            return ;
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellPadding(4);
    tableFormat.setCellSpacing(2);
    QTextTable *table = cursor.insertTable(1, colcount, tableFormat);
    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(12);
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    format.setFont(font);
    format.setForeground(Qt::black);
    format.setFontWeight(QFont::Normal);
    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold);
    for (int i = 0; i < colcount; ++i)
    {
        QTextTableCell cell = table->cellAt(0, i);
        QTextCursor cellCursor = cell.firstCursorPosition();
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment(Qt::AlignCenter);
        cellCursor.setBlockFormat(blockFormat);
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
