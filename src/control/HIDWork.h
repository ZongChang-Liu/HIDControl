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
class HIDDataFrame;
class HIDWork final : public QObject
{
    Q_OBJECT
public:
    explicit HIDWork(QObject *parent = nullptr);
    ~HIDWork() override;

    hid_device_info* getDeviceInfo() const;

    Q_SLOT void openDevice(const QString& path);
    Q_SLOT void closeDevice();
    Q_SLOT void sendCommand(int cmd, const QByteArray& data, bool isAsync, int timeout);
    Q_SLOT void receiveCommand(int timeout);


    Q_SIGNAL void sigDeviceStatus(int status);
private:
    hid_device *m_hidDevice{nullptr};
    HIDDataFrame* m_frame{nullptr};
    QTimer* m_timer{nullptr};
    int code = 0;
};

#endif //HID_WORK_H
