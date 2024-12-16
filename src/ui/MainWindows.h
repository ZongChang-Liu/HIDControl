//
// Created by liu_zongchang on 2024/12/11 21:24.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_MAIN_WINDOWS_H
#define HID_CONTROL_MAIN_WINDOWS_H


#include "ElaWindow.h"
#include "ElaMenu.h"
#include "ElaLineEdit.h"
#include "ElaComboBox.h"
#include "ElaScrollPage.h"
#include "ElaSpinBox.h"
#include "ElaRadioButton.h"
#include "ElaCalendarPicker.h"
#include "ElaCheckBox.h"
#include "hidapi.h"

#pragma execution_character_set(push, "utf-8")

class LogPage;
class HIDFilterPage;
class MainWindows : public ElaWindow {
Q_OBJECT
public:
    explicit MainWindows(QWidget *parent = nullptr);

    ~MainWindows() override;

    void initSystem();

    void initWindow();

    void initEdgeLayout();

    void initContent();

    void initDeviceInfoUi();

    void initParamInfoUi();

    void initTimeInfoUi();

    void initModeInfoUi();

    void createDockWidget(const QString &title, QWidget *widget, Qt::DockWidgetArea area);

    Q_SLOT void onDIDSelectChanged(int index);

    Q_SLOT void updateDeviceList();

    Q_SLOT void updateDeviceInfo();

    Q_SLOT void showEvent(QShowEvent *event) override;

    Q_SLOT void hideEvent(QHideEvent *event) override;

    Q_SLOT void onActionTriggered(QAction *action);

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else

    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

#endif
#endif
private:
    ElaScrollPage *m_homePage{nullptr};
    QWidget *m_contentWidget{nullptr};

    struct HiDDeviceInfo {
        int vid;
        int pid;
        QString manufacturer;
        QString product;
        QString path;
        bool operator==(const HiDDeviceInfo &info) const {
            return vid == info.vid && pid == info.pid && manufacturer == info.manufacturer && product == info.product &&
                   path == info.path;
        }
    };
    QList<HiDDeviceInfo> m_hidDeviceInfoList;
    hid_device *m_hidDevice{nullptr};
    QWidget *m_deviceInfoWidget{nullptr};
    ElaComboBox *m_deviceComboBox{nullptr};
    ElaLineEdit *m_hidVidLineEdit{nullptr};
    ElaLineEdit *m_hidPidLineEdit{nullptr};
    ElaLineEdit *m_deviceManufacturerEditLine{nullptr};
    ElaLineEdit *m_deviceProductEditLine{nullptr};
    ElaLineEdit *m_deviceIdEditLine{nullptr};
    ElaLineEdit *m_deviceTypeEditLine{nullptr};
    ElaLineEdit *m_deviceVersionEditLine{nullptr};

    QWidget *m_paramInfoWidget{nullptr};
    ElaComboBox *m_paramComboBox{nullptr};
    ElaText *m_paramText{nullptr};
    ElaComboBox *m_paramTypeComboBox{nullptr};
    ElaLineEdit *m_paramThresholdLowEditLine{nullptr};
    ElaLineEdit *m_paramThresholdHighEditLine{nullptr};
    ElaSpinBox *m_paramUpdateIntervalSpinBox{nullptr};

    QWidget *m_timeInfoWidget{nullptr};
    ElaLineEdit *m_deviceTimeEditLine{nullptr};
    ElaLineEdit *m_systemTimeEditLine{nullptr};
    ElaComboBox *m_timeZoneHourComboBox{nullptr};
    ElaComboBox *m_timeZoneMinuteComboBox{nullptr};

    QWidget *m_modeInfoWidget{nullptr};
    ElaRadioButton *m_modeRadioButtonMode{nullptr};
    ElaRadioButton *m_modeRadioTimeMode{nullptr};
    ElaCheckBox *m_modeCheckBox{nullptr};
    ElaCalendarPicker *m_modeCalendarPicker{nullptr};

    HIDFilterPage *m_hidFilterPage{nullptr};
    LogPage *m_logPage{nullptr};
    ElaMenu *m_docketMenu = nullptr;

    ElaText *m_statusText{nullptr};
    QTimer *m_timer{nullptr};
};


#endif //HID_CONTROL_MAIN_WINDOWS_H
