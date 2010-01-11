#ifndef CALCMODEL_H
#define CALCMODEL_H

#include <QAbstractItemModel>
#include "netmodel.h"

class CalcModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    void setup(NetModel &);
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void setModel(NetModel&);
private:
    void clearModelData();
protected:
    NetModel *netmodel;
    QList< QList<QVariant> > modelData;
    QList<QString> header;
    virtual void setupModelData() = 0;
    virtual void setupHeader() = 0;
    virtual int colCount() const {return 0;}
private slots:
    void update();
    void beforeUpdate();
};

#endif // CALCMODEL_H








