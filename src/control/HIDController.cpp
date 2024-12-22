//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#include <QThread>
#include <QDebug>

#include "HIDController.h"
#include "HIDDataFrame.h"
#include "HIDWork.h"

HIDController::HIDController(QObject* parent) : QObject(parent)
{
    qRegisterMetaType<HIDDataFrame>("HIDDataFrame");
    qRegisterMetaType<QByteArray>("QByteArray&");
    m_worker = new HIDWork(this);
    m_worker->start();

    connect(m_worker, &HIDWork::sigAddDevice, this, &HIDController::sigAddDevice);
    connect(m_worker, &HIDWork::sigRemoveDevice, this, &HIDController::sigRemoveDevice);
    connect(m_worker, &HIDWork::sigDeviceStatus, this, &HIDController::sigDeviceStatus);
    connect(m_worker, &HIDWork::sigReceiveCommand, this, &HIDController::sigReceiveCommand);
}

HIDController::~HIDController(){
    m_worker->stop();
    m_worker->quit();
    m_worker->wait();
}

void HIDController::onUpdateDeviceList() const
{
    m_worker->updateDeviceList();
}

void HIDController::openDevice(const QString& path) const
{
    m_worker->openDevice(path);
}

void HIDController::closeDevice(const QString& path) const
{
    m_worker->closeDevice(path);
}

HIDDevice* HIDController::getDevice(const QString& path) const
{
    return m_worker->getDevice(path);
}

void HIDController::sendCommand(const QString& path, const int cmd, const QByteArray& sendData, const int time) const
{
    m_worker->addCommand(path, cmd, sendData, time);
}

