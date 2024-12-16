//
// Created by liu_zongchang on 2024/12/12 21:02.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_HID_FILTER_PAGE_H
#define HID_CONTROL_HID_FILTER_PAGE_H


#include <QWidget>
#include <QStandardItemModel>

class ElaTreeView;
class HIDModel;
class HIDFilterPage : public QWidget {
    Q_OBJECT
public:
    explicit HIDFilterPage(QWidget *parent = nullptr);
    ~HIDFilterPage() override;
    void addHIDItem(const QString& manufacturer, const QString& product, const QString& vid , const QString& pid);
    void removeHIDItem(const QString& manufacturer, const QString& product, const QString &vid, const QString &pid);
    Q_SIGNAL void sigHIDSelected(bool isSelected, const QString& vid, const QString& pid);
private:
    HIDModel* m_hidModel{nullptr};
    ElaTreeView *m_hidTreeView{nullptr};
};


#endif //HID_CONTROL_HID_FILTER_PAGE_H
