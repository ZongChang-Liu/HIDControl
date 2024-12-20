//
// Created by liu_zongchang on 2024/12/11 21:24.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include "MainWindows.h"
#include "ElaStatusBar.h"
#include "ElaText.h"
#include "ElaDockWidget.h"
#include "ElaMenuBar.h"
#include "ElaScrollPageArea.h"

#include "control/HIDDataFrame.h"
#include "control/HIDController.h"
#include "control/HIDDef.h"
#include "ui/LogPage.h"
#include "ui/HIDFilterPage.h"

#ifdef Q_OS_WIN

#include <windows.h>
#include <initguid.h>
#include <Hidclass.h>
#include <Dbt.h>
#include <QTimeZone>

#endif

MainWindows::MainWindows(QWidget *parent) : ElaWindow(parent) {

    m_hidController = new HIDController(this);
    initSystem();
    initWindow();
    initEdgeLayout();
    initContent();

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, [=]() {
        QString time = tr("当前时间：") + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        m_systemTimeEditLine->setText(
                tr(QDateTime::currentDateTime().toString("yyyy年 MM月 dd日 hh时 mm分 ss秒").toUtf8()));
        m_statusText->setText(time);
    });

    connect(m_deviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDIDSelectChanged(int)));
    connect(m_hidController, SIGNAL(sigDeviceStatus(int)), this, SLOT(onDeviceStatus(int)));
    connect(m_paramTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetTempType()));
    connect(m_paramThresholdLowSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onSetThreshold()));
    connect(m_paramThresholdHighSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onSetThreshold()));
    connect(m_paramUpdateIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSetLogInterval()));
    connect(m_deviceTimeCalendarPicker, SIGNAL(selectedDateChanged(QDate)), this, SLOT(onSetDeviceTime()));
    connect(m_deviceTimeHourComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetDeviceTime()));
    connect(m_deviceTimeMinuteComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetDeviceTime()));
    connect(m_deviceTimeSecondComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetDeviceTime()));
    connect(m_timeZoneHourComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetDeviceTime()));
    connect(m_timeZoneMinuteComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetDeviceTime()));
    connect(m_modeRadioButtonMode, SIGNAL(clicked()), this, SLOT(onSetMode()));
    connect(m_modeRadioTimeMode, SIGNAL(clicked()), this, SLOT(onSetMode()));
    connect(m_modeCheckBox, SIGNAL(clicked()), this, SLOT(onSetStartDelay()));
    connect(m_modeCalendarPicker, SIGNAL(selectedDateChanged(QDate)), this, SLOT(onSetMode()));
    connect(m_modeHourComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetMode()));
    connect(m_modeMinuteComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetMode()));
    connect(m_modeSecondComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetMode()));


    if(m_logPage){
        m_logPage->appendLog(tr("初始化完成！"));
    }
    updateDeviceList();
}

MainWindows::~MainWindows() = default;


void MainWindows::initSystem() {
#ifdef Q_OS_WIN
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification((HANDLE) this->winId(), &NotificationFilter,
                                                       DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr) {
        qDebug() << "HID Register Failed";
    } else {
        qDebug() << "HID Register Success";
    }
#endif
}


void MainWindows::initWindow() {
    this->setUserInfoCardPixmap(QPixmap(":/resources/logo.png"));
    this->setUserInfoCardTitle("HID Control");
    this->setUserInfoCardSubTitle("1439797751@QQ.com");
    this->setWindowTitle("HID Control");
    this->setWindowIcon(QIcon(":/resources/logo.png"));
    this->setMinimumSize(600, 400);
    this->setIsStayTop(false);
    this->setIsNavigationBarEnable(true);
    this->setIsCentralStackedWidgetTransparent(false);
    this->setWindowButtonFlags(
            ElaAppBarType::MinimizeButtonHint | ElaAppBarType::MaximizeButtonHint | ElaAppBarType::CloseButtonHint |
            ElaAppBarType::ThemeChangeButtonHint);
    this->moveToCenter();
}

void MainWindows::initEdgeLayout() {
    auto *menuBar = new ElaMenuBar(this);
    m_docketMenu = new ElaMenu(tr("窗口"), this);
    menuBar->addMenu(m_docketMenu);
    menuBar->setFixedHeight(30);
    auto *customWidget = new QWidget(this);
    auto *customLayout = new QVBoxLayout(customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(menuBar);
    customLayout->addStretch();
    this->setCustomWidget(ElaAppBarType::LeftArea, customWidget);


    menuBar->addAction(tr("读取设备信息"));
    connect(menuBar, &ElaMenuBar::triggered, this, &MainWindows::onActionTriggered);

    // m_hidFilterPage = new HIDFilterPage(this);
    // createDockWidget(tr("HID过滤"), m_hidFilterPage, Qt::RightDockWidgetArea);

//    m_logPage = new LogPage(this);
//    createDockWidget(tr("日志"), m_logPage, Qt::RightDockWidgetArea);

    auto *statusBar = new ElaStatusBar(this);
    QString time = tr("当前时间：") + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_statusText = new ElaText(time, this);
    m_statusText->setFixedWidth(240);
    m_statusText->setTextPixelSize(14);
    statusBar->addWidget(m_statusText);
    this->setStatusBar(statusBar);
}

void MainWindows::initContent() {
    m_homePage = new ElaScrollPage(this);
    m_contentWidget = new QWidget(this);

    initDeviceInfoUi();
    initParamInfoUi();
    initTimeInfoUi();
    initModeInfoUi();

    auto *centerLayout = new QVBoxLayout(m_contentWidget);
    centerLayout->addWidget(m_deviceInfoWidget);
    centerLayout->addWidget(m_paramInfoWidget);
    centerLayout->addWidget(m_timeInfoWidget);
    centerLayout->addWidget(m_modeInfoWidget);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 15, 150);
    m_homePage->setTitleVisible(false);
    m_homePage->addCentralWidget(m_contentWidget, true, true, 0);
    addPageNode(tr("设备配置"), m_homePage, ElaIconType::GearComplex);
}

