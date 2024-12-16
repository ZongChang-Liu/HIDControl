//
// Created by liu_zongchang on 2024/12/16 0:48.
// Email 1439797751@qq.com
//
//

#include "HIDModel.h"
#include "TreeItem.h"
HIDModel::HIDModel(QObject* parent)
    : QAbstractItemModel{parent}
{
    _rootItem = new TreeItem("root");
}

HIDModel::~HIDModel()
{
    delete _rootItem;
}
QModelIndex HIDModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
    {
        return QModelIndex{};
    }
    auto* childItem = static_cast<TreeItem*>(child.internalPointer());
    TreeItem* parentItem = childItem->getParentItem();
    if (parentItem == _rootItem || parentItem == nullptr)
    {
        return QModelIndex{};
    }
    return createIndex(parentItem->getRow(), 0, parentItem);
}

QModelIndex HIDModel::index(int row, int column) const {
    return index(row, column, QModelIndex());
}
QModelIndex HIDModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex{};
    }
    TreeItem* parentItem;
    if (!parent.isValid())
    {
        parentItem = _rootItem;
    }
    else
    {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }
    TreeItem* childItem = nullptr;
    if (parentItem->getChildrenItems().count() > row)
    {
        childItem = parentItem->getChildrenItems().at(row);
    }
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    return QModelIndex{};
}


int HIDModel::rowCount() const
{
    return rowCount(QModelIndex());
}

int HIDModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem;
    if (parent.column() > 0)
    {
        return 0;
    }
    if (!parent.isValid())
    {
        parentItem = _rootItem;
    }
    else
    {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }
    return parentItem->getChildrenItems().count();
}

int HIDModel::columnCount() const {
    return columnCount(QModelIndex());
}

int HIDModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}


QVariant HIDModel::data(const QModelIndex &index) const {
    return data(index, Qt::DisplayRole);
}

QVariant HIDModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return static_cast<TreeItem*>(index.internalPointer())->getItemTitle();
    }
    else if (role == Qt::CheckStateRole)
    {
        auto* item = static_cast<TreeItem*>(index.internalPointer());
        if (item->getIsHasChild())
        {
            return item->getChildCheckState();
        }
        else
        {
            return item->getIsChecked() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant{};
}


bool HIDModel::setData(const QModelIndex &index, const QVariant &value) {
    return setData(index, value, Qt::CheckStateRole);
}

bool HIDModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        auto* item = static_cast<TreeItem*>(index.internalPointer());
        item->setIsChecked(!item->getIsChecked());
        item->setChildChecked(item->getIsChecked());
        Q_EMIT dataChanged(QModelIndex{}, QModelIndex{}, {role});
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags HIDModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsUserCheckable;
    return flags;
}


QVariant HIDModel::headerData(int section, Qt::Orientation orientation) const {
    return headerData(section, orientation, Qt::DisplayRole);
}

QVariant HIDModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return QString("HID Model");
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

int HIDModel::getItemCount() const
{
    return this->_itemsMap.count();
}






