//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#ifndef HID_MODEL_H
#define HID_MODEL_H
#include <QObject>

class HIDDevice;
class HIDWork;
class HIDDataFrame;
class HIDController final : public QObject
{
    Q_OBJECT
public:
    explicit HIDController(QObject *parent = nullptr);
    ~HIDController() override;

    void onUpdateDeviceList() const;

    void openDevice(const QString& path = nullptr) const;
    void closeDevice(const QString& path = nullptr) const;
    HIDDevice* getDevice(const QString& path) const;
    void sendCommand(const QString& path,int cmd, const QByteArray& sendData, int time = 50) const;

    Q_SIGNAL void sigReceiveCommand(QString path,const HIDDataFrame& data);
    Q_SIGNAL void sigAddDevice(QString path);
    Q_SIGNAL void sigRemoveDevice(QString path);
    Q_SIGNAL void sigDeviceStatus(QString path, int status);
private:
    HIDWork* m_worker;
};



#endif //HID_MODEL_H