void MainWindows::createDockWidget(const QString &title, QWidget *widget, Qt::DockWidgetArea area) {
    auto *dockWidget = new ElaDockWidget(title, this);
    dockWidget->setObjectName(title);
    widget->setParent(dockWidget);
    dockWidget->setDockWidgetTitleIconVisible(false);
    dockWidget->setWidget(widget);
    this->addDockWidget(area, dockWidget);
    this->resizeDocks({dockWidget}, {200}, Qt::Horizontal);

    auto *action = dockWidget->toggleViewAction();
    action->setText(title);
    action->setCheckable(true);
    action->setChecked(true);
    m_docketMenu->addAction(action);
}

void MainWindows::onActionTriggered(QAction *action) {
    if(action->text() == tr("读取设备信息")){
        m_hidController->sendCommand(HIDDef::Query_Device_Info, nullptr, false, 50);
    }
}

void MainWindows::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    m_timer->start();
}

void MainWindows::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    m_timer->stop();
}

void MainWindows::initDeviceInfoUi() {
    m_deviceInfoWidget = new QWidget(m_contentWidget);
    auto *deviceInfoText = new ElaText(tr("设备信息"), m_deviceInfoWidget);
    auto *m_deviceInfoContent = new ElaScrollPageArea(m_deviceInfoWidget);
    m_deviceInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(m_deviceInfoContent);

    auto *devicePathWidget = new QWidget(m_deviceInfoWidget);
    auto *devicePathLayout = new QHBoxLayout(devicePathWidget);
    auto *devicePathText = new ElaText(tr("设备路径:"), m_deviceInfoWidget);
    devicePathText->setTextPixelSize(15);
    devicePathText->setFixedWidth(70);
    m_deviceComboBox = new ElaComboBox(m_deviceInfoWidget);
    m_deviceComboBox->setFixedHeight(30);
    m_deviceComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *deviceVidText = new ElaText(tr("VID:"), m_deviceInfoWidget);
    deviceVidText->setTextPixelSize(15);
    m_hidVidLineEdit = new ElaLineEdit(m_deviceInfoWidget);
    m_hidVidLineEdit->setFixedHeight(30);
    m_hidVidLineEdit->setFixedWidth(100);
    m_hidVidLineEdit->setReadOnly(true);
    auto *devicePidText = new ElaText(tr("PID:"), m_deviceInfoWidget);
    devicePidText->setTextPixelSize(15);
    m_hidPidLineEdit = new ElaLineEdit(m_deviceInfoWidget);
    m_hidPidLineEdit->setFixedHeight(30);
    m_hidPidLineEdit->setFixedWidth(100);
    m_hidPidLineEdit->setReadOnly(true);

    devicePathLayout->addWidget(devicePathText);
    devicePathLayout->addWidget(m_deviceComboBox);
    devicePathLayout->addWidget(deviceVidText);
    devicePathLayout->addWidget(m_hidVidLineEdit);
    devicePathLayout->addWidget(devicePidText);
    devicePathLayout->addWidget(m_hidPidLineEdit);

    auto *deviceProductWidget = new QWidget(m_deviceInfoWidget);
    auto *deviceProductLayout = new QHBoxLayout(deviceProductWidget);
    auto *deviceManufacturerText = new ElaText(tr("制造商:"), m_deviceInfoWidget);
    deviceManufacturerText->setTextPixelSize(15);
    m_deviceManufacturerEditLine = new ElaLineEdit(m_deviceInfoWidget);
    m_deviceManufacturerEditLine->setReadOnly(true);
    m_deviceManufacturerEditLine->setFixedHeight(30);
    auto *deviceProductText = new ElaText(tr("产品:"), m_deviceInfoWidget);
    deviceProductText->setTextPixelSize(15);
    m_deviceProductEditLine = new ElaLineEdit(m_deviceInfoWidget);
    m_deviceProductEditLine->setReadOnly(true);
    m_deviceProductEditLine->setFixedHeight(30);

    deviceProductLayout->addWidget(deviceManufacturerText);
    deviceProductLayout->addWidget(m_deviceManufacturerEditLine);
    deviceProductLayout->addWidget(deviceProductText);
    deviceProductLayout->addWidget(m_deviceProductEditLine);

    auto *deviceIdWidget = new QWidget(m_deviceInfoWidget);
    auto *deviceIdLayout = new QHBoxLayout(deviceIdWidget);
    auto *deviceIdText = new ElaText(tr("设备ID:"), m_deviceInfoWidget);
    deviceIdText->setTextPixelSize(15);
    m_deviceIdEditLine = new ElaLineEdit(m_deviceInfoWidget);
    m_deviceIdEditLine->setReadOnly(true);
    m_deviceIdEditLine->setFixedHeight(30);

    auto *deviceTypeText = new ElaText(tr("设备类型:"), m_deviceInfoWidget);
    deviceTypeText->setTextPixelSize(15);
    deviceTypeText->setFixedWidth(70);
    m_deviceTypeEditLine = new ElaLineEdit(m_deviceInfoWidget);
    m_deviceTypeEditLine->setReadOnly(true);
    m_deviceTypeEditLine->setFixedHeight(30);

    auto *deviceVersionText = new ElaText(tr("设备版本:"), m_deviceInfoWidget);
    deviceVersionText->setTextPixelSize(15);
    deviceVersionText->setFixedWidth(70);
    m_deviceVersionEditLine = new ElaLineEdit(m_deviceInfoWidget);
    m_deviceVersionEditLine->setReadOnly(true);
    m_deviceVersionEditLine->setFixedHeight(30);

    deviceIdLayout->addWidget(deviceIdText);
    deviceIdLayout->addWidget(m_deviceIdEditLine);
    deviceIdLayout->addWidget(deviceTypeText);
    deviceIdLayout->addWidget(m_deviceTypeEditLine);
    deviceIdLayout->addWidget(deviceVersionText);
    deviceIdLayout->addWidget(m_deviceVersionEditLine);

    contentLayout->addWidget(devicePathWidget);
    contentLayout->addWidget(deviceProductWidget);
    contentLayout->addWidget(deviceIdWidget);

    auto *deviceInfoLayout = new QVBoxLayout(m_deviceInfoWidget);
    deviceInfoLayout->setContentsMargins(0, 0, 0, 0);
    deviceInfoLayout->addWidget(deviceInfoText);
    deviceInfoLayout->addWidget(m_deviceInfoContent);

    m_deviceInfoContent->setFixedHeight(180);

    deviceInfoText->setVisible(false);
//    devicePathWidget->setVisible(false);
//    deviceProductWidget->setVisible(false);
//    m_deviceInfoContent->setFixedHeight(70);
}

