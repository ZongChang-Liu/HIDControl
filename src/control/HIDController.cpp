//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#include "HIDController.h"
#include <QThread>
HIDController::HIDController(QObject* parent) : QObject(parent)
{
    m_thread = new QThread(this);
    m_worker = new HIDWork();
    m_worker->moveToThread(m_thread);
    m_thread->start();
    connect(m_worker, &HIDWork::sigDeviceStatus, this, &HIDController::sigDeviceStatus);
}

HIDController::~HIDController(){
    closeDevice();
    m_thread->quit();
    m_thread->wait();
};

void HIDController::openDevice(const QString& path) const
{
    QMetaObject::invokeMethod(m_worker, "openDevice", Qt::QueuedConnection, Q_ARG(QString, path));
}

void HIDController::closeDevice() const
{
    m_worker->closeDevice();
}

int HIDController::sendCommand(const int cmd, const QByteArray& data, const bool isAsync, const int timeout) const
{
    if (isAsync)
    {
        int ret;
        QMetaObject::invokeMethod(m_worker, "sendCommand", Qt::BlockingQueuedConnection,
            Q_RETURN_ARG(int,ret), Q_ARG(int, cmd), Q_ARG(QByteArray, data), Q_ARG(bool,isAsync), Q_ARG(int, timeout));
        return ret;
    }

    QMetaObject::invokeMethod(m_worker, "sendCommand", Qt::QueuedConnection, Q_ARG(int, cmd), Q_ARG(QByteArray, data), Q_ARG(bool,isAsync), Q_ARG(int, timeout));
    return 0;
}

hid_device_info* HIDController::getDeviceInfo() const
{
    return m_worker->getDeviceInfo();
}