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

HIDWork::HIDWork(QObject* parent) : QObject(parent)
{
    m_frame = new HIDDataFrame();
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
//    if (hid_set_nonblocking(m_hidDevice, 0) != 0)// 1启用非阻塞  0禁用非阻塞。
//    {
//        qDebug() << "hid_set_nonblocking failed";
//    }
    Q_EMIT sigDeviceStatus(HIDDef::Device_Opened);
    m_timer = new QTimer(this);

//    connect(m_timer, &QTimer::timeout, this, [=]() {
//    });
    m_timer->start(100);
}

void HIDWork::closeDevice()
{
    if (m_hidDevice != nullptr) {
        hid_close(m_hidDevice);
        Q_EMIT sigDeviceStatus(HIDDef::Device_Closed);
        m_hidDevice = nullptr;
    }

    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

void HIDWork::sendCommand(int cmd, const QByteArray& data, bool isAsync, int timeout)
{
    if (m_hidDevice == nullptr)
    {
        return;
    }
    m_frame->m_userType = 0x01;
    m_frame->m_userId = 0x12345678;
    m_frame->m_code = cmd;
    m_frame->m_date = data;

    unsigned char *command = HIDDataFrame::createCommand(*m_frame);
    for (int i = 0; i < m_frame->m_length / 64 + 1; ++i) {
        if ( i != 0) {
            QThread::msleep(10);
        }

        int len = m_frame->m_length - i * 64 > 64 ? 64 : m_frame->m_length - i * 64;
        auto *sendData = new unsigned char[len + 1];
        sendData[0] = 0x00;
        memcpy(sendData + 1, command + i * 64, len);
        qDebug() << "HIDWork---->" << __func__ << i << QByteArray(reinterpret_cast<const char*>(sendData), len + 1).toHex();
        int ret = hid_write(m_hidDevice, sendData, len + 1);
        if (ret < 0) {
            qDebug() << "HIDWork---->" << __func__ << "failed" << QString::fromWCharArray(hid_error(m_hidDevice));
        }
    }

    receiveCommand(timeout);
}

void HIDWork::receiveCommand(int timeout) {
    if (m_hidDevice == nullptr)
    {
        return;
    }

    int len = 0;
    unsigned char *recvData = nullptr;
    auto *data = new unsigned char[64];
    while(true){
        len += 64;
        memset(data, 0, 64);
        int ret = hid_read_timeout(m_hidDevice, data, 64, timeout);
        if (ret < 0) {
            qDebug() << "HIDWork---->" << __func__ << "failed" << QString::fromWCharArray(hid_error(m_hidDevice));
            return;
        }
        if (ret == 0) {
            qDebug() << "HIDWork---->" << __func__ << "timeout";
            return;
        }
        if (recvData == nullptr) {
            recvData = new unsigned char[len];
            memcpy(recvData, data, 64);
        } else {
            auto *temp = new unsigned char[len];
            memcpy(temp, data, 64);
            memcpy(temp + len - 64, recvData, len - 64);
            delete[] recvData;
            recvData = temp;
        }
        qDebug() << "HIDWork---->" << __func__ << QByteArray(reinterpret_cast<const char*>(recvData), len).toHex();
        if(HIDDataFrame::parseCommand(recvData,len, *m_frame)){
            break;
        }
        QThread::msleep(10);
    }
    delete[] recvData;
    delete[] data;
}