void MainWindows::initParamInfoUi() {
    m_paramInfoWidget = new QWidget(m_contentWidget);

    auto *paramInfoText = new ElaText(tr("参数设置"), m_paramInfoWidget);
    auto *paramInfoContent = new ElaScrollPageArea(m_paramInfoWidget);
    paramInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(paramInfoContent);

    auto *paramTypeWidget = new QWidget(m_paramInfoWidget);
    auto *paramTypeLayout = new QHBoxLayout(paramTypeWidget);
    m_paramComboBox = new ElaComboBox(m_paramInfoWidget);
    m_paramComboBox->setFixedHeight(30);
    m_paramComboBox->addItems({"参数1", "参数2", "参数3", "参数4", "参数5"});
    m_paramComboBox->setMinimumWidth(200);
    m_paramText = new ElaText("参数类型：温度", m_paramInfoWidget);
    m_paramText->setTextPixelSize(15);
    auto *paramTypeText = new ElaText("单位：", m_paramInfoWidget);
    paramTypeText->setTextPixelSize(15);
    m_paramTypeComboBox = new ElaComboBox(m_paramInfoWidget);
    m_paramTypeComboBox->addItems({tr("摄氏度 ℃"), tr("华氏度 ℉")});
    paramTypeLayout->addWidget(m_paramComboBox);
    paramTypeLayout->addSpacing(30);
    paramTypeLayout->addWidget(m_paramText);
    paramTypeLayout->addSpacing(30);
    paramTypeLayout->addWidget(paramTypeText);
    paramTypeLayout->addWidget(m_paramTypeComboBox);
    paramTypeLayout->addStretch();

    auto *paramThresholdWidget = new QWidget(m_paramInfoWidget);
    auto *paramThresholdLayout = new QHBoxLayout(paramThresholdWidget);
    auto *paramThresholdLowText = new ElaText("低阈值：", m_paramInfoWidget);
    paramThresholdLowText->setTextPixelSize(15);
    m_paramThresholdLowSpinBox = new ElaDoubleSpinBox(m_paramInfoWidget);
    m_paramThresholdLowSpinBox->setRange(-9999, 9999);
    m_paramThresholdLowSpinBox->setDecimals(1);
    m_paramThresholdLowSpinBox->setValue(16.0);
    m_paramThresholdLowSpinBox->setFixedHeight(30);

    auto *paramThresholdHighText = new ElaText("高阈值：", m_paramInfoWidget);
    paramThresholdHighText->setTextPixelSize(15);
    m_paramThresholdHighSpinBox = new ElaDoubleSpinBox(m_paramInfoWidget);
    m_paramThresholdHighSpinBox->setRange(-9999, 9999);
    m_paramThresholdHighSpinBox->setDecimals(1);
    m_paramThresholdHighSpinBox->setValue(32.0);
    m_paramThresholdHighSpinBox->setFixedHeight(30);

    paramThresholdLayout->addWidget(paramThresholdLowText);
    paramThresholdLayout->addWidget(m_paramThresholdLowSpinBox);
    paramThresholdLayout->addSpacing(30);
    paramThresholdLayout->addWidget(paramThresholdHighText);
    paramThresholdLayout->addWidget(m_paramThresholdHighSpinBox);
    paramThresholdLayout->addStretch();


    auto *paramUpdateWidget = new QWidget(m_paramInfoWidget);
    auto *paramUpdateLayout = new QHBoxLayout(paramUpdateWidget);
    paramUpdateLayout->setAlignment(Qt::AlignLeft);
    auto *paramUpdateText = new ElaText("上报时间间隔：", m_paramInfoWidget);
    paramUpdateText->setTextPixelSize(15);
    m_paramUpdateIntervalSpinBox = new ElaSpinBox(m_paramInfoWidget);
    m_paramUpdateIntervalSpinBox->setFixedHeight(30);
    m_paramUpdateIntervalSpinBox->setMinimumWidth(200);
    m_paramUpdateIntervalSpinBox->setSuffix(tr("分钟"));
    m_paramUpdateIntervalSpinBox->setRange(1, 28800);
    auto *paramUpdateDayText = new ElaText("", m_paramInfoWidget);
    paramUpdateDayText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    paramUpdateDayText->setTextPixelSize(15);
    paramUpdateDayText->setText(tr("每1分钟上报一次"));
    connect(m_paramUpdateIntervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        if (value <= 60) {
            paramUpdateDayText->setText(tr("每%1分钟上报一次").arg(QString::number(value)));
        } else if (value <= 1440) {
            int hour = value / 60;
            int minute = value % 60;
            QString text = tr("每%1小时%2上报一次").arg(QString::number(hour),
                                                        minute == 0 ? "" : tr("%1分钟").arg(QString::number(minute)));
            paramUpdateDayText->setText(text);
        } else {
            int day = value / 1440;
            int hour = (value % 1440) / 60;
            QString text = tr("每%1天%2小时上报一次").arg(QString::number(day),
                                                          hour == 0 ? "" : tr("%1小时").arg(QString::number(hour)));
            paramUpdateDayText->setText(text);
        }
    });

    paramUpdateLayout->addWidget(paramUpdateText);
    paramUpdateLayout->addWidget(m_paramUpdateIntervalSpinBox);
    paramUpdateLayout->addWidget(paramUpdateDayText);

    contentLayout->addWidget(paramTypeWidget);
    contentLayout->addWidget(paramThresholdWidget);
    contentLayout->addWidget(paramUpdateWidget);

    auto *paramInfoLayout = new QVBoxLayout(m_paramInfoWidget);
    paramInfoLayout->setContentsMargins(0, 0, 0, 0);
    paramInfoLayout->addWidget(paramInfoText);
    paramInfoLayout->addWidget(paramInfoContent);

    paramInfoContent->setFixedHeight(180);

    paramInfoText->setVisible(false);
}

