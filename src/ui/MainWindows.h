//
// Created by liu_zongchang on 2024/12/11 21:24.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_MAIN_WINDOWS_H
#define HID_CONTROL_MAIN_WINDOWS_H


#include <QDateTime>
#include "ElaWindow.h"
#include "ElaMenu.h"
#include "ElaLineEdit.h"
#include "ElaComboBox.h"
#include "ElaScrollPage.h"
#include "ElaSpinBox.h"
#include "ElaDoubleSpinBox.h"
#include "ElaCalendarPicker.h"
#include "ElaScrollPageArea.h"

#pragma execution_character_set(push, "utf-8")

class QLabel;
class LogPage;
class HIDFilterPage;
class HIDController;
class HIDDevice;
class HIDDataFrame;
class MainWindows final : public ElaWindow {
Q_OBJECT
public:
    explicit MainWindows(QWidget *parent = nullptr);

    ~MainWindows() override;

    void initSystem() const;

    void initWindow();

    void initEdgeLayout();

    void initContent();

    void initDeviceInfoUi();

    void initAlarmSettingUi();

    void initStartConfigUi();

    void createDockWidget(const QString &title, QWidget *widget, Qt::DockWidgetArea area);

    void clearInfo();

    Q_SLOT void onSetTempType() const;

    Q_SLOT void onSetThreshold() const;

    Q_SLOT void onSetLogInterval() const;

    Q_SLOT void onSetStartDelay() const;

    Q_SLOT void onSetDeviceTime();

    Q_SLOT void onSetMode() const;

    Q_SLOT void onStopMode() const;

    Q_SLOT void onDeviceStatus(const QString& path,int status) const;

    Q_SLOT void onAddDevice(const QString& path);

    Q_SLOT void onRemoveDevice(const QString& path);

    Q_SLOT void onParseCommand(const QString& path,const HIDDataFrame& data);

    Q_SLOT void onDeviceSelected(bool isSelected, const QString& path);

    Q_SLOT void showEvent(QShowEvent *event) override;

    Q_SLOT void hideEvent(QHideEvent *event) override;

    Q_SLOT void onActionTriggered(const QAction *action) const;
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

    QString m_currentDevicePath;
    HIDController *m_hidController{nullptr};
    ElaScrollPageArea *m_deviceInfoContent{nullptr};
    QLabel *m_modelId{nullptr};
    QLabel *m_deviceId{nullptr};
    QLabel *m_version{nullptr};
    ElaComboBox *m_deviceZoneHourComboBox{nullptr};
    ElaComboBox *m_deviceZoneMinuteComboBox{nullptr};
    QDateTime m_deviceTime;
    QLabel *m_deviceTimeLabel{nullptr};
    ElaComboBox *m_temperatureScaleComboBox{nullptr};
    ElaSpinBox *m_logIntervalSpinBox{nullptr};
    ElaText *m_logIntervalText{nullptr};

    ElaScrollPageArea *m_AlarmSettingContent{nullptr};
    ElaDoubleSpinBox *m_highTempSpinBox{nullptr};
    ElaDoubleSpinBox *m_lowTempSpinBox{nullptr};
    ElaSpinBox *m_highDelaySpinBox{nullptr};
    ElaSpinBox *m_lowDelaySpinBox{nullptr};

    ElaScrollPageArea *m_startConfigContent{nullptr};
    ElaSpinBox *m_startIntervalSpinBox{nullptr};
    ElaText *m_startIntervalText{nullptr};
    ElaCalendarPicker* m_startTimeCalendarPicker{nullptr};
    ElaComboBox *m_startTimeHourComboBox{nullptr};
    ElaComboBox *m_startTimeMinuteComboBox{nullptr};


    HIDFilterPage *m_hidFilterPage{nullptr};
    ElaMenu *m_docketMenu = nullptr;

    ElaText *m_statusText{nullptr};
    QTimer *m_timer{nullptr};
};


#endif //HID_CONTROL_MAIN_WINDOWS_H
