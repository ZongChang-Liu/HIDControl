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

#pragma execution_character_set(push, "utf-8")

class HIDSelectPage;
class LogPage;
class MainWindows : public ElaWindow{
    Q_OBJECT
public:
    explicit MainWindows(QWidget* parent = nullptr);
    ~MainWindows() override;

    void initWindow();
    void initEdgeLayout();
    void initContent();

    void initDeviceInformation();
    void initParamInformation();
    void initTimeInformation();
    void initModeInformation();

    void createDockWidget(const QString& title,QWidget *widget,Qt::DockWidgetArea area);

    Q_SLOT void showEvent(QShowEvent *event) override;
    Q_SLOT void hideEvent(QHideEvent *event) override;
    Q_SLOT void onActionTriggered(QAction *action);
private:
    ElaScrollPage* m_homePage{nullptr};
    QWidget* m_contentWidget{nullptr};

    QWidget* m_deviceInfoWidget{nullptr};
    ElaLineEdit* m_deviceIdEditLine{nullptr};
    ElaLineEdit* m_deviceTypeEditLine{nullptr};
    ElaLineEdit* m_deviceVersionEditLine{nullptr};

    QWidget* m_paramInfoWidget{nullptr};
    ElaComboBox* m_paramComboBox{nullptr};
    ElaText* m_paramText{nullptr};
    ElaComboBox* m_paramTypeComboBox{nullptr};
    ElaLineEdit* m_paramThresholdLowEditLine{nullptr};
    ElaLineEdit* m_paramThresholdHighEditLine{nullptr};
    ElaSpinBox* m_paramUpdateIntervalSpinBox{nullptr};

    QWidget* m_timeInfoWidget{nullptr};
    ElaLineEdit* m_deviceTimeEditLine{nullptr};
    ElaLineEdit* m_systemTimeEditLine{nullptr};
    ElaComboBox* m_timeZoneHourComboBox{nullptr};
    ElaComboBox* m_timeZoneMinuteComboBox{nullptr};

    QWidget* m_modeInfoWidget{nullptr};
    ElaRadioButton* m_modeRadioButtonMode{nullptr};
    ElaRadioButton* m_modeRadioTimeMode{nullptr};
    ElaCheckBox* m_modeCheckBox{nullptr};
    ElaCalendarPicker* m_modeCalendarPicker{nullptr};


    HIDSelectPage* m_hidSelectPage{nullptr};
    LogPage* m_logPage{nullptr};
    ElaMenu* m_docketMenu = nullptr;

    ElaText* m_statusText{nullptr};
    QTimer* m_timer{nullptr};
};


#endif //HID_CONTROL_MAIN_WINDOWS_H