void MainWindows::initTimeInfoUi() {
    m_timeInfoWidget = new QWidget(m_contentWidget);

    auto *timeInfoText = new ElaText(tr("时间设置"), m_timeInfoWidget);
    auto *m_timeInfoContent = new ElaScrollPageArea(m_timeInfoWidget);
    m_timeInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(m_timeInfoContent);

    auto *deviceTimeWidget = new QWidget(m_paramInfoWidget);
    auto *deviceTimeLayout = new QHBoxLayout(deviceTimeWidget);
    auto *deviceTimeText = new ElaText("设备时间：", m_timeInfoWidget);
    deviceTimeText->setTextPixelSize(15);

    m_deviceTimeCalendarPicker = new ElaCalendarPicker(m_modeInfoWidget);
    m_deviceTimeCalendarPicker->setFixedHeight(30);
    m_deviceTimeHourComboBox = new ElaComboBox(m_modeInfoWidget);
    m_deviceTimeHourComboBox->addItems({"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                        "20", "21", "22", "23"});
    m_deviceTimeHourComboBox->setFixedHeight(30);
    m_deviceTimeMinuteComboBox = new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        if (i < 10) {
            m_deviceTimeMinuteComboBox->addItem("0" + QString::number(i));
        } else {
            m_deviceTimeMinuteComboBox->addItem(QString::number(i));
        }
    }
    m_deviceTimeMinuteComboBox->setFixedHeight(30);
    m_deviceTimeSecondComboBox = new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        if (i < 10) {
            m_deviceTimeSecondComboBox->addItem("0" + QString::number(i));
        } else {
            m_deviceTimeSecondComboBox->addItem(QString::number(i));
        }
    }
    m_deviceTimeSecondComboBox->setFixedHeight(30);
    auto *modeTimeHourText = new ElaText("时", m_modeInfoWidget);
    modeTimeHourText->setTextPixelSize(15);
    auto *modeTimeMinuteText = new ElaText("分", m_modeInfoWidget);
    modeTimeMinuteText->setTextPixelSize(15);
    auto *modeTimeSecondText = new ElaText("秒", m_modeInfoWidget);
    modeTimeSecondText->setTextPixelSize(15);

    deviceTimeLayout->addWidget(deviceTimeText);
    deviceTimeLayout->addWidget(m_deviceTimeCalendarPicker);
    deviceTimeLayout->addWidget(m_deviceTimeHourComboBox);
    deviceTimeLayout->addWidget(modeTimeHourText);
    deviceTimeLayout->addWidget(m_deviceTimeMinuteComboBox);
    deviceTimeLayout->addWidget(modeTimeMinuteText);
    deviceTimeLayout->addWidget(m_deviceTimeSecondComboBox);
    deviceTimeLayout->addWidget(modeTimeSecondText);
    deviceTimeLayout->addStretch();

    auto *systemTimeWidget = new QWidget(m_paramInfoWidget);
    auto *systemTimeLayout = new QHBoxLayout(systemTimeWidget);
    auto *systemTimeText = new ElaText("系统时间：", m_timeInfoWidget);
    systemTimeText->setTextPixelSize(15);
    m_systemTimeEditLine = new ElaLineEdit(m_timeInfoWidget);
    m_systemTimeEditLine->setReadOnly(true);
    m_systemTimeEditLine->setFixedHeight(30);
    systemTimeLayout->addWidget(systemTimeText);
    systemTimeLayout->addWidget(m_systemTimeEditLine);

    auto *timeZoneWidget = new QWidget(m_paramInfoWidget);
    auto *timeZoneLayout = new QHBoxLayout(timeZoneWidget);
    auto *timeZoneText = new ElaText("时  区：", m_timeInfoWidget);
    timeZoneText->setTextPixelSize(15);
    m_timeZoneHourComboBox = new ElaComboBox(m_timeInfoWidget);
    m_timeZoneHourComboBox->addItems({"-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "UTC",
                                      "+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9", "+10", "+11", "+12"});
    m_timeZoneHourComboBox->setCurrentText("UTC");
    m_timeZoneHourComboBox->setFixedHeight(30);
    auto *timeZoneColon = new ElaText(" : ", m_timeInfoWidget);
    timeZoneColon->setTextPixelSize(15);
    m_timeZoneMinuteComboBox = new ElaComboBox(m_timeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        m_timeZoneMinuteComboBox->addItem(QString::number(i));
    }
    m_timeZoneMinuteComboBox->setFixedHeight(30);
    timeZoneLayout->addWidget(timeZoneText);
    timeZoneLayout->addWidget(m_timeZoneHourComboBox);
    timeZoneLayout->addWidget(timeZoneColon);
    timeZoneLayout->addWidget(m_timeZoneMinuteComboBox);
    timeZoneLayout->addStretch();

    contentLayout->addWidget(deviceTimeWidget);
    contentLayout->addWidget(timeZoneWidget);
    contentLayout->addWidget(systemTimeWidget);

    auto *timeInfoLayout = new QVBoxLayout(m_timeInfoWidget);
    timeInfoLayout->setContentsMargins(0, 0, 0, 0);
    timeInfoLayout->addWidget(timeInfoText);
    timeInfoLayout->addWidget(m_timeInfoContent);

    m_timeInfoContent->setFixedHeight(180);

    timeInfoText->setVisible(false);
}

