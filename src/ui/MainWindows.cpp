//
// Created by liu_zongchang on 2024/12/11 21:24.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QTimer>
#include <QTimer>
#include <QTimeZone>
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

#endif

MainWindows::MainWindows(QWidget* parent) : ElaWindow(parent)
{
    m_hidController = new HIDController(this);
    initSystem();
    initWindow();
    initEdgeLayout();
    initContent();

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, [=]
    {
        const QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        if (!m_currentDevicePath.isEmpty())
        {
            m_deviceTime = m_deviceTime.addSecs(1);
            m_deviceTimeLabel->setText(m_deviceTime.toString("yyyy-MM-dd hh:mm:ss"));
        }
        m_statusText->setText(time);
    });

    connect(m_hidController, &HIDController::sigAddDevice, this, &MainWindows::onAddDevice);
    connect(m_hidController, &HIDController::sigRemoveDevice, this, &MainWindows::onRemoveDevice);
    connect(m_hidController, &HIDController::sigDeviceStatus, this, &MainWindows::onDeviceStatus);
    connect(m_hidController, &HIDController::sigReceiveCommand, this, &MainWindows::onParseCommand);
    connect(m_hidFilterPage, &HIDFilterPage::sigHIDSelected, this, &MainWindows::onDeviceSelected);
    connect(m_temperatureScaleComboBox, &ElaComboBox::currentTextChanged, this, &MainWindows::onSetTempType);
    connect(m_deviceZoneHourComboBox, &ElaComboBox::currentTextChanged, this, &MainWindows::onSetDeviceTime);
    connect(m_deviceZoneMinuteComboBox, &ElaComboBox::currentTextChanged, this, &MainWindows::onSetDeviceTime);
    connect(m_logIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSetLogInterval()));

    m_hidController->onUpdateDeviceList();
}

MainWindows::~MainWindows() = default;


void MainWindows::initSystem() const
{
#ifdef Q_OS_WIN
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(), &NotificationFilter,
                                                       DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hDevNotify == nullptr)
    {
        qDebug() << "HID Register Failed";
    }
    else
    {
        qDebug() << "HID Register Success";
    }
#endif
}


void MainWindows::initWindow()
{
    this->setUserInfoCardPixmap(QPixmap(":/resources/logo.png"));
    this->setUserInfoCardTitle("Recorder config");
    this->setUserInfoCardSubTitle("1439797751@QQ.com");
    this->setWindowTitle("Recorder Config v" + QString(VERSION));
    this->setWindowIcon(QIcon(":/resources/logo.png"));
    this->setMinimumSize(800, 700);
    this->setIsStayTop(false);
    this->setIsNavigationBarEnable(false);
    this->setIsCentralStackedWidgetTransparent(false);
    this->setWindowButtonFlags(
        ElaAppBarType::MinimizeButtonHint | ElaAppBarType::MaximizeButtonHint | ElaAppBarType::CloseButtonHint);
    this->moveToCenter();
}

