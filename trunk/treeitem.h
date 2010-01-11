#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include "netmodel.h"

//! [0]
class TreeItem
{
public:
    TreeItem(TreeItem *parent = 0):parentItem(parent),event(NULL),operation(NULL) {}
    TreeItem(Event *e, TreeItem &parent):parentItem(&parent),event(e),operation(NULL) {}
    TreeItem(Operation *o, TreeItem &parent):parentItem(&parent),event(NULL),operation(o) {}
    ~TreeItem();

    void appendChild(TreeItem *child);
    void insertChild(TreeItem *child, int position) {childItems.insert(position,child);}
    void removeChild(int position)
    {
        delete child(position);
        childItems.removeAt(position);
    }
    void removeAllChilds()
    {
        qDeleteAll(childItems);
        childItems.clear();
    }
    void clear() {event=NULL;operation=NULL;}

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    //QVariant data(int column) const;
    int row() const;
    TreeItem *parent();

    Event *getEvent() {return event;}
    Operation *getOperation() {return operation;}
    //void setEvent(Event *e) {if (!operation) event=e;}
    //void setOperation(Operation *o) {if (!event) operation=o;}

private:
    QList<TreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItem *parentItem;
    Event *event;
    Operation *operation;
};
//! [0]
/*
class EventTreeItem : public TreeItem
{
    private:
        Event *event;
    public:
        EventTreeItem(Event *e, const QList<QVariant> &data, TreeItem *parent = 0) :
                TreeItem(data, parent), event(e) { }
        Event *getEvent() {return event;}
};

class OperationTreeItem : public TreeItem
{
    private:
        Operation *operation;
    public:
        OperationTreeItem(Operation *o, const QList<QVariant> &data, TreeItem *parent = 0) :
                TreeItem(data, parent), operation(o) { }
        Operation *getOperation() {return operation;}
};
*/
#endif
