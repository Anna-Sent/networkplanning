#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include "netmodel.h"

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
        delete childItems[position];
        childItems.removeAt(position);
    }
    void removeAllChilds()
    {
        qDeleteAll(childItems);
        childItems.clear();
    }
    void clear() {event=NULL;operation=NULL;}

    TreeItem *child(int row);
    int childCount() const {return childItems.count();}
    //int columnCount() const;
    //QVariant data(int column) const;
    int row() const;
    TreeItem *parent();

    Event *getEvent() {return event;}
    Operation *getOperation() {return operation;}
    //void setEvent(Event *e) {if (!operation) event=e;}
    //void setOperation(Operation *o) {if (!event) operation=o;}

private:
    QList<TreeItem*> childItems;
    //QList<QVariant> itemData;
    TreeItem *parentItem;
    Event *event;
    Operation *operation;
};

#endif
