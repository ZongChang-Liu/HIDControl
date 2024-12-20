//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#ifndef HID_MODEL_H
#define HID_MODEL_H
#include <QObject>

#include "HIDWork.h"


class HIDController final : public QObject
{
    Q_OBJECT
public:
    explicit HIDController(QObject *parent = nullptr);
    ~HIDController() override;
    void openDevice(const QString& path) const;
    void closeDevice() const;
    int sendCommand(int cmd, const QByteArray& data, bool isAsync = false, int timeout = 50) const;
    hid_device_info* getDeviceInfo() const;

    Q_SIGNAL void sigDeviceStatus(int status);
private:
    QThread* m_thread;
    HIDWork* m_worker;
};



#endif //HID_MODEL_H
