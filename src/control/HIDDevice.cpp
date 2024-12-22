//
// Created by Zongc on 2024/12/22.
//

#include <QDebug>
#include <QThread>
#include "HIDDevice.h"

#include "HIDDataFrame.h"

HIDDevice::HIDDevice(hid_device_info* info, QObject* parent) : QObject(parent)
{
    m_path = info->path;
    m_info = info;
    m_frame = new HIDDataFrame();
}

HIDDevice::~HIDDevice() = default;

QString HIDDevice::path() const
{
    return m_path;
}

HIDDataFrame HIDDevice::frame() const
{
    return *m_frame;
}

bool HIDDevice::open()
{
    close();
    m_hidDevice = hid_open_path(m_path.toStdString().c_str());
    return m_hidDevice != nullptr;
}

void HIDDevice::close()
{
    hid_close(m_hidDevice);
    m_hidDevice = nullptr;
}

hid_device_info* HIDDevice::getDeviceInfo() const
{
    return m_info;
}

void HIDDevice::setUseInfo(const int type, const int id) const
{
    m_frame->reset();
    m_frame->m_userType = type;
    m_frame->m_userId = id;
}

bool HIDDevice::sendCommand(const int cmd, const QByteArray& data) const
{
    if (m_hidDevice == nullptr) {
        return false;
    }
    m_frame->m_code = cmd;
    m_frame->m_date = data;
    const unsigned char *command = HIDDataFrame::createCommand(*m_frame);
    for (int i = 0; i < m_frame->m_length / 64 + 1; ++i) {
        if ( i != 0) {
            QThread::msleep(10);
        }
        const int len = m_frame->m_length - i * 64 > 64 ? 64 : m_frame->m_length - i * 64;
        auto *sendData = new unsigned char[len + 1];
        sendData[0] = 0x00;
        memcpy(sendData + 1, command + i * 64, len);
        const int ret = hid_write(m_hidDevice, sendData, len + 1);
        if (ret < 0) {
            qDebug() << "HIDDevice----->" << __func__ << "write error";
            return false;
        }
    }
    qDebug() << "HIDDevice----->" << __func__ << "send data:" << *m_frame;
    return true;
}

bool HIDDevice::receiveCommand(QByteArray& receiveData, const int timeout) const
{
    if (m_hidDevice == nullptr) {
        return false;
    }

    int len = 0;
    unsigned char *recvData = nullptr;
    auto *data = new unsigned char[64];
    while(true){
        QThread::msleep(10);
        len += 64;
        memset(data, 0, 64);
        const int ret = hid_read_timeout(m_hidDevice, data, 64, timeout);
        if (ret < 0) {
            qDebug() << "HIDDevice----->" << __func__ << "read error";
            return false;
        }
        if (ret == 0) {
            qDebug() << "HIDDevice----->" << __func__ << "read timeout";
            return false;
        }
        if (recvData == nullptr) {
            recvData = new unsigned char[len];
            memcpy(recvData, data, 64);
        } else {
            auto *temp = new unsigned char[len];
            memcpy(temp, recvData, len - 64);
            memcpy(temp + len - 64, data, 64);
            delete[] recvData;
            recvData = temp;
        }
        if(HIDDataFrame::parseCommand(recvData,len, *m_frame)){

            receiveData = m_frame->m_date;
            break;
        }
    }
    delete[] recvData;
    delete[] data;
    qDebug() << "HIDDevice----->" << __func__ << "receive data:" << *m_frame;
    return true;
}

bool HIDDevice::operator==(const HIDDevice& device)const
{
    const hid_device_info *thisInfo = getDeviceInfo();
    const hid_device_info *info = device.getDeviceInfo();
    if (thisInfo == nullptr || info == nullptr) {
        return false;
    }
    return thisInfo->vendor_id == info->vendor_id && thisInfo->product_id == info->product_id && thisInfo->path == info->path && thisInfo->serial_number == info->serial_number;
}

QDebug operator<<(QDebug debug, const HIDDevice& device)
{
    const auto info = device.getDeviceInfo();
    if (info == nullptr) {
        return debug;
    }

    debug.nospace() << "\r\nHIDDevice[" << Qt::endl
                    << " vendor_id: " << "0x" + QString::number(info->vendor_id, 16).toUpper() << Qt::endl
                    << " product_id: " << "0x" + QString::number(info->product_id, 16).toUpper() << Qt::endl
                    << " path: " << QString(info->path) << Qt::endl
                    << " serial_number: " << QString::fromWCharArray(info->serial_number) << Qt::endl
                    << " manufacturer: " << QString::fromWCharArray(info->manufacturer_string) << Qt::endl
                    << " product: " << QString::fromWCharArray(info->product_string) << Qt::endl
                    << " version: " << QString::number(info->release_number, 16).toUpper()
                    << "]";
    return debug;
}