void MainWindows::initModeInfoUi() {
    m_modeInfoWidget = new QWidget(m_contentWidget);

    auto *modeInfoText = new ElaText(tr("模式设置"), m_modeInfoWidget);
    auto *modeInfoContent = new ElaScrollPageArea(m_modeInfoWidget);
    modeInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(modeInfoContent);

    auto *startModeWidget = new QWidget(m_modeInfoWidget);
    auto *startModeLayout = new QHBoxLayout(startModeWidget);
    auto *startModeText = new ElaText("启动模式：", m_modeInfoWidget);
    startModeText->setTextPixelSize(15);
    m_modeRadioButtonMode = new ElaRadioButton(tr("按钮模式"), m_modeInfoWidget);
    m_modeRadioButtonMode->setChecked(true);
    m_modeRadioTimeMode = new ElaRadioButton(tr("时间模式"), m_modeInfoWidget);
    startModeLayout->addWidget(startModeText);
    startModeLayout->addWidget(m_modeRadioButtonMode);
    startModeLayout->addWidget(m_modeRadioTimeMode);
    startModeLayout->addStretch();

    auto *modeTimeWidget = new QWidget(m_modeInfoWidget);
    auto *modeTimeLayout = new QHBoxLayout(modeTimeWidget);
    auto *modeTimeDayText = new ElaText("启动期日：", m_modeInfoWidget);
    modeTimeDayText->setTextPixelSize(15);
    m_modeCalendarPicker = new ElaCalendarPicker(m_modeInfoWidget);
    m_modeCalendarPicker->setFixedHeight(30);
    m_modeHourComboBox = new ElaComboBox(m_modeInfoWidget);
    m_modeHourComboBox->addItems({"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                    "20", "21", "22", "23"});
    m_modeHourComboBox->setFixedHeight(30);
    m_modeMinuteComboBox= new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        if (i < 10) {
            m_modeMinuteComboBox->addItem("0" + QString::number(i));
        } else {
            m_modeMinuteComboBox->addItem(QString::number(i));
        }
    }
    m_modeMinuteComboBox->setFixedHeight(30);
    m_modeSecondComboBox = new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        if (i < 10) {
            m_modeSecondComboBox->addItem("0" + QString::number(i));
        } else {
            m_modeSecondComboBox->addItem(QString::number(i));
        }
    }
    m_modeSecondComboBox->setFixedHeight(30);
    auto *modeTimeHourText = new ElaText("时", m_modeInfoWidget);
    modeTimeHourText->setTextPixelSize(15);
    auto *modeTimeMinuteText = new ElaText("分", m_modeInfoWidget);
    modeTimeMinuteText->setTextPixelSize(15);
    auto *modeTimeSecondText = new ElaText("秒", m_modeInfoWidget);
    modeTimeSecondText->setTextPixelSize(15);
    modeTimeLayout->addWidget(modeTimeDayText);
    modeTimeLayout->addWidget(m_modeCalendarPicker);
    modeTimeLayout->addWidget(m_modeHourComboBox);
    modeTimeLayout->addWidget(modeTimeHourText);
    modeTimeLayout->addWidget(m_modeMinuteComboBox);
    modeTimeLayout->addWidget(modeTimeMinuteText);
    modeTimeLayout->addWidget(m_modeSecondComboBox);
    modeTimeLayout->addWidget(modeTimeSecondText);
    modeTimeLayout->addStretch();
    modeTimeWidget->setVisible(false);

    auto *stopModeWidget = new QWidget(m_modeInfoWidget);
    auto *stopModeLayout = new QHBoxLayout(stopModeWidget);
    auto *stopModeText = new ElaText("停止模式：", m_modeInfoWidget);
    stopModeText->setTextPixelSize(15);
    m_modeCheckBox = new ElaCheckBox(tr("按键停止"), m_modeInfoWidget);
    stopModeLayout->addWidget(stopModeText);
    stopModeLayout->addWidget(m_modeCheckBox);
    stopModeLayout->addStretch();

    contentLayout->addWidget(startModeWidget);
    contentLayout->addWidget(modeTimeWidget);
    contentLayout->addWidget(stopModeWidget);

    auto *modeInfoLayout = new QVBoxLayout(m_modeInfoWidget);
    modeInfoLayout->setContentsMargins(0, 0, 0, 0);
    modeInfoLayout->addWidget(modeInfoText);
    modeInfoLayout->addWidget(modeInfoContent);

    modeInfoContent->setFixedHeight(130);
    connect(m_modeRadioTimeMode, &ElaRadioButton::toggled, this, [=](bool checked) {
        modeTimeWidget->setVisible(checked);
        if (checked) {
            modeInfoContent->setFixedHeight(190);
        } else {
            modeInfoContent->setFixedHeight(130);
        }
        m_homePage->update();
    });

    modeInfoText->setVisible(false);
}

