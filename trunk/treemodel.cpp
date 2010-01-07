/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"

//! [0]
TreeModel::TreeModel(NetModel &netmodel, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << QString::fromUtf8("Код события") << QString::fromUtf8("Событие") << QString::fromUtf8("Код работы") << QString::fromUtf8("Работа") << QString::fromUtf8("Продолжительность");
    rootItem = new TreeItem(rootData);
    this->netmodel = &netmodel;
    setupModelData(rootItem);
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &) const
{
    //if (parent.isValid())
      //  return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    //else
      //  return rootItem->columnCount();
    return 5;
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item->getEvent())
    {
        switch (index.column())
        {
            case 0: return item->getEvent()->getN();
            case 1: return item->getEvent()->getName();
            default: return QVariant();
        }
    }
    else if (item->getOperation())
    {
        switch (index.column())
        {
            case 2: return item->getOperation()->getCode();
            case 3: return item->getOperation()->getName();
            case 4: return item->getOperation()->getWaitTime();
            default: return QVariant();
        }
    }
    else
        return QVariant();
//    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;
    if ((index.column()==0||index.column()==1)
        && static_cast<TreeItem*>(index.internalPointer())->getEvent())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if ((index.column()==2||index.column()==3||index.column()==4)
        && static_cast<TreeItem*>(index.internalPointer())->getOperation())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return 0;
}
//! [4]

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (role!=Qt::EditRole)
        return false;
    QVariant v = value;
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item->getEvent())
    {
        switch (index.column())
        {
            case 0:
                    if (v.convert(QVariant::Int)
                        && netmodel->setN(this, item->getEvent(), v.toInt()))
                    {
                        emit dataChanged(index,index);
                        return true;
                    }
                    else
                        return false;
            case 1:
                    if (v.convert(QVariant::String)
                        && netmodel->setName(this, item->getEvent(), v.toString()))
                    {
                        emit dataChanged(index,index);
                        return true;
                    }
                    else
                        return false;
            default: return false;
        }
    }
    else if (item->getOperation())
    {
        switch (index.column())
        {
            case 2:
                {
                    Event *e = v.value<Event*>();
                    Operation *o = item->getOperation();
                    netmodel->setOperationEndEvent(this, &o, e);
                    emit dataChanged(index,index);
                    return true;
                }
            case 3:
                if (v.convert(QVariant::String)
                    && netmodel->setOperationName(this, item->getOperation(), v.toString()))
                {
                    emit dataChanged(index,index);
                    return true;
                }
                else
                    return false;
            case 4:
                if (v.convert(QVariant::Double)
                    && netmodel->setOperationWaitTime(this, item->getOperation(), v.toDouble()))
                {
                    emit dataChanged(index,index);
                    return true;
                }
                else
                    return false;
            default: return false;
        }
    }
    return false;
}


//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

void TreeModel::process(Event *e, TreeItem *parent, QSet<int> &set)
{
    set += e->getN();
    QList<QVariant> columnData;
    columnData << e->getN() << e->getName();
    TreeItem *item = new TreeItem(columnData, parent);
    item->setEvent(e);
    parent->appendChild(item);
    foreach (Operation *o, e->getOutOperations())
    {
        process(o, item);
    }
    foreach (Operation *o, e->getOutOperations())
    {
        Event *next = o->getEndEvent();
        if (!set.contains(next->getN()))
            process(next, parent, set);
    }
}

void TreeModel::process(Operation *o, TreeItem *parent)
{
    QList<QVariant> columnData;
    columnData << "" << "" << o->getCode() << o->getName() << o->getWaitTime();
    TreeItem *item = new TreeItem(columnData, parent);
    item->setOperation(o);
    parent->appendChild(item);
}

void TreeModel::setupModelData(TreeItem *parent)
{
    QList<TreeItem*> parents;
    parents << parent;
    QSet<int> set;
    process(netmodel->getBeginEvent(), parent, set);
}

void TreeModel::fill(QComboBox *cbox, const QModelIndex &index) const
{
    Operation *op = static_cast<TreeItem*>(index.internalPointer())->getOperation();
    if (op)
    {
        Event *begin = op->getBeginEvent();
        if (begin&&cbox)
        {
            QList<Event*> events = *(netmodel->getEvents());
            for (int i=0;i<events.count();++i)
            {
                Event *e = events[i];
                if (e==begin)
                    events.removeAt(i);
            }
            foreach (Event *e, events)
            {
                Operation o;
                o.setBeginEvent(begin);
                o.setEndEvent(e);
                QVariant t;
                t.setValue(e);
                cbox->addItem(o.getCode(), t);
            }
        }
    }
}
/*
On models that support this, inserts count rows into the model before the given row.
The items in the new row will be children of the item represented by the parent
model index.
Returns true if the rows were successfully inserted; otherwise returns false.
*/
bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
//    beginInsertRows(parent, row, row+count-1);
    for (int i=row; i<row+count; ++i)
    {
        if (parent.isValid())
        {
            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            Event *event = parentItem->getEvent();
            if (event)
            {
                Operation *o = new Operation();
                o->setBeginEvent(event);
                if (netmodel->insertOperation(this, o, i))
                {
                    QModelIndex parent1 = createIndex(parent.row(), 0, parent.internalPointer());
                    beginInsertRows(parent1, i, i);
                    event->insertOutOperation(o, i);
                    QList<QVariant> columnData;
                    columnData << "" << "" << o->getCode() << o->getName() << o->getWaitTime();
                    TreeItem *item = new TreeItem(columnData, parentItem);
                    item->setOperation(o);
                    parentItem->insertChild(item, row);
                    endInsertRows();
                    result = result || true;
                }
                else
                    delete o;
            }
        }
        else
        {
            Event *e = new Event(42);
            if (netmodel->insertEvent(this, e, i))
            {
                beginInsertRows(parent, i, i);
                QList<QVariant> columnData;
                columnData << e->getN() << e->getName();
                TreeItem *item = new TreeItem(columnData, rootItem);
                item->setEvent(e);
                rootItem->insertChild(item, row);
                endInsertRows();
                result = result || true;
            }
            else
                delete e;
        }
    }
//    endInsertRows();
    return result;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    beginRemoveRows(parent, row, row+count-1);
    for (int i=row; i<row+count; ++i)
    {
        if (parent.isValid())
        {
            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            Event *e = parentItem->getEvent();
            if (e && i<e->getOutOperations().count())
            {
                Operation *o = e->getOutOperations()[i];
                if (netmodel->removeOperation(this, o))
                {
//                    beginRemoveRows(parent, i, i);
                    parentItem->child(i)->clear();
                    parentItem->removeChild(i);
//                    endRemoveRows();
                    result = result || true;
                }
            }
        }
        else
        {
            TreeItem *parentItem = rootItem;//static_cast<TreeItem*>(parent.internalPointer());
            if (i<parentItem->childCount())
            {
                Event *e = parentItem->child(i)->getEvent();
                if (netmodel->removeEvent(this, e))
                {
//                    beginRemoveRows(parent, i, i);
                    parentItem->child(i)->clear();
                    parentItem->removeChild(i);
//                    endRemoveRows();
                    result = result || true;
                }
            }
        }
    }
    endRemoveRows();
    return result;
}


















