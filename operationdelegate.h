#ifndef OPERATIONDELEGATE_H
#define OPERATIONDELEGATE_H

#include <QItemDelegate>

class OperationDelegate : public QItemDelegate
{
    Q_OBJECT
private:
    int codeColumn;
public:
    OperationDelegate(int codeColumn, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
private slots:
    void commitAndCloseEditor(int index);
};

#endif // OPERATIONDELEGATE_H
