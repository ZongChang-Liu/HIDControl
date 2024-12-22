//
// Created by liu_zongchang on 2024/12/12 21:02.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_HID_FILTER_PAGE_H
#define HID_CONTROL_HID_FILTER_PAGE_H


#include <QAbstractListModel>
#include <QWidget>


class HIDDeviceListModel final : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit HIDDeviceListModel(QObject *parent = nullptr);
    ~HIDDeviceListModel() override;

    void addDevice(const QString& id);
    void removeDevice(const QString& id);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
private:
    QStringList m_idList;
};

class ElaListView;
class HIDFilterPage final : public QWidget {
    Q_OBJECT
public:
    explicit HIDFilterPage(QWidget *parent = nullptr);
    ~HIDFilterPage() override;
    void addHIDItem(const QString& path, const QString& id);
    void removeHIDItem(const QString& path);

    Q_SIGNAL void sigHIDSelected(bool isSelected, QString id);
private:
    QMap<QString,QString> m_hidDeviceMap;
    ElaListView *m_hidListView{nullptr};
    HIDDeviceListModel *m_hidDeviceListModel{nullptr};
};


#endif //HID_CONTROL_HID_FILTER_PAGE_H
