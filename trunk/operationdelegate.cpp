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
    QItemDelegate::paint(painter, option, index);
}

QWidget *OperationDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    if (index.column() == codeColumn) {
        QComboBox *combobox = new QComboBox(parent);
        connect(combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(commitAndCloseEditor(int)));
        return combobox;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void OperationDelegate::commitAndCloseEditor(int index)
{
    Q_UNUSED(index);
    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void OperationDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const
{
    if (index.column() == codeColumn) {
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
        QComboBox *cbox = qobject_cast<QComboBox *>(editor);
        QVariant data = cbox->itemData(cbox->currentIndex());
        model->setData(index, data, Qt::EditRole);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}
