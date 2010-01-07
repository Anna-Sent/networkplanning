#include "operationdelegate.h"
#include "treemodel.h"
#include "treeitem.h"
#include <QComboBox>

OperationDelegate::OperationDelegate(int codeColumn,QObject *parent) : QItemDelegate(parent)
{
    this->codeColumn=codeColumn;
}

void OperationDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    // if (index.column() == codeColumn) {
    /*int secs = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = QString("%1:%2")
                       .arg(secs / 60, 2, 10, QChar('0'))
                       .arg(secs % 60, 2, 10, QChar('0'));
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);*/
    //} else{
    QItemDelegate::paint(painter, option, index);
    //}
}

QWidget *OperationDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    if (index.column() == codeColumn) {
        /*QTimeEdit *timeEdit = new QTimeEdit(parent);
        timeEdit->setDisplayFormat("mm:ss");
        connect(timeEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));*/
        QComboBox *combobox = new QComboBox(parent);
        //combobox->addItem("123");
        connect(combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditor(int)));
        return combobox;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void OperationDelegate::commitAndCloseEditor(int/* index*/)
{
    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void OperationDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const
{
    if (index.column() == codeColumn) {
        /*int secs = index.model()->data(index, Qt::DisplayRole).toInt();
        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        timeEdit->setTime(QTime(0, secs / 60, secs % 60));*/
        //int code = index.model()->data(index, Qt::EditRole).toInt();
        Operation *o = static_cast<TreeItem*>(index.internalPointer())->getOperation();
        if (o)
        {
            Event *e = o->getEndEvent();
            QVariant data;
            data.setValue(e);
            QComboBox *cbox = qobject_cast<QComboBox *>(editor);
            dynamic_cast<const TreeModel*>(index.model())->fill(cbox, index);
            cbox->setCurrentIndex(cbox->findData(data));
        }
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void OperationDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    if (index.column() == codeColumn) {
        /*QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        QTime time = timeEdit->time();
        int secs = (time.minute() * 60) + time.second();
        model->setData(index, secs);*/
        QComboBox *cbox = qobject_cast<QComboBox *>(editor);
        QVariant data = cbox->itemData(cbox->currentIndex());
        model->setData(index, data, Qt::EditRole);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}
