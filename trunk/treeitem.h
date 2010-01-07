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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include "netmodel.h"

//! [0]
class TreeItem
{
public:
    TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);
    void insertChild(TreeItem *child, int position) {childItems.insert(position,child);}
    void removeChild(int position)
    {
        delete child(position);
        childItems.removeAt(position);
    }
    void clear() {event=NULL;operation=NULL;}

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parent();

    Event *getEvent() {return event;}
    Operation *getOperation() {return operation;}
    void setEvent(Event *e) {if (!operation) event=e;}
    void setOperation(Operation *o) {if (!event) operation=o;}

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
