//
// Created by liu_zongchang on 2024/12/16 0:37.
// Email 1439797751@qq.com
//
//

#include <QUuid>
#include "TreeItem.h"

TreeItem::TreeItem(const QString& itemTitle, TreeItem *parent) : QObject(parent) {
    m_itemKey = QUuid::createUuid().toString().remove("{").remove("}").remove("-");
    m_itemTitle = itemTitle;
    m_parentItem = parent;
    m_isChecked = false;
}

TreeItem::~TreeItem() {
    qDeleteAll(m_childrenItems);
}

QList<TreeItem *> TreeItem::getChildrenItems() const {
    return m_childrenItems;
}

void TreeItem::setChildrenItems(const QList<TreeItem *> &childrenItems) {
    m_childrenItems = childrenItems;
    Q_EMIT sigChildrenItemsChanged();
}

bool TreeItem::getIsChecked() const {
    return m_isChecked;
}

void TreeItem::setIsChecked(bool isChecked) {
    m_isChecked = isChecked;
    Q_EMIT sigIsCheckedChanged();
}

TreeItem *TreeItem::getParentItem() const {
    return m_parentItem;
}

void TreeItem::setParentItem(TreeItem *parentItem) {
    m_parentItem = parentItem;
    Q_EMIT sigParentItemChanged();
}

QString TreeItem::getItemKey() const {
    return m_itemKey;
}

QString TreeItem::getItemTitle() const {
    return m_itemTitle;
}

void TreeItem::setChildChecked(bool isChecked) {
    if (isChecked)
    {
        for (auto node : m_childrenItems)
        {
            node->setIsChecked(isChecked);
            node->setChildChecked(isChecked);
        }
    }
    else
    {
        for (auto node : m_childrenItems)
        {
            node->setChildChecked(isChecked);
            node->setIsChecked(isChecked);
        }
    }
}

Qt::CheckState TreeItem::getChildCheckState() {
    bool isAllChecked = true;
    bool isAnyChecked = false;
    for (auto node : m_childrenItems)
    {
        if (node->getIsChecked())
        {
            isAnyChecked = true;
        }
        else
        {
            isAllChecked = false;
        }
        Qt::CheckState childState = node->getChildCheckState();
        if (childState == Qt::PartiallyChecked)
        {
            isAllChecked = false;
            isAnyChecked = true;
            break;
        }
        else if (childState == Qt::Unchecked)
        {
            isAllChecked = false;
        }
    }
    if (m_childrenItems.count() > 0)
    {
        if (isAllChecked)
        {
            return Qt::Checked;
        }
        if (isAnyChecked)
        {
            return Qt::PartiallyChecked;
        }
        return Qt::Unchecked;
    }
    return Qt::Checked;
}

void TreeItem::appendChildItem(TreeItem *childItem) {
    m_childrenItems.append(childItem);
}

void TreeItem::removeChildItem(TreeItem *childItem) {
    m_childrenItems.removeOne(childItem);
}

bool TreeItem::getIsHasChild() const {
    if (m_childrenItems.count() > 0)
    {
        return true;
    }
    return false;
}

int TreeItem::getRow() const {
    if (m_parentItem)
    {
        return m_parentItem->getChildrenItems().indexOf(const_cast<TreeItem*>(this));
    }
    return 0;
}