void MainWindows::onDIDSelectChanged(int index) const {
    const QString path = m_deviceComboBox->currentText();
    if (path.isEmpty()) {
        return;
    }

    m_hidController->openDevice(path);
}

void MainWindows::updateDeviceInfo() const {
    const hid_device_info *pInfo = m_hidController->getDeviceInfo();
    if (pInfo != nullptr) {
        m_deviceComboBox->setCurrentText(pInfo->path);
        m_deviceManufacturerEditLine->setText(QString::fromWCharArray(pInfo->manufacturer_string));
        m_deviceProductEditLine->setText(QString::fromWCharArray(pInfo->product_string));
        m_deviceIdEditLine->setText("0x" + QString::number(pInfo->product_id, 16).toUpper());
        m_deviceTypeEditLine->setText("0x" + QString::number(pInfo->usage, 16).toUpper());
        m_deviceVersionEditLine->setText("0x" + QString::number(pInfo->release_number, 16).toUpper());
        m_hidVidLineEdit->setText("0x" + QString::number(pInfo->vendor_id, 16).toUpper());
        m_hidPidLineEdit->setText("0x" + QString::number(pInfo->product_id, 16).toUpper());
    }
}

void MainWindows::updateDeviceList() {
    hid_device_info *hid_info = hid_enumerate(0x0, 0x0);
    QList<HiDDeviceInfo> hidDeviceInfoList;
    for (const hid_device_info *info = hid_info; info != nullptr; info = info->next) {
        QString product = QString::fromWCharArray(info->manufacturer_string);
        if (product.isEmpty() || !product.contains(PRODUCT)) {
            continue;
        }

        HiDDeviceInfo deviceInfo;
        deviceInfo.vid = info->vendor_id;
        deviceInfo.pid = info->product_id;
        deviceInfo.manufacturer = QString::fromWCharArray(info->manufacturer_string);
        deviceInfo.product = QString::fromWCharArray(info->product_string);
        deviceInfo.path = info->path;
        hidDeviceInfoList.append(deviceInfo);
    }
    hid_free_enumeration(hid_info);

    for (const auto &info: hidDeviceInfoList) {
        if (!m_hidDeviceInfoList.contains(info)) {
            m_deviceComboBox->addItem(info.path);
            if (m_logPage) {
                m_logPage->appendLog(tr("设备已连接：") + info.manufacturer + " " + info.product + " VID:0x" +
                                     QString::number(info.vid, 16).toUpper() + " PID:0x" +
                                     QString::number(info.pid, 16).toUpper());
            }
            if (m_hidFilterPage) {
                m_hidFilterPage->addHIDItem(info.manufacturer, info.product, QString::number(info.vid, 16).toUpper(),
                                            QString::number(info.pid, 16).toUpper());
            }
        }
    }

    for (const auto &info: m_hidDeviceInfoList) {
        if (!hidDeviceInfoList.contains(info)) {
            m_deviceComboBox->removeItem(m_deviceComboBox->findText(info.path));
            if (m_logPage) {
                m_logPage->appendLog(tr("设备已断开：") + info.manufacturer + " " + info.product + " VID:0x" +
                                     QString::number(info.vid, 16).toUpper() + " PID:0x" +
                                     QString::number(info.pid, 16).toUpper());
            }
            if (m_hidFilterPage) {
                m_hidFilterPage->removeHIDItem(info.manufacturer, info.product, QString::number(info.vid, 16).toUpper(),
                                               QString::number(info.pid, 16).toUpper());
            }
        }
    }

    m_hidDeviceInfoList = hidDeviceInfoList;
}

