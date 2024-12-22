//
// Created by liu_zongchang on 2024/12/12 21:02.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QDebug>
#include "HIDFilterPage.h"

#include <ElaListView.h>
#include <ElaScrollPageArea.h>
#include <QLabel>


HIDDeviceListModel::HIDDeviceListModel(QObject* parent) : QAbstractListModel(parent)
{
}

HIDDeviceListModel::~HIDDeviceListModel() = default;

void HIDDeviceListModel::addDevice(const QString& id)
{
    beginInsertRows(QModelIndex(),m_idList.count(),m_idList.count());
    m_idList.append(id);
    endInsertRows();

    qDebug() << "add Device: " << id << "Count: " << m_idList.count();
}

void HIDDeviceListModel::removeDevice(const QString& id)
{
    beginRemoveRows(QModelIndex(),m_idList.count(),m_idList.count());
    m_idList.removeOne(id);
    endRemoveRows();
    qDebug() << "Remove Device: " << id << "Count: " << m_idList.count();
}

int HIDDeviceListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_idList.count();
}

QVariant HIDDeviceListModel::data(const QModelIndex& index, const int role) const
{
    if (index.row() < 0 || index.row() >= m_idList.count())
    {
        return QVariant{};
    }

    if (role == Qt::DisplayRole)
    {
        return m_idList.at(index.row());
    }
    return QVariant{};
}

HIDFilterPage::HIDFilterPage(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    auto *label = new QLabel(tr("Device List"), this);
    QFont font;
    font.setBold(true);
    font.setPixelSize(15);
    label->setFont(font);
    mainLayout->addWidget(label);
    this->setMinimumSize(200, 300);
    this->setMaximumWidth(300);
    m_hidListView = new ElaListView(this);
    mainLayout->addWidget(m_hidListView);
    m_hidDeviceListModel = new HIDDeviceListModel(this);
    m_hidListView->setModel(m_hidDeviceListModel);
    m_hidListView->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(m_hidListView, &ElaListView::clicked, [this](const QModelIndex& index){
        const QString id = m_hidDeviceListModel->data(index, Qt::DisplayRole).toString();
        emit sigHIDSelected(m_hidListView->selectionModel()->isSelected(index), m_hidDeviceMap.key(id));
    });
}

HIDFilterPage::~HIDFilterPage() = default;

void HIDFilterPage::addHIDItem(const QString& path, const QString& id)
{
    if (m_hidDeviceMap.contains(path))
    {
        return;
    }

    m_hidDeviceMap.insert(path, id);
    m_hidDeviceListModel->addDevice(id);
    if (m_hidDeviceMap.count() == 1)
    {
        m_hidListView->selectionModel()->select(m_hidDeviceListModel->index(0), QItemSelectionModel::Select);
        emit sigHIDSelected(true, path);
    }
}

void HIDFilterPage::removeHIDItem(const QString& path)
{
    const QString id = m_hidDeviceMap.value(path);
    m_hidDeviceMap.remove(path);
    m_hidDeviceListModel->removeDevice(id);
}
