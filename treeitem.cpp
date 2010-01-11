/*
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "treeitem.h"

TreeItem::~TreeItem()
{
    removeAllChilds();
}

void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}
/*
int TreeItem::childCount() const
{
    if (event)
        return event->getOutOperations().count();
    else if (operation)
        return 0;
    else
        return ;
//    return childItems.count();
}*/
/*
int TreeItem::columnCount() const
{
    return itemData.count();
}*/

TreeItem *TreeItem::parent()
{
    return parentItem;
}

int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
