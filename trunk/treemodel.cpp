//#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"
#include <assert.h>

TreeModel::TreeModel(NetModel &netmodel, QObject *parent)
    : QAbstractItemModel(parent)
{
    header.clear();
    header << QString::fromUtf8("Код события") << QString::fromUtf8("Событие") << QString::fromUtf8("Код работы") << QString::fromUtf8("Работа") << QString::fromUtf8("Продолжительность");
    rootItem = new TreeItem();
    setModel(netmodel);
}

void TreeModel::setModel(NetModel &netmodel)
{
    this->netmodel = &netmodel;
    connect(this->netmodel, SIGNAL(beforeClear()), this, SLOT(beforeClear()));
    connect(this->netmodel, SIGNAL(eventIdChanged(Event *, int)),
            this, SLOT(eventIdChanged(Event *, int)));
    connect(this->netmodel, SIGNAL(eventNameChanged(Event *, const QString &)),
            this, SLOT(eventNameChanged(Event *, const QString &)));
    connect(this->netmodel, SIGNAL(operationEndEventChanged(Operation **, Event *)),
            this, SLOT(operationEndEventChanged(Operation **, Event *)));
    connect(this->netmodel, SIGNAL(operationNameChanged(Operation *, const QString &)),
            this, SLOT(operationNameChanged(Operation *, const QString &)));
    connect(this->netmodel, SIGNAL(operationWaitTimeChanged(Operation *, double)),
            this, SLOT(operationWaitTimeChanged(Operation *, double)));
    connect(this->netmodel, SIGNAL(afterEventAdd()),
            this, SLOT(afterEventAdd()));
    connect(this->netmodel, SIGNAL(beforeEventDelete(Event *)),
            this, SLOT(beforeEventDelete(Event *)));
    connect(this->netmodel, SIGNAL(afterOperationAdd(Operation *)),
            this, SLOT(afterOperationAdd(Operation *)));
    connect(this->netmodel, SIGNAL(beforeOperationDelete(Operation *)),
            this, SLOT(beforeOperationDelete(Operation *)));
    connect(this->netmodel, SIGNAL(afterEventInsert(int)),
            this, SLOT(afterEventInsert(int)));
    connect(this->netmodel, SIGNAL(afterOperationInsert(Operation *, int)),
            this, SLOT(afterOperationInsert(Operation *, int)));
    connect(this->netmodel, SIGNAL(updated()), this, SLOT(updated()));
    setupModelData(rootItem);
    reset();
}

void TreeModel::addEvent(const QModelIndex &/*selected*/)
{
    netmodel->addEvent();
}

void TreeModel::insertEvent(const QModelIndex &selected)
{
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        if (item->getEvent())
            netmodel->insertEvent(selected.row());
    }
}

void TreeModel::addOperation(const QModelIndex &selected)
{
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        QModelIndex parent;
        int i;
        if (item->getEvent())
        {
            i = item->getEvent()->getOutOperations().count();
            parent = selected;
        }
        else if (item->getOperation())
        {
            parent = selected.parent();
            i = item->getOperation()->getBeginEvent()->getOutOperations().count();
        }
        if (parent.isValid())
        {
            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            Event *event = parentItem->getEvent();
            if (event)
            {
                Operation *o = new Operation();
                o->setBeginEvent(event);
                if (netmodel->insertOperation(o, i))
                {
//                    QModelIndex parent1 = createIndex(parent.row(), 0, parent.internalPointer());
//                    event->insertOutOperation(o, i);
                }
                else
                    delete o;
            }
        }
    }
}

void TreeModel::insertOperation(const QModelIndex &selected)
{
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        QModelIndex parent;
        int i;
        if (item->getEvent())
        {
            i = 0;
            parent = selected;
        }
        else if (item->getOperation())
        {
            i = selected.row();
            parent = selected.parent();
        }
        if (parent.isValid())
        {
            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            Event *event = parentItem->getEvent();
            if (event)
            {
                Operation *o = new Operation();
                o->setBeginEvent(event);
                if (netmodel->insertOperation(o, i))
                {
                    //QModelIndex parent1 = createIndex(parent.row(), 0, parent.internalPointer());
                    //event->insertOutOperation(o, i);
                }
                else
                    delete o;
                //assert(event->getOutOperations().count()==
                assert(parentItem->childCount()==event->getOutOperations().count());
            }
        }
    }
}

void TreeModel::removeEvent(const QModelIndex &selected)
{
    if (selected.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
        Event *e = item->getEvent();
        if (e)
        {
            QList<Operation*> out = e->getOutOperations();
            foreach (Operation* o, out)
            {
                netmodel->removeOperation(o);
            }
            netmodel->removeEvent(e);
        }
    }
}