void MainWindows::onDeviceStatus(const int status) {
    switch (status) {
        case HIDDef::Device_Opened:
            updateDeviceInfo();
            m_hidController->sendCommand(HIDDef::Update_Device_Value, QByteArray(), true, 50);
            break;
        case HIDDef::Device_Error:
            break;
        case HIDDef::Device_Closed:
            break;
        default:
            break;
    }
}


#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

bool MainWindows::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
#else

bool MainWindows::nativeEvent(const QByteArray &eventType, void *message, long *result) {
#endif

    Q_UNUSED(eventType)
    MSG *msg = reinterpret_cast<MSG *>(message);
    switch (msg->message) {
        ///////////////////////////这个是设备变化的消息////////////////////////////
        case WM_DEVICECHANGE: {
            if (msg->wParam == DBT_DEVICEARRIVAL || msg->wParam == DBT_DEVICEREMOVECOMPLETE) {
                auto lpdb = (PDEV_BROADCAST_HDR) msg->lParam;
                auto lpdbv = (PDEV_BROADCAST_DEVICEINTERFACE) lpdb;
                if (lpdbv->dbcc_classguid == GUID_DEVINTERFACE_HID) {
                    updateDeviceList();
                    update();
                }
            }
            break;
        }
    }

    return ElaWindow::nativeEvent(eventType, message, result);
}

#endif


void MainWindows::onSetTempType() const {
    qDebug() << "MainWindows--->" << __func__ << m_paramTypeComboBox->currentIndex();
    uint8_t tempType = m_paramTypeComboBox->currentIndex();
    QByteArray data;
    data.append(static_cast<char>(tempType));
    m_hidController->sendCommand(HIDDef::Setting_Temp_Type, data, false, 50);

//    m_hidController->sendCommand(HIDDef::Query_Device_Info, nullptr, false, 50);
}

void MainWindows::onSetThreshold() const {
    qDebug() << "MainWindows--->" << __func__ << m_paramComboBox->currentIndex() << m_paramThresholdLowSpinBox->value()
             << m_paramThresholdHighSpinBox->value();
    QByteArray data;
    uint8_t valueId = m_paramComboBox->currentIndex();
    auto low = (float) m_paramThresholdLowSpinBox->value();
    auto high = (float) m_paramThresholdHighSpinBox->value();
    char arr[4];
    memcpy(arr, &high, sizeof(high));
    data.append(arr[0]);
    data.append(arr[1]);
    data.append(arr[2]);
    data.append(arr[3]);
    memcpy(arr, &low, sizeof(low));
    data.append(arr[0]);
    data.append(arr[1]);
    data.append(arr[2]);
    data.append(arr[3]);
    data.append(static_cast<char>(valueId));
    m_hidController->sendCommand(HIDDef::Setting_Value_Threshold, data, false, 50);

}

void MainWindows::onSetLogInterval() const {
    qDebug() << "MainWindows--->" << __func__ << m_paramUpdateIntervalSpinBox->value();
    uint16_t interval = m_paramUpdateIntervalSpinBox->value();
    uint8_t hh = interval / 60;
    uint8_t mm = interval % 60;
    uint8_t tt = 28800 * mm / 60 * 24;

    hh = hh % 10 + (hh % 100 / 10) * 16 + (hh / 100) * 16 * 16;
    mm = mm % 10 + (mm % 100 / 10) * 16 + (mm / 100) * 16 * 16;
    tt = tt % 10 + (tt % 100 / 10) * 16 + (tt / 100) * 16 * 16;

    QByteArray data;
    data.append(static_cast<char>(hh));
    data.append(static_cast<char>(mm));
    data.append(static_cast<char>(tt));
    m_hidController->sendCommand(HIDDef::Setting_Log_Interval, data, false, 50);
}

