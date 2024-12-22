//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QDateTime>
#include "HIDWork.h"
#include "HIDDef.h"
#include "HIDDataFrame.h"
#include "HIDDevice.h"

HIDWork::HIDWork(QObject* parent) : QThread(parent)
{
}

HIDWork::~HIDWork() = default;

void HIDWork::updateDeviceList()
{
    hid_device_info *hid_info = hid_enumerate(0x0, 0x0);
    QList<HIDDevice*> hidDeviceList;
    for (hid_device_info *info = hid_info; info != nullptr; info = info->next) {
        QString product = QString::fromWCharArray(info->manufacturer_string);
        if (product.isEmpty() || !product.contains(PRODUCT)) {
            continue;
        }

        auto* device = new HIDDevice(info);
        device->setUseInfo(user_type, user_id);
        hidDeviceList.append(device);
    }
    hid_free_enumeration(hid_info);

    for (const auto &device: m_hidDeviceMap) {
        if (!hidDeviceList.contains(device)) {
            Q_EMIT sigRemoveDevice(device->path());
            m_hidDeviceMap.remove(device->path());
            device->close();
            delete device;
        }
    }


    for (const auto &device: hidDeviceList) {
        const auto info = device->getDeviceInfo();
        if (info == nullptr) {
            continue;
        }

        if (!m_hidDeviceMap.contains(info->path)) {
            m_hidDeviceMap.insert(info->path, device);
            Q_EMIT sigAddDevice(info->path);
        }
    }


}

HIDDevice* HIDWork::getDevice(const QString& path) const
{
    HIDDevice* device = m_hidDeviceMap.value(path);
    if (device == nullptr) {
        return nullptr;
    }
    return device;
}

int HIDWork::openDevice(const QString& path)
{
    if (path == nullptr) {
        // open all devices
        for (const auto &device: m_hidDeviceMap) {
            if (device->open()) {
                Q_EMIT sigDeviceStatus(device->getDeviceInfo()->path,HIDDef::Device_Opened);
            } else {
                Q_EMIT sigDeviceStatus(device->getDeviceInfo()->path,HIDDef::Device_Error);
            }
        }
    }
    else {
        auto *device = getDevice(path);
        if (device == nullptr) {
            return -1;
        }
        if (device->open()) {
            Q_EMIT sigDeviceStatus(path,HIDDef::Device_Opened);
        } else {
            Q_EMIT sigDeviceStatus(path,HIDDef::Device_Error);
        }
    }
    return 0;
}

int HIDWork::closeDevice(const QString& path)
{
    if (path == nullptr) {
        // close all devices
        for (const auto &device: m_hidDeviceMap) {
            device->close();
            Q_EMIT sigDeviceStatus(device->getDeviceInfo()->path,HIDDef::Device_Closed);
        }
    }
    else {
        auto *device = getDevice(path);
        if (device == nullptr) {
            return -1;
        }
        device->close();
        Q_EMIT sigDeviceStatus(path,HIDDef::Device_Closed);
    }
    return 0;
}

void HIDWork::stop()
{
    m_isStop = true;
    wait();
}

void HIDWork::run()
{
    while (!m_isStop) {
        if (m_cmdList.isEmpty()) {
            msleep(100);
            continue;
        }
        const auto command = m_cmdList.takeFirst();
        if (command.path.isEmpty()) {
            continue;
        }
        if (command.code == 0) {
            continue;
        }

        const auto *device = getDevice(command.path);
        if (device == nullptr) {
            continue;
        }
        if (!device->sendCommand(command.code, command.data)) {
            continue;
        }

        QByteArray receive;
        device->receiveCommand(receive, command.timeout);
        Q_EMIT sigReceiveCommand(command.path, device->frame());
        msleep(10);
    }
}

void HIDWork::addCommand(const QString& path, const int cmd, const QByteArray& send, const int timeout)
{
    CMD command;
    command.path = path;
    command.code = cmd;
    command.data = send;
    command.timeout = timeout;
    m_cmdList.append(command);
}
