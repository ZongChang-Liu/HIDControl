//
// Created by liu_zongchang on 2024/12/22 9:54.
// Email 1439797751@qq.com
//


#ifndef HID_DEVICE_H
#define HID_DEVICE_H
#include <hidapi.h>
#include <QObject>

class HIDDataFrame;
class HIDDevice final : public QObject
{
    Q_OBJECT
public:
    explicit HIDDevice(hid_device_info *info, QObject *parent = nullptr);
    ~HIDDevice() override;
    QString path() const;
    HIDDataFrame frame() const;
    bool open();
    void close();

    hid_device_info* getDeviceInfo() const;

    void setUseInfo(int type, int id) const;

    bool sendCommand(int cmd, const QByteArray &data) const;
    bool receiveCommand(QByteArray &receiveData,int timeout) const;

    bool operator==(const HIDDevice &device) const;
    friend QDebug operator<<(QDebug debug, const HIDDevice &device);
private:
    QString m_path;
    hid_device_info* m_info{nullptr};
    hid_device *m_hidDevice{nullptr};
    HIDDataFrame* m_frame{nullptr};
};



#endif //HID_DEVICE_H
