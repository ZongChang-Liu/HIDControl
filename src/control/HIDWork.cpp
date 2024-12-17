//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#include <QDebug>
#include <QTimer>
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
    if (hid_set_nonblocking(m_hidDevice, 1) != 0)// 1启用非阻塞  0禁用非阻塞。
    {
        qDebug() << "hid_set_nonblocking failed";
    }
    Q_EMIT sigDeviceStatus(HIDDef::Device_Opened);
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, [=]() {
        code++;
        if (code == HIDDef::Setting_Device_Stop + 1) {
            code = HIDDef::Query_Device_Id;
        } else if (code == HIDDef::Query_Device_Time + 1) {
            code = HIDDef::Setting_Temp_Type;
        }
        switch (code) {
            case HIDDef::Setting_Temp_Type:
            {
                uint8_t tempType = 0x01;
                QByteArray data;
                data.append(static_cast<char>(tempType));
                sendCommand(HIDDef::Setting_Temp_Type, data, false, 50);
            }
                break;
            case HIDDef::Setting_Value_Threshold:
            {
                QByteArray data;
                uint8_t valueId = 0x01;
                uint32_t low = 0x01;
                uint32_t high = 0x10000000;
                data.append(static_cast<char>(high));
                data.append(static_cast<char>(high >> 8));
                data.append(static_cast<char>(high >> 16));
                data.append(static_cast<char>(high >> 24));
                data.append(static_cast<char>(low));
                data.append(static_cast<char>(low >> 8));
                data.append(static_cast<char>(low >> 16));
                data.append(static_cast<char>(low >> 24));
                data.append(static_cast<char>(valueId));
                sendCommand(HIDDef::Setting_Value_Threshold, data, false, 50);
            }
                break;
            case HIDDef::Setting_Log_Interval:
            {
                uint8_t hh = 0x12;
                uint8_t mm = 0x30;
                uint8_t t = 0x50;
                QByteArray data;
                data.append(static_cast<char>(hh));
                data.append(static_cast<char>(mm));
                data.append(static_cast<char>(t));
                sendCommand(HIDDef::Setting_Log_Interval, data, false, 50);
            }
                break;
            case HIDDef::Setting_Start_Delay:
            {
                uint8_t hh = 0x06;
                uint8_t mm = 0x15;
                QByteArray data;
                data.append(static_cast<char>(hh));
                data.append(static_cast<char>(mm));
                sendCommand(HIDDef::Setting_Start_Delay, data, false, 50);
            }
                break;
            case HIDDef::Setting_Device_Time:
            {
                uint8_t yy = 0x24;
                uint8_t MM = 0x12;
                uint8_t dd = 0x16;
                uint8_t hh = 0x21;
                uint8_t mm = 0x45;
                uint8_t ss = 0x30;
                uint8_t week = 0x01;
                QByteArray data;
                data.append(static_cast<char>(yy));
                data.append(static_cast<char>(MM));
                data.append(static_cast<char>(dd));
                data.append(static_cast<char>(hh));
                data.append(static_cast<char>(mm));
                data.append(static_cast<char>(ss));
                data.append(static_cast<char>(week));
                sendCommand(HIDDef::Setting_Device_Time, data, false, 50);
            }
                break;
            case HIDDef::Setting_Device_Timezone:{

            }
                break;
            case HIDDef::Setting_Start_Mode:{
                uint8_t mode = 0x01;
                //时间戳
                uint32_t value = QDateTime::currentDateTimeUtc().toTime_t();
                QByteArray data;
                data.append(static_cast<char>(mode));
                data.append(static_cast<char>(value));
                data.append(static_cast<char>(value >> 8));
                data.append(static_cast<char>(value >> 16));
                data.append(static_cast<char>(value >> 24));
                sendCommand(HIDDef::Setting_Start_Mode, data, false, 50);
            }
                break;
            case HIDDef::Setting_Device_Stop:
            {
                sendCommand(HIDDef::Setting_Device_Stop, QByteArray(), false, 50);
            }
                break;
            case HIDDef::Query_Device_Id:{
                sendCommand(HIDDef::Query_Device_Id, QByteArray(), false, 50);
            }
                break;
            case HIDDef::Query_Device_Model:{
                sendCommand(HIDDef::Query_Device_Model, QByteArray(), false, 50);
            }
                break;
            case HIDDef::Query_Device_Version:{
                sendCommand(HIDDef::Query_Device_Version, QByteArray(), false, 50);
            }
                break;
            case HIDDef::Query_Device_ValueNum:{
                sendCommand(HIDDef::Query_Device_ValueNum, QByteArray(), false, 50);
            }
                break;
            case HIDDef::Query_Log_Info:
            {
                uint16_t index = 0x0101;
                QByteArray data;
                data.append(static_cast<char>(index));
                data.append(static_cast<char>(index >> 8));
                sendCommand(HIDDef::Query_Log_Info, data, false, 50);
            }
                break;
            case HIDDef::Query_Device_Time:
            {
                sendCommand(HIDDef::Query_Device_Time, QByteArray(), false, 50);
            }
                break;
        }
    });
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
    auto *sendData = new unsigned char[m_frame->m_length + 1];
    sendData[0] = 0x00;
    memcpy(sendData + 1, command, m_frame->m_length);
    qDebug() << "HIDWork---->" << __func__ << QByteArray(reinterpret_cast<const char*>(sendData), m_frame->m_length + 1).toHex();
    int ret = hid_write(m_hidDevice, sendData, m_frame->m_length + 1);
    if (ret < 0) {
       qDebug() << "HIDWork---->" << __func__ << "failed" << QString::fromWCharArray(hid_error(m_hidDevice));
    }
}

void HIDWork::receiveCommand(const QByteArray &data) {

}