void MainWindows::onSetStartDelay() const {

}

void MainWindows::onSetDeviceTime() const {
     qDebug() << "MainWindows--->" << __func__ << m_deviceTimeCalendarPicker->getSelectedDate()
              << m_deviceTimeHourComboBox->currentIndex() << m_deviceTimeMinuteComboBox->currentIndex()
              << m_deviceTimeSecondComboBox->currentIndex() << m_timeZoneHourComboBox->currentIndex()
              << m_timeZoneMinuteComboBox->currentIndex();
    //获取时间 默认为UTC时间
    QDate date = m_deviceTimeCalendarPicker->getSelectedDate();

    uint8_t dateHH = m_deviceTimeHourComboBox->currentIndex();
    uint8_t dateMM = m_deviceTimeMinuteComboBox->currentIndex();
    uint8_t dateSS = m_deviceTimeSecondComboBox->currentIndex();

    QDateTime dateTime = QDateTime(date, QTime(dateHH, dateMM, dateSS), Qt::UTC);
    dateTime = dateTime.addSecs(8 * 3600);

    uint8_t yy = dateTime.date().year() - 2000;
    uint8_t MM = dateTime.date().month();
    uint8_t dd = dateTime.date().day();
    uint8_t hh = dateTime.time().hour();
    uint8_t mm = dateTime.time().minute();
    uint8_t ss = dateTime.time().second();
    uint8_t week = dateTime.date().dayOfWeek();

    int zoneHH = m_timeZoneHourComboBox->currentIndex() - 12;
    int zoneMM = MM + m_timeZoneMinuteComboBox->currentIndex();

    if (zoneMM > 60) {
        zoneHH += 1;
        zoneMM -= 60;
    }

    //正负
    uint8_t sign = 0;
    if (zoneHH < 0) {
        sign = 1;
        zoneHH = -zoneHH;
    }


    hh = hh % 10 + (hh % 100 / 10) * 16;
    mm = mm % 10 + (mm % 100 / 10) * 16;
    ss = ss % 10 + (ss % 100 / 10) * 16;
    yy = yy % 10 + (yy % 100 / 10) * 16;
    MM = MM % 10 + (MM % 100 / 10) * 16;
    dd = dd % 10 + (dd % 100 / 10) * 16;
    week = week % 10 + (week % 100 / 10) * 16;
    zoneHH = zoneHH % 10 + (zoneHH % 100 / 10) * 16;
    zoneMM = zoneMM % 10 + (zoneMM % 100 / 10) * 16;


    QByteArray data;
    data.append(static_cast<char>(yy));
    data.append(static_cast<char>(MM));
    data.append(static_cast<char>(dd));
    data.append(static_cast<char>(hh));
    data.append(static_cast<char>(mm));
    data.append(static_cast<char>(ss));
    data.append(static_cast<char>(week));
    data.append(static_cast<char>(sign));
    data.append(static_cast<char>(zoneHH));
    data.append(static_cast<char>(zoneMM));
    m_hidController->sendCommand(HIDDef::Setting_Device_Time, data, false, 50);
}

void MainWindows::onSetMode() const {
    qDebug() << "MainWindows--->" << __func__ << m_modeRadioButtonMode->isChecked();
    uint8_t mode = m_modeRadioButtonMode->isChecked() ? 0 : 1;
    QByteArray data;
    if (mode == 0){
        QDate date = m_modeCalendarPicker->getSelectedDate();

        uint8_t yy = date.year() - 2000;
        uint8_t MM = date.month();
        uint8_t dd = date.day();
        uint8_t hh = m_modeHourComboBox->currentIndex();
        uint8_t mm = m_modeMinuteComboBox->currentIndex();
        uint8_t ss = m_modeSecondComboBox->currentIndex();

        hh = hh % 10 + (hh % 100 / 10) * 16;
        mm = mm % 10 + (mm % 100 / 10) * 16;
        ss = ss % 10 + (ss % 100 / 10) * 16;
        yy = yy % 10 + (yy % 100 / 10) * 16;
        MM = MM % 10 + (MM % 100 / 10) * 16;
        dd = dd % 10 + (dd % 100 / 10) * 16;

        data.append(static_cast<char>(yy));
        data.append(static_cast<char>(MM));
        data.append(static_cast<char>(dd));
        data.append(static_cast<char>(hh));
        data.append(static_cast<char>(mm));
        data.append(static_cast<char>(ss));
    }

    data.append(static_cast<char>(mode));
    m_hidController->sendCommand(HIDDef::Setting_Start_Mode, data, false, 50);
}

void MainWindows::onStopMode() const {
    qDebug() << "MainWindows--->" << __func__ << m_modeCheckBox->isChecked();
    uint8_t mode = m_modeCheckBox->isChecked() ? 1 : 0;
    QByteArray data;
    data.append(static_cast<char>(mode));
    m_hidController->sendCommand(HIDDef::Setting_Stop_Mode, data, false, 50);
}


