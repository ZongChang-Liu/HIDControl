//
// Created by liu_zongchang on 2024/12/16 15:18.
// Email 1439797751@qq.com
//
//

#ifndef HID_WORK_H
#define HID_WORK_H
#include <QObject>
#include <QMap>
#include <QThread>
#pragma execution_character_set(push, "utf-8")
class HIDDevice;
class HIDDataFrame;
class HIDWork final : public QThread
{
    Q_OBJECT
public:
    class CMD
    {
    public:
        QString path = nullptr;
        int code = 0;
        QByteArray data = nullptr;
        int timeout = 50;
    };

    explicit HIDWork(QObject *parent = nullptr);
    ~HIDWork() override;

    void updateDeviceList();
    HIDDevice* getDevice(const QString& path) const;

    int openDevice(const QString& path = nullptr);
    int closeDevice(const QString& path = nullptr);

    void addCommand(const QString& path,int cmd, const QByteArray& send, int timeout = 50);

    void stop();
    void run() override;

    Q_SIGNAL void sigDeviceStatus(const QString& path,int status);
    Q_SIGNAL void sigAddDevice(const QString& path);
    Q_SIGNAL void sigRemoveDevice(const QString& path);
    Q_SIGNAL void sigReceiveCommand(const QString& path,const HIDDataFrame& data);
private:
    const QString PRODUCT{"FRESVUE"};
    QMap<QString,HIDDevice*> m_hidDeviceMap;
    QList<CMD> m_cmdList;
    int user_type{0x01};
    int user_id{0x12345678};
    bool m_isStop{false};
};

#endif //HID_WORK_H