void MainWindows::initEdgeLayout()
{
    auto* menuBar = new ElaMenuBar(this);
    m_docketMenu = new ElaMenu(tr("Windows"), this);
    // menuBar->addMenu(m_docketMenu);
    menuBar->setFixedHeight(30);
    auto* customWidget = new QWidget(this);
    auto* customLayout = new QVBoxLayout(customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(menuBar);
    customLayout->addStretch();
    this->setCustomWidget(ElaAppBarType::LeftArea, customWidget);


    // menuBar->addAction(tr("读取设备信息"));
    connect(menuBar, &ElaMenuBar::triggered, this, &MainWindows::onActionTriggered);

    auto* statusBar = new ElaStatusBar(this);
    const QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_statusText = new ElaText(time, this);
    m_statusText->setFixedWidth(180);
    m_statusText->setTextPixelSize(14);
    statusBar->addWidget(m_statusText);
    this->setStatusBar(statusBar);
}

void MainWindows::initContent()
{
    m_contentWidget = new QWidget(this);
    m_hidFilterPage = new HIDFilterPage(this);

    initDeviceInfoUi();
    initAlarmSettingUi();
    initStartConfigUi();

    auto* centerLayout = new QHBoxLayout(m_contentWidget);
    centerLayout->addWidget(m_hidFilterPage);

    auto* mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    centerLayout->addLayout(mainLayout);

    mainLayout->addWidget(m_deviceInfoContent);
    mainLayout->addWidget(m_AlarmSettingContent);
    mainLayout->addWidget(m_startConfigContent);
    mainLayout->addStretch();

    // m_homePage = new ElaScrollPage(this);
    // m_homePage->setTitleVisible(false);
    // m_homePage->addCentralWidget(m_contentWidget, true, true, 0);

    addPageNode(tr("Device Setting"), m_contentWidget, ElaIconType::GearComplex);
}

void MainWindows::createDockWidget(const QString& title, QWidget* widget, Qt::DockWidgetArea area)
{
    auto* dockWidget = new ElaDockWidget(title, this);
    dockWidget->setObjectName(title);
    widget->setParent(dockWidget);
    dockWidget->setDockWidgetTitleIconVisible(false);
    dockWidget->setWidget(widget);
    this->addDockWidget(area, dockWidget);
    this->resizeDocks({dockWidget}, {200}, Qt::Horizontal);

    auto* action = dockWidget->toggleViewAction();
    action->setText(title);
    action->setCheckable(true);
    action->setChecked(true);
    m_docketMenu->addAction(action);
}

void MainWindows::clearInfo()
{
    //关闭全部信号
    m_currentDevicePath.clear();
    m_deviceTime = QDateTime();
    m_deviceTimeLabel->clear();
    m_modelId->clear();
    m_deviceId->clear();
    m_version->clear();
    m_temperatureScaleComboBox->setCurrentIndex(0);
    m_deviceZoneHourComboBox->setCurrentIndex(12);
    m_deviceZoneMinuteComboBox->setCurrentIndex(0);
    m_logIntervalSpinBox->setValue(1);
    m_highTempSpinBox->setValue(100);
    m_lowTempSpinBox->setValue(0);
}

void MainWindows::onActionTriggered(const QAction* action) const
{
}

void MainWindows::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    m_timer->start();
}

void MainWindows::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    m_timer->stop();
}

