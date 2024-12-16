//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#ifndef HID_WORK_H
#define HID_WORK_H
#include <hidapi.h>
#include <QObject>
#pragma execution_character_set(push, "utf-8")

class HIDWork final : public QObject
{
    Q_OBJECT
public:
    explicit HIDWork(QObject *parent = nullptr);
    ~HIDWork() override;

    hid_device_info* getDeviceInfo() const;

    Q_SLOT void openDevice(const QString& path);
    Q_SLOT void closeDevice();
    Q_SLOT void sendCommand(int cmd, QByteArray data, bool isAsync = false, int timeout = 50);

    Q_SIGNAL void sigDeviceStatus(int status);
    Q_SIGNAL void sigCommandResult(int cmd, QByteArray data);
private:
    hid_device *m_hidDevice{nullptr};
};

#endif //HID_WORK_H