void TreeModel::removeOperation(const QModelIndex &selected)
{
    TreeItem *item = static_cast<TreeItem*>(selected.internalPointer());
    if (item->getOperation())
    {
        QModelIndex parent = selected.parent();
        int i = selected.row();
        if (parent.isValid())
        {
            TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
            Event *e = parentItem->getEvent();
            if (e && i<e->getOutOperations().count())
            {
                Operation *o = e->getOutOperations()[i];
                netmodel->removeOperation(o);
                assert(parentItem->childCount()==e->getOutOperations().count());
            }
        }
    }
}

void TreeModel::eventIdChanged(Event *e, int)
{
    for (int i=0;i<rootItem->childCount();++i)
    {
        if (rootItem->child(i)->getEvent()==e)
        {
            QModelIndex index = createIndex(i, 0, rootItem->child(i));
            emit dataChanged(index, index);
            break;
        }
    }
    //reset();
}

void TreeModel::eventNameChanged(Event *e, const QString &)
{
    for (int i=0;i<rootItem->childCount();++i)
    {
        if (rootItem->child(i)->getEvent()==e)
        {
            QModelIndex index = createIndex(i, 1, rootItem->child(i));
            emit dataChanged(index, index);
            break;
        }
    }
    //reset();
}

void TreeModel::operationEndEventChanged(Operation **, Event *)
{
    //reset();
}

void TreeModel::operationNameChanged(Operation *o, const QString &)
{
    Event *e = o->getBeginEvent();
    for (int i=0;i<rootItem->childCount();++i)
    {
        if (rootItem->child(i)->getEvent()==e)
        {
            for (int j=0;j<rootItem->child(i)->childCount();++j)
            {
                if (rootItem->child(i)->child(j)->getOperation()==o)
                {
                    QModelIndex index = createIndex(j, 3, rootItem->child(i)->child(j));
                    emit dataChanged(index, index);
                    break;
                }
            }
            break;
        }
    }
    //reset();
}

void TreeModel::operationWaitTimeChanged(Operation *o, double)
{
    Event *e = o->getBeginEvent();
    for (int i=0;i<rootItem->childCount();++i)
    {
        if (rootItem->child(i)->getEvent()==e)
        {
            for (int j=0;j<rootItem->child(i)->childCount();++j)
            {
                if (rootItem->child(i)->child(j)->getOperation()==o)
                {
                    QModelIndex index = createIndex(j, 4, rootItem->child(i)->child(j));
                    emit dataChanged(index, index);
                    break;
                }
            }
            break;
        }
    }
    //reset();
}

void TreeModel::afterEventAdd()
{
    Event *e = netmodel->last();
    beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
    rootItem->appendChild(new TreeItem(e, *rootItem));
    endInsertRows();
}

void TreeModel::beforeEventDelete(Event *e)
{
    for (int i=0;i<rootItem->childCount();++i)
        if (e==rootItem->child(i)->getEvent())
        {
            beginRemoveRows(QModelIndex(), i, i);
            rootItem->removeChild(i);
            endRemoveRows();
            break;
        }
}

void TreeModel::afterOperationAdd(Operation *o)
{
    Event *e = o->getBeginEvent();
    for (int i=0;i<rootItem->childCount();++i)
    {
        TreeItem *parent = rootItem->child(i);
        if (e==parent->getEvent())
        {
            TreeItem *item = new TreeItem(o, *parent);
            beginInsertRows(createIndex(i, 0, parent), parent->childCount(), parent->childCount());
            parent->appendChild(item);
            endInsertRows();
            assert(parent->childCount()-1==e->getOutOperations().indexOf(o));
            break;
        }
    }
}

void TreeModel::beforeOperationDelete(Operation *o)
{
    Event *e = o->getBeginEvent();
    for (int i=0;i<rootItem->childCount();++i)
    {
        TreeItem *parent = rootItem->child(i);
        if (e==parent->getEvent())
        {
            for (int j=0;j<parent->childCount();++j)
            {
                Operation *o1 = parent->child(j)->getOperation();
                if (o==o1)
                {
                    beginRemoveRows(createIndex(i, 0, parent), j, j);
                    parent->removeChild(j);
                    endRemoveRows();
                    break;
                }
            }
            break;
        }
    }
}

void TreeModel::afterEventInsert(int i)
{
    Event *e = netmodel->event(i);
    beginInsertRows(QModelIndex(), i, i);
    rootItem->insertChild(new TreeItem(e, *rootItem), i);
    endInsertRows();
}