void MainWindows::initDeviceInfoUi()
{
    m_deviceInfoContent = new ElaScrollPageArea(m_contentWidget);
    m_deviceInfoContent->setContentsMargins(0, 0, 0, 0);

    auto* contentLayout = new QVBoxLayout(m_deviceInfoContent);

    QFont font;
    font.setBold(false);
    font.setPixelSize(14);

    font.setPixelSize(15);
    font.setBold(true);
    auto* deviceInfoText = new ElaText(tr("Device Info"), m_deviceInfoContent);
    deviceInfoText->setFont(font);
    deviceInfoText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* deviceInfoWidget = new QWidget(m_deviceInfoContent);
    auto* deviceInfoWidgetLayout = new QGridLayout(deviceInfoWidget);
    deviceInfoWidgetLayout->setContentsMargins(5, 5, 5, 5);
    deviceInfoWidgetLayout->setVerticalSpacing(10);
    deviceInfoWidgetLayout->setHorizontalSpacing(10);

    font.setPixelSize(14);
    font.setBold(false);

    const auto modelIdWidget = new QWidget(m_deviceInfoContent);
    auto* modelIdLayout = new QHBoxLayout(modelIdWidget);
    modelIdLayout->setContentsMargins(0, 0, 0, 0);
    modelIdLayout->setSpacing(5);
    modelIdWidget->setLayout(modelIdLayout);
    const auto m_modelIdText = new ElaText(tr("Model ID:"), m_deviceInfoContent);
    m_modelIdText->setFont(font);
    m_modelIdText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_modelId = new QLabel(tr(""), m_deviceInfoContent);
    m_modelId->setFont(font);
    m_modelId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    modelIdLayout->addWidget(m_modelIdText);
    modelIdLayout->addWidget(m_modelId);

    const auto deviceIdWidget = new QWidget(m_deviceInfoContent);
    auto* deviceIdLayout = new QHBoxLayout(deviceIdWidget);
    deviceIdLayout->setContentsMargins(0, 0, 0, 0);
    deviceIdLayout->setSpacing(5);
    deviceIdWidget->setLayout(deviceIdLayout);
    const auto m_deviceIdText = new ElaText(tr("Device ID:"), m_deviceInfoContent);
    m_deviceIdText->setFont(font);
    m_deviceIdText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_deviceId = new QLabel(tr(""), m_deviceInfoContent);
    m_deviceId->setFont(font);
    m_deviceId->setTextInteractionFlags(Qt::TextSelectableByMouse);
    deviceIdLayout->addWidget(m_deviceIdText);
    deviceIdLayout->addWidget(m_deviceId);

    const auto versionWidget = new QWidget(m_deviceInfoContent);
    auto* versionLayout = new QHBoxLayout(versionWidget);
    versionLayout->setContentsMargins(0, 0, 0, 0);
    versionLayout->setSpacing(5);
    versionWidget->setLayout(versionLayout);
    const auto m_versionText = new ElaText(tr("Version:"), m_deviceInfoContent);
    m_versionText->setFont(font);
    m_versionText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_version = new QLabel(tr(""), m_deviceInfoContent);
    m_version->setFont(font);
    m_version->setTextInteractionFlags(Qt::TextSelectableByMouse);
    versionLayout->addWidget(m_versionText);
    versionLayout->addWidget( m_version);


    const auto deviceZoneWidget = new QWidget(m_deviceInfoContent);
    auto* deviceZoneLayout = new QHBoxLayout(deviceZoneWidget);
    deviceZoneLayout->setContentsMargins(0, 0, 0, 0);
    deviceZoneLayout->setSpacing(5);
    const auto deviceZone = new ElaText(tr("Device Zone:"), m_deviceInfoContent);
    deviceZone->setFont(font);
    deviceZone->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_deviceZoneHourComboBox = new ElaComboBox(m_deviceInfoContent);
    m_deviceZoneHourComboBox->setObjectName("deviceZoneHourComboBox");
    m_deviceZoneHourComboBox->addItems({
        "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1",
        "UTC", "+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9", "+10", "+11", "+12"
    });
    //获取当前时区
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    int offset = timeZone.offsetFromUtc(QDateTime::currentDateTime());
    m_deviceZoneHourComboBox->setCurrentIndex(offset / 3600 + 12);
    m_deviceZoneMinuteComboBox = new ElaComboBox(m_deviceInfoContent);
    m_deviceZoneMinuteComboBox->setObjectName("deviceZoneMinuteComboBox");
    m_deviceZoneMinuteComboBox->addItems({"00", "30", "60"});
    deviceZoneLayout->addWidget(deviceZone);
    deviceZoneLayout->addWidget(m_deviceZoneHourComboBox);
    const auto deviceZoneSplitText = new ElaText(tr(":"), m_deviceInfoContent);
    deviceZoneSplitText->setFont(font);
    deviceZoneSplitText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    deviceZoneLayout->addWidget(deviceZoneSplitText);
    deviceZoneLayout->addWidget(m_deviceZoneMinuteComboBox);


    const auto deviceTimeWidget = new QWidget(m_deviceInfoContent);
    auto* deviceTimeLayout = new QHBoxLayout(deviceTimeWidget);
    deviceTimeLayout->setContentsMargins(0, 0, 0, 0);
    deviceTimeLayout->setSpacing(5);
    const auto deviceTimeText = new ElaText(tr("Device Time: "), m_deviceInfoContent);
    deviceTimeText->setFont(font);
    deviceTimeText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_deviceTimeLabel = new QLabel(tr(""), m_deviceInfoContent);
    m_deviceTimeLabel->setFont(font);
    m_deviceTimeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_deviceTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    deviceTimeLayout->addWidget(deviceTimeText);
    deviceTimeLayout->addWidget(m_deviceTimeLabel);

    const auto temperatureScaleWidget = new QWidget(m_deviceInfoContent);
    auto* temperatureScaleLayout = new QHBoxLayout(temperatureScaleWidget);
    temperatureScaleLayout->setContentsMargins(0, 0, 0, 0);
    temperatureScaleLayout->setSpacing(5);
    const auto temperatureScale = new ElaText(tr("TemperatureScale:"), m_deviceInfoContent);
    temperatureScale->setFont(font);
    temperatureScale->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_temperatureScaleComboBox = new ElaComboBox(m_deviceInfoContent);
    m_temperatureScaleComboBox->setObjectName("temperatureScaleComboBox");
    m_temperatureScaleComboBox->addItems({" ℃ ", " ℉ "});
    temperatureScaleLayout->addWidget(temperatureScale);
    temperatureScaleLayout->addWidget(m_temperatureScaleComboBox);


    const auto logInterValScaleWidget = new QWidget(m_deviceInfoContent);
    auto* logInterValScaleLayout = new QHBoxLayout(logInterValScaleWidget);
    logInterValScaleLayout->setContentsMargins(0, 0, 0, 0);
    logInterValScaleLayout->setSpacing(5);
    const auto logInterVal = new ElaText(tr("Log Interval:"), m_deviceInfoContent);
    logInterVal->setFont(font);
    logInterVal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_logIntervalSpinBox = new ElaSpinBox(m_deviceInfoContent);
    m_logIntervalSpinBox->setObjectName("logInterValComboBox");
    m_logIntervalSpinBox->setMinimum(1);
    m_logIntervalSpinBox->setMaximum(1500);
    m_logIntervalText = new ElaText(tr("Report once every 1 minutes"), m_deviceInfoContent);
    m_logIntervalText->setFont(font);
    logInterValScaleLayout->addWidget(logInterVal);
    logInterValScaleLayout->addWidget(m_logIntervalSpinBox);
    logInterValScaleLayout->addSpacing(10);
    logInterValScaleLayout->addWidget(m_logIntervalText);


    deviceInfoWidgetLayout->addWidget(modelIdWidget, 0, 0, 1, 1);
    deviceInfoWidgetLayout->addWidget(deviceIdWidget, 0, 1, 1, 1);
    deviceInfoWidgetLayout->addWidget(versionWidget, 0, 2, 1, 1);
    deviceInfoWidgetLayout->addWidget(deviceZoneWidget, 1, 0, 1, 1);
    deviceInfoWidgetLayout->addWidget(deviceTimeWidget, 1, 1, 1, 2);
    deviceInfoWidgetLayout->addWidget(temperatureScaleWidget, 2, 0, 1, 1);
    deviceInfoWidgetLayout->addWidget(logInterValScaleWidget, 2, 1, 1, 2);


    auto* line = new QLabel(m_deviceInfoContent);
    line->setFixedHeight(2);
    line->setStyleSheet("background-color: #000000");

    contentLayout->addWidget(deviceInfoText);
    contentLayout->addWidget(line);
    contentLayout->addWidget(deviceInfoWidget);


    m_deviceInfoContent->setFixedHeight(180);
}

