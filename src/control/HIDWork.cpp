//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#include <QDebug>
#include "HIDWork.h"
#include "HIDDef.h"

HIDWork::HIDWork(QObject* parent) : QObject(parent)
{
}

HIDWork::~HIDWork() = default;

hid_device_info* HIDWork::getDeviceInfo() const
{
    if (m_hidDevice == nullptr)
    {
        return nullptr;
    }

    return hid_get_device_info(m_hidDevice);
}

void HIDWork::openDevice(const QString& path)
{
    closeDevice();
    m_hidDevice = hid_open_path(path.toStdString().c_str());
    if (m_hidDevice == nullptr) {
        Q_EMIT sigDeviceStatus(HIDDef::Device_Error);
        return;
    }
    if (hid_set_nonblocking(m_hidDevice, 1) != 0)// 1启用非阻塞  0禁用非阻塞。
    {
        qDebug() << "hid_set_nonblocking failed";
    }
    Q_EMIT sigDeviceStatus(HIDDef::Device_Opened);
}

void HIDWork::closeDevice()
{
    if (m_hidDevice != nullptr) {
        hid_close(m_hidDevice);
        Q_EMIT sigDeviceStatus(HIDDef::Device_Closed);
        m_hidDevice = nullptr;
    }
}

void HIDWork::sendCommand(int cmd, QByteArray data, bool isAsync, int timeout)
{
}