void TreeModel::afterOperationInsert(Operation *o, int index)
{
    Event *e = o->getBeginEvent();
    for (int i=0;i<rootItem->childCount();++i)
    {
        TreeItem *parent = rootItem->child(i);
        if (e==parent->getEvent())
        {
            beginInsertRows(createIndex(i, 0, parent), index, index);
            parent->insertChild(new TreeItem(o, *parent), index);
            endInsertRows();
            break;
        }
    }
}

void TreeModel::updated()
{
    //reset();
}

void TreeModel::beforeClear()
{
    rootItem->removeAllChilds();
    disconnect(this, SLOT(beforeClear()));
    netmodel = NULL;
    reset();
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return 5;
}

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
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;//Qt::ItemIsDropEnabled;
    if ((index.column()==0||index.column()==1)
        && static_cast<TreeItem*>(index.internalPointer())->getEvent())
        return //Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else if ((index.column()==2||index.column()==3||index.column()==4)
        && static_cast<TreeItem*>(index.internalPointer())->getOperation())
        return //Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return 0;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role!=Qt::EditRole || !netmodel)
        return false;
    QVariant v = value;
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item->getEvent())
    {
        switch (index.column())
        {
            case 0:
                    if (v.convert(QVariant::Int)
                        && netmodel->setN(item->getEvent(), v.toInt()))
                    {
                        emit dataChanged(index,index);
                        return true;
                    }
                    else
                        return false;
            case 1:
                    if (v.convert(QVariant::String)
                        && netmodel->setName(item->getEvent(), v.toString()))
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
                    bool res = netmodel->setOperationEndEvent(&o, e);
                    if (res)
                    {
                        emit dataChanged(index,index);
                        return true;
                    } else return false;
                }
            case 3:
                if (v.convert(QVariant::String)
                    && netmodel->setOperationName(item->getOperation(), v.toString()))
                {
                    emit dataChanged(index,index);
                    return true;
                }
                else
                    return false;
            case 4:
                if (v.convert(QVariant::Double)
                    && netmodel->setOperationWaitTime(item->getOperation(), v.toDouble()))
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

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole
        && section>=0 && section<columnCount())
        return header[section];

    return QVariant();
}

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

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;
    if (netmodel)
    {
        if (!parent.isValid())
            return netmodel->getEvents()->count(); //parentItem = rootItem;
        else
        {
            parentItem = static_cast<TreeItem*>(parent.internalPointer());
            if (parentItem->getEvent())
                return parentItem->getEvent()->getOutOperations().count();
            else
                return 0;
            //        return childCount(parentItem);//parentItem->childCount();
        }
    }
    else
        return 0;
}

void TreeModel::setupModelData(TreeItem *parent)
{
    foreach (Event *e, *netmodel->getEvents())
    {
        TreeItem *eitem = new TreeItem(e, *parent);
        parent->appendChild(eitem);
        foreach (Operation *o, e->getOutOperations())
        {
            TreeItem *oitem = new TreeItem(o, *eitem);
            eitem->appendChild(oitem);
        }
    }
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
/*
bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if (!netmodel)
        return result;
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
                if (netmodel->insertOperation(o, i))
                {
                    QModelIndex parent1 = createIndex(parent.row(), 0, parent.internalPointer());
                    beginInsertRows(parent1, i, i);
                    event->insertOutOperation(o, i);
                    //TreeItem *item = new TreeItem(o, *parentItem);
                    //parentItem->insertChild(item, row);
                    endInsertRows();
                    result = result || true;
                }
                else
                    delete o;
            }
        }
        else
        {
            //Event *e = new Event(netmodel->generateId());
            if (netmodel->insertEvent(i))
            {
                beginInsertRows(parent, i, i);
                //Event *e = netmodel->event(i);
                //TreeItem *item = new TreeItem(e, *rootItem);
                //rootItem->insertChild(item, row);
                endInsertRows();
                result = result || true;
            }
            //else
            //    delete e;
        }
    }
    return result;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if (!netmodel)
        return result;
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
                if (netmodel->removeOperation(o))
                {
                    //parentItem->child(i)->clear();
                    //parentItem->removeChild(i);
                    result = result || true;
                }
            }
        }
        else
        {
            TreeItem *parentItem = rootItem;
            if (i<netmodel->getEvents()->count())//parentItem->childCount())
            {
                Event *e = netmodel->getEvents()->at(i);// parentItem->child(i)->getEvent();
                QList<Operation*> out = e->getOutOperations();
                foreach (Operation* o, out)
                {
                    netmodel->removeOperation(o);
                }
                if (netmodel->removeEvent(e))
                {
                    //parentItem->child(i)->clear();
                    //parentItem->removeChild(i);
                    result = result || true;
                }
            }
        }
    }
    endRemoveRows();
    return result;
}
*/

