void MainWindows::initAlarmSettingUi()
{
    m_AlarmSettingContent = new ElaScrollPageArea(m_contentWidget);
    m_AlarmSettingContent->setContentsMargins(0, 0, 0, 0);

    auto* contentLayout = new QVBoxLayout(m_AlarmSettingContent);

    QFont font;

    font.setBold(true);
    font.setPixelSize(15);
    auto* alarmSettingText = new ElaText(tr("Alarm Setting"), m_AlarmSettingContent);
    alarmSettingText->setFont(font);
    alarmSettingText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* alarmSettingWidget = new QWidget(m_AlarmSettingContent);
    auto* alarmSettingWidgetLayout = new QGridLayout(alarmSettingWidget);
    alarmSettingWidgetLayout->setContentsMargins(5, 5, 5, 5);
    alarmSettingWidgetLayout->setVerticalSpacing(10);
    alarmSettingWidgetLayout->setHorizontalSpacing(10);

    font.setBold(false);
    font.setPixelSize(14);
    const auto alarmLimits = new ElaText(tr("Alarm Limits:"), m_AlarmSettingContent);
    alarmLimits->setFont(font);

    const auto alarmDelay = new ElaText(tr("Alarm Delay:"), m_AlarmSettingContent);
    alarmDelay->setFont(font);


    const auto dotted = new QFrame(m_AlarmSettingContent);
    dotted->setFrameShape(QFrame::HLine);
    dotted->setLineWidth(1);
    dotted->setMidLineWidth(1);
    dotted->setStyleSheet("border-top: 2px dashed rgb(220,220,220);");

    const auto High = new ElaText(tr("High >:"), m_AlarmSettingContent);
    High->setFont(font);

    m_highTempSpinBox = new ElaDoubleSpinBox(m_AlarmSettingContent);
    m_highTempSpinBox->setObjectName("highTempSpinBox");
    m_highTempSpinBox->setDecimals(1);
    m_highTempSpinBox->setMinimum(-1000);
    m_highTempSpinBox->setMaximum(1000);

    m_highDelaySpinBox = new ElaSpinBox(m_AlarmSettingContent);
    m_highDelaySpinBox->setObjectName("highDelaySpinBox");

    const auto Low = new ElaText(tr("Low <:"), m_AlarmSettingContent);
    Low->setFont(font);

    m_lowTempSpinBox = new ElaDoubleSpinBox(m_AlarmSettingContent);
    m_lowTempSpinBox->setObjectName("lowTempSpinBox");
    m_lowTempSpinBox->setDecimals(1);
    m_lowTempSpinBox->setMinimum(-1000);
    m_lowTempSpinBox->setMaximum(1000);

    m_lowDelaySpinBox = new ElaSpinBox(m_AlarmSettingContent);
    m_lowDelaySpinBox->setObjectName("lowDelaySpinBox");


    alarmSettingWidgetLayout->addWidget(alarmLimits, 0, 1, 1, 1, Qt::AlignCenter);
    alarmSettingWidgetLayout->addWidget(alarmDelay, 0, 2, 1, 1, Qt::AlignCenter);
    alarmSettingWidgetLayout->addWidget(dotted, 1, 0, 1, 3);
    alarmSettingWidgetLayout->addWidget(High, 2, 0, 1, 1, Qt::AlignLeft);
    alarmSettingWidgetLayout->addWidget(m_highTempSpinBox, 2, 1, 1, 1, Qt::AlignCenter);
    alarmSettingWidgetLayout->addWidget(m_highDelaySpinBox, 2, 2, 1, 1, Qt::AlignCenter);
    alarmSettingWidgetLayout->addWidget(Low, 3, 0, 1, 1, Qt::AlignLeft);
    alarmSettingWidgetLayout->addWidget(m_lowTempSpinBox, 3, 1, 1, 1, Qt::AlignCenter);
    alarmSettingWidgetLayout->addWidget(m_lowDelaySpinBox, 3, 2, 1, 1, Qt::AlignCenter);


    contentLayout->addWidget(alarmSettingText);
    auto* line = new QLabel(m_AlarmSettingContent);
    line->setFixedHeight(2);
    line->setStyleSheet("background-color: #000000;border: 1px solid #000000");
    contentLayout->addWidget(line);
    contentLayout->addWidget(alarmSettingWidget);

    m_AlarmSettingContent->setFixedHeight(180);
}

void MainWindows::initStartConfigUi()
{
    m_startConfigContent = new ElaScrollPageArea(m_contentWidget);
    m_startConfigContent->setContentsMargins(0, 0, 0, 0);

    auto* contentLayout = new QVBoxLayout(m_startConfigContent);


    QFont font;

    font.setBold(true);
    font.setPixelSize(15);
    auto* startConfigurationText = new ElaText(tr("Start Configuration"), m_startConfigContent);
    startConfigurationText->setFont(font);
    startConfigurationText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* startConfigWidget = new QWidget(m_startConfigContent);
    auto* startConfigWidgetLayout = new QVBoxLayout(startConfigWidget);
    startConfigWidgetLayout->setContentsMargins(5, 5, 5, 5);
    startConfigWidgetLayout->setSpacing(10);

    font.setBold(false);
    font.setPixelSize(14);

    const auto startDelayWidget = new QWidget(m_startConfigContent);
    auto* startDelayLayout = new QHBoxLayout(startDelayWidget);
    startDelayLayout->setContentsMargins(0, 0, 0, 0);
    startDelayLayout->setSpacing(10);
    const auto startDelay = new ElaText(tr("Start Delay:"), m_AlarmSettingContent);
    startDelay->setFont(font);
    startDelay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_startIntervalSpinBox = new ElaSpinBox(m_startConfigContent);
    m_startIntervalSpinBox->setObjectName("startIntervalSpinBox");
    m_startIntervalText = new ElaText(tr("Start after 1 minutes"), m_startConfigContent);
    m_startIntervalText->setFont(font);
    startDelayLayout->addWidget(startDelay);
    startDelayLayout->addWidget(m_startIntervalSpinBox);
    startDelayLayout->addSpacing(10);
    startDelayLayout->addWidget(m_startIntervalText);

    const auto timedStartTimeWidget = new QWidget(m_startConfigContent);
    auto* timedStartTimeLayout = new QHBoxLayout(timedStartTimeWidget);
    timedStartTimeLayout->setContentsMargins(0, 0, 0, 0);
    timedStartTimeLayout->setSpacing(5);
    const auto timedStartTime = new ElaText(tr("Timed Start Time:"), m_AlarmSettingContent);
    timedStartTime->setFont(font);
    timedStartTime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_startTimeCalendarPicker = new ElaCalendarPicker(m_startConfigContent);
    m_startTimeHourComboBox = new ElaComboBox(m_startConfigContent);
    m_startTimeHourComboBox->setObjectName("startTimeHourComboBox");
    for (int i = 0; i < 24; i++)
    {
        m_startTimeHourComboBox->addItem(QString::number(i));
    }
    m_startTimeMinuteComboBox = new ElaComboBox(m_startConfigContent);
    m_startTimeMinuteComboBox->setObjectName("startTimeMinuteComboBox");
    for (int i = 0; i < 60; i++)
    {
        m_startTimeMinuteComboBox->addItem(QString::number(i));
    }
    timedStartTimeLayout->addWidget(timedStartTime);
    timedStartTimeLayout->addWidget(m_startTimeCalendarPicker);
    timedStartTimeLayout->addWidget(m_startTimeHourComboBox);
    const auto deviceTimeHourSplitText = new ElaText(tr(":"), m_startConfigContent);
    deviceTimeHourSplitText->setFont(font);
    deviceTimeHourSplitText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    timedStartTimeLayout->addWidget(deviceTimeHourSplitText);
    timedStartTimeLayout->addWidget(m_startTimeMinuteComboBox);

    startConfigWidgetLayout->addWidget(startDelayWidget);
    startConfigWidgetLayout->addWidget(timedStartTimeWidget);


    contentLayout->addWidget(startConfigurationText);
    auto* line = new QLabel(m_startConfigContent);
    line->setFixedHeight(2);
    line->setStyleSheet("background-color: #000000;border: 1px solid #000000");
    contentLayout->addWidget(line);
    contentLayout->addWidget(startConfigWidget);

    m_startConfigContent->setFixedHeight(180);
}

void MainWindows::onDeviceStatus(const QString& path, const int status) const
{
    switch (status)
    {
    case HIDDef::Device_Opened:
        m_hidController->sendCommand(path, HIDDef::Query_Device_Info, nullptr, 100);
        break;
    case HIDDef::Device_Error:
        break;
    case HIDDef::Device_Closed:
        break;
    default:
        break;
    }
}

void MainWindows::onAddDevice(const QString& path)
{
    qDebug() << "add device" << path;
    m_hidController->openDevice(path);
}

void MainWindows::onRemoveDevice(const QString& path)
{
    qDebug() << "remove device" << path;
    m_hidFilterPage->removeHIDItem(path);
    if (m_currentDevicePath == path)
    {
        clearInfo();
    }
}

void MainWindows::onParseCommand(const QString& path, const HIDDataFrame& data)
{
    qDebug() << "parse command" << path << data;
    if (data.m_code == HIDDef::Query_Device_Info)
    {
        m_currentDevicePath = path;
        QByteArray dateArray = data.m_date;
        qDebug() << "dateArray:" << dateArray.toHex();
        uint8_t status = dateArray[0];
        uint8_t language = dateArray[1];
        const QString modelId = dateArray.mid(2, 15);
        const QString deviceId = dateArray.mid(17, 11);
        const QString version = dateArray.mid(28, 6);

        uint8_t tempUint = dateArray[36];
        uint8_t logMinute = dateArray[37];logMinute = logMinute / 16 * 10 + logMinute % 16;
        uint8_t logHour = dateArray[38];logHour = logHour / 16 * 10 + logHour % 16;
        uint8_t reportPeriod = dateArray[39];reportPeriod = reportPeriod / 16 * 10 + reportPeriod % 16;
        uint8_t reportType = dateArray[40];

        uint8_t DST = dateArray[41];

        uint8_t zoneHour = dateArray[43];zoneHour = zoneHour / 16 * 10 + zoneHour % 16;
        uint8_t zoneMinute = dateArray[44]; zoneMinute = zoneMinute / 16 * 10 + zoneMinute % 16;
        if (dateArray[42] != '+')
        {
            zoneHour = -zoneHour;
        }

        uint8_t deviceTimeYear = dateArray[45]; deviceTimeYear = deviceTimeYear / 16 * 10 + deviceTimeYear % 16;
        uint8_t deviceTimeMonth = dateArray[46]; deviceTimeMonth = deviceTimeMonth / 16 * 10 + deviceTimeMonth % 16;
        uint8_t deviceTimeDay = dateArray[47]; deviceTimeDay = deviceTimeDay / 16 * 10 + deviceTimeDay % 16;
        uint8_t deviceTimeHour = dateArray[48]; deviceTimeHour = deviceTimeHour / 16 * 10 + deviceTimeHour % 16;
        uint8_t deviceTimeMinute = dateArray[49]; deviceTimeMinute = deviceTimeMinute / 16 * 10 + deviceTimeMinute % 16;
        uint8_t deviceTimeSecond = dateArray[50]; deviceTimeSecond = deviceTimeSecond / 16 * 10 + deviceTimeSecond % 16;
        uint8_t deviceTimeWeek = dateArray[51]; deviceTimeWeek = deviceTimeWeek / 16 * 10 + deviceTimeWeek % 16;

        m_deviceTime.setDate(QDate(deviceTimeYear + 2000, deviceTimeMonth, deviceTimeDay));
        m_deviceTime.setTime(QTime(deviceTimeHour, deviceTimeMinute, deviceTimeSecond));

        auto* ValueArray = new uint8_t[4]{};
        ValueArray[0] = dateArray[60];
        ValueArray[1] = dateArray[61];
        ValueArray[2] = dateArray[62];
        ValueArray[3] = dateArray[63];
        float heightTemp = *(float*)ValueArray;
        ValueArray[0] = dateArray[64];
        ValueArray[1] = dateArray[65];
        ValueArray[2] = dateArray[66];
        ValueArray[3] = dateArray[67];
        float lowTemp = *(float*)ValueArray;



        qDebug() << "status:" << status << "language:" << language << "modelId:" << modelId << "deviceId:" << deviceId
            << "version:" << version << "tempUint:" << tempUint << "logHour:" << logHour << "logMinute:" << logMinute
            << "reportPeriod:" << reportPeriod << "reportType:" << reportType
            << "DST:" << DST << "zoneHour:" << zoneHour << "zoneMinute:" << zoneMinute
            << "deviceTime:" << m_deviceTime.toString("yyyy-MM-dd hh:mm:ss")
            << "deviceTimeWeek:" << deviceTimeWeek << "heightTemp:" << heightTemp << "lowTemp:" << lowTemp;

        //关闭信号

        m_hidFilterPage->addHIDItem(path, deviceId);
        m_modelId->setText(modelId);
        m_deviceId->setText(deviceId);
        m_version->setText(version);
        QSignalBlocker m_temperatureScaleComboBoxBlocker(m_temperatureScaleComboBox);
        m_temperatureScaleComboBox->setCurrentIndex(tempUint);
        QSignalBlocker m_deviceZoneHourComboBoxBlocker(m_deviceZoneHourComboBox);
        m_deviceZoneHourComboBox->setCurrentIndex(zoneHour + 12);
        QSignalBlocker m_deviceZoneMinuteComboBoxBlocker(m_deviceZoneMinuteComboBox);
        m_deviceZoneMinuteComboBox->setCurrentIndex(zoneMinute);
        QSignalBlocker m_logIntervalSpinBoxBlocker(m_logIntervalSpinBox);
        m_logIntervalSpinBox->setValue(logHour * 60 + logMinute);
        int interval = 28800 * m_logIntervalSpinBox->value() / 1440;
        m_logIntervalText->setText(tr("Record Period of %1 days").arg(interval));
        m_deviceTimeLabel->setText(m_deviceTime.toString("yyyy-MM-dd hh:mm:ss"));
        QSignalBlocker m_highTempSpinBoxBlocker(m_highTempSpinBox);
        m_highTempSpinBox->setValue(heightTemp);
        QSignalBlocker m_lowTempSpinBoxBlocker(m_lowTempSpinBox);
        m_lowTempSpinBox->setValue(lowTemp);
    }
}

void MainWindows::onDeviceSelected(const bool isSelected, const QString& path)
{
    if (isSelected)
    {
        m_hidController->sendCommand(path, HIDDef::Query_Device_Info, nullptr, 100);
    }
    else if (m_currentDevicePath == path)
    {
        clearInfo();
    }
}


#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

bool MainWindows::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
#else

bool MainWindows::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
#endif

    Q_UNUSED(eventType)
    const MSG* msg = static_cast<MSG*>(message);
    switch (msg->message)
    {
    ///////////////////////////这个是设备变化的消息////////////////////////////
    case WM_DEVICECHANGE:
        {
            if (msg->wParam == DBT_DEVICEARRIVAL || msg->wParam == DBT_DEVICEREMOVECOMPLETE)
            {
                auto lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
                auto lpdbv = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(lpdb);
                if (lpdbv->dbcc_classguid == GUID_DEVINTERFACE_HID)
                {
                    m_hidController->onUpdateDeviceList();
                }
            }
            break;
        }
    }

    return ElaWindow::nativeEvent(eventType, message, result);
}

#endif


void MainWindows::onSetTempType() const
{
    if (m_currentDevicePath.isEmpty())
    {
        return;
    }
    QByteArray data;
    data.append(static_cast<char>(m_temperatureScaleComboBox->currentIndex()));
    m_hidController->sendCommand(m_currentDevicePath, HIDDef::Setting_Temp_Unit, data, 50);
}

void MainWindows::onSetThreshold() const
{
}

void MainWindows::onSetLogInterval() const
{
    const int hour = m_logIntervalSpinBox->value() / 60;
    const int minute = m_logIntervalSpinBox->value() % 60;
    const int interval = 28800 * m_logIntervalSpinBox->value() / 1440;
    m_logIntervalText->setText(tr("Record Period of %1 days").arg(interval));

    if (m_currentDevicePath.isEmpty())
    {
        return;
    }

    uint8_t bcdHour = hour / 10 * 16 + hour % 10;
    uint8_t bcdMinute = minute / 10 * 16 + minute % 10;
    QByteArray data;
    data.append(bcdHour);
    data.append(bcdMinute);
    data.append(interval);
    m_hidController->sendCommand(m_currentDevicePath, HIDDef::Setting_Log_Interval, data, 50);
}

void MainWindows::onSetStartDelay() const
{
}

void MainWindows::onSetDeviceTime()
{
    if (m_currentDevicePath.isEmpty())
    {
        return;
    }
    const int zoneHour = m_deviceZoneHourComboBox->currentIndex() - 12;
    const int zoneMinute = m_deviceZoneMinuteComboBox->currentText().toInt();
    m_deviceTime = QDateTime::currentDateTime().addSecs(zoneHour * 3600 + zoneMinute * 60);
    m_deviceTimeLabel->setText(m_deviceTime.toString("yyyy-MM-dd hh:mm:ss"));
}

void MainWindows::onSetMode() const
{
}

void MainWindows::onStopMode() const
{
}
