//
// Created by liu_zongchang on 2024/12/11 21:24.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QPropertyAnimation>
#include "MainWindows.h"
#include "LogPage.h"
#include "ElaStatusBar.h"
#include "ElaText.h"
#include "ElaDockWidget.h"
#include "ElaMenuBar.h"
#include "ElaScrollPageArea.h"
#include "ElaTheme.h"

MainWindows::MainWindows(QWidget *parent) : ElaWindow(parent) {
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
    connect(m_hidVidLineEdit, &ElaLineEdit::textChanged, this, [=](const QString &text) {
        if (text.isEmpty()) {
            return;
        }
        if (!text.contains(QRegExp("^0x[0-9A-Fa-f]{4}$"))) {
            return;
        }
        onDIDSelectChanged(m_hidVidLineEdit->text().toInt(), m_hidPidLineEdit->text().toInt());
    });
    connect(m_hidPidLineEdit, &ElaLineEdit::textChanged, this, [=](const QString &text) {
        if (text.isEmpty()) {
            updateDeviceInfo();
            return;
        }
        if (!text.contains(QRegExp("^0x[0-9A-Fa-f]{4}$"))) {
            updateDeviceInfo();
            return;
        }
        onDIDSelectChanged(m_hidVidLineEdit->text().toInt(), m_hidPidLineEdit->text().toInt());
    });
    updateDeviceList();
}

MainWindows::~MainWindows() = default;

void MainWindows::initWindow() {
    this->setUserInfoCardPixmap(QPixmap(":/resources/logo.png"));
    this->setUserInfoCardTitle("HID Control");
    this->setUserInfoCardSubTitle("1439797751@QQ.com");
    this->setWindowTitle("HID Control");
    this->setWindowIcon(QIcon(":/resources/logo.png"));
    this->setMinimumSize(600, 400);
    this->setIsStayTop(true);
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
    connect(menuBar, &ElaMenuBar::triggered, this, &MainWindows::onActionTriggered);

    m_logPage = new LogPage(this);
    createDockWidget(tr("日志"), m_logPage, Qt::RightDockWidgetArea);

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
    initParamInformation();
    initTimeInformation();
    initModeInformation();

    auto *centerLayout = new QVBoxLayout(m_contentWidget);
    centerLayout->addWidget(m_deviceInfoWidget);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(m_paramInfoWidget);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(m_timeInfoWidget);
    centerLayout->addSpacing(20);
    centerLayout->addWidget(m_modeInfoWidget);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 15, 150);
    m_homePage->setTitleVisible(false);
    m_homePage->addCentralWidget(m_contentWidget, true, true, 0);
    addPageNode(tr("设置"), m_homePage, ElaIconType::GearComplex);
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

    auto* devicePathWidget = new QWidget(m_deviceInfoWidget);
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
    auto *devicePidText = new ElaText(tr("PID:"), m_deviceInfoWidget);
    devicePidText->setTextPixelSize(15);
    m_hidPidLineEdit = new ElaLineEdit(m_deviceInfoWidget);
    m_hidPidLineEdit->setFixedHeight(30);
    m_hidPidLineEdit->setFixedWidth(100);

    devicePathLayout->addWidget(devicePathText);
    devicePathLayout->addWidget(m_deviceComboBox);
    devicePathLayout->addSpacing(10);
    devicePathLayout->addWidget(deviceVidText);
    devicePathLayout->addWidget(m_hidVidLineEdit);
    devicePathLayout->addSpacing(5);
    devicePathLayout->addWidget(devicePidText);
    devicePathLayout->addWidget(m_hidPidLineEdit);

    auto* deviceProductWidget = new QWidget(m_deviceInfoWidget);
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
    deviceProductLayout->addSpacing(15);
    deviceProductLayout->addWidget(deviceProductText);
    deviceProductLayout->addWidget(m_deviceProductEditLine);
    deviceProductLayout->addSpacing(15);

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
    deviceIdLayout->addSpacing(10);
    deviceIdLayout->addWidget(deviceTypeText);
    deviceIdLayout->addWidget(m_deviceTypeEditLine);
    deviceIdLayout->addSpacing(10);
    deviceIdLayout->addWidget(deviceVersionText);
    deviceIdLayout->addWidget(m_deviceVersionEditLine);

    contentLayout->addWidget(devicePathWidget);
    contentLayout->addWidget(deviceProductWidget);
    contentLayout->addWidget(deviceIdWidget);

    auto *deviceInfoLayout = new QVBoxLayout(m_deviceInfoWidget);
    deviceInfoLayout->setContentsMargins(0, 0, 0, 0);
    deviceInfoLayout->addWidget(deviceInfoText);
    deviceInfoLayout->addSpacing(10);
    deviceInfoLayout->addWidget(m_deviceInfoContent);

    m_deviceInfoContent->setFixedHeight(200);
}

void MainWindows::initParamInformation() {
    m_paramInfoWidget = new QWidget(m_contentWidget);

    auto *paramTextWidget = new QWidget(m_paramInfoWidget);
    auto *paramTextLayout = new QHBoxLayout(paramTextWidget);
    paramTextLayout->setContentsMargins(0, 0, 0, 0);
    auto *paramInfoText = new ElaText(tr("参数设置"), m_paramInfoWidget);
    m_paramComboBox = new ElaComboBox(this);
    m_paramComboBox->setFixedHeight(paramInfoText->height() + 10);
    m_paramComboBox->addItems({"参数1", "参数2", "参数3", "参数4", "参数5"});
    m_paramComboBox->setMinimumWidth(200);
    paramTextLayout->addWidget(paramInfoText);
    paramTextLayout->addSpacing(15);
    paramTextLayout->addWidget(m_paramComboBox);
    paramTextLayout->addStretch();

    auto *paramInfoContent = new ElaScrollPageArea(m_paramInfoWidget);
    paramInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(paramInfoContent);

    auto *paramTypeWidget = new QWidget(m_paramInfoWidget);
    auto *paramTypeLayout = new QHBoxLayout(paramTypeWidget);
    m_paramText = new ElaText("参数类型：温度", m_paramInfoWidget);
    m_paramText->setTextPixelSize(15);
    auto *paramTypeText = new ElaText("单位：", m_paramInfoWidget);
    paramTypeText->setTextPixelSize(15);
    m_paramTypeComboBox = new ElaComboBox(m_paramInfoWidget);
    m_paramTypeComboBox->addItems({tr("摄氏度 ℃"), tr("华氏度 ℉")});
    paramTypeLayout->addWidget(m_paramText);
    paramTypeLayout->addSpacing(30);
    paramTypeLayout->addWidget(paramTypeText);
    paramTypeLayout->addWidget(m_paramTypeComboBox);
    paramTypeLayout->addStretch();

    auto *paramThresholdWidget = new QWidget(m_paramInfoWidget);
    auto *paramThresholdLayout = new QHBoxLayout(paramThresholdWidget);
    auto *paramThresholdLowText = new ElaText("低阈值：", m_paramInfoWidget);
    paramThresholdLowText->setTextPixelSize(15);
    m_paramThresholdLowEditLine = new ElaLineEdit(m_paramInfoWidget);
    m_paramThresholdLowEditLine->setFixedHeight(30);

    auto *paramThresholdHighText = new ElaText("高阈值：", m_paramInfoWidget);
    paramThresholdHighText->setTextPixelSize(15);
    m_paramThresholdHighEditLine = new ElaLineEdit(m_paramInfoWidget);
    m_paramThresholdHighEditLine->setFixedHeight(30);

    paramThresholdLayout->addWidget(paramThresholdLowText);
    paramThresholdLayout->addWidget(m_paramThresholdLowEditLine);
    paramThresholdLayout->addSpacing(30);
    paramThresholdLayout->addWidget(paramThresholdHighText);
    paramThresholdLayout->addWidget(m_paramThresholdHighEditLine);
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
    m_paramUpdateIntervalSpinBox->setRange(1, 99999);
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
    paramUpdateLayout->addSpacing(10);
    paramUpdateLayout->addWidget(paramUpdateDayText);

    contentLayout->addWidget(paramTypeWidget);
    contentLayout->addWidget(paramThresholdWidget);
    contentLayout->addWidget(paramUpdateWidget);

    auto *paramInfoLayout = new QVBoxLayout(m_paramInfoWidget);
    paramInfoLayout->setContentsMargins(0, 0, 0, 0);
    paramInfoLayout->addWidget(paramTextWidget);
    paramInfoLayout->addSpacing(10);
    paramInfoLayout->addWidget(paramInfoContent);

    paramInfoContent->setFixedHeight(60 * 3);
}

void MainWindows::initTimeInformation() {
    m_timeInfoWidget = new QWidget(m_contentWidget);

    auto *timeInfoText = new ElaText(tr("时间设置"), m_timeInfoWidget);
    auto *m_timeInfoContent = new ElaScrollPageArea(m_timeInfoWidget);
    m_timeInfoContent->setContentsMargins(0, 0, 0, 0);
    auto *contentLayout = new QVBoxLayout(m_timeInfoContent);

    auto *deviceTimeWidget = new QWidget(m_paramInfoWidget);
    auto *deviceTimeLayout = new QHBoxLayout(deviceTimeWidget);
    auto *deviceTimeText = new ElaText("设备时间：", m_timeInfoWidget);
    deviceTimeText->setTextPixelSize(15);
    m_deviceTimeEditLine = new ElaLineEdit(m_timeInfoWidget);
    m_deviceTimeEditLine->setReadOnly(true);
    m_deviceTimeEditLine->setFixedHeight(30);
    deviceTimeLayout->addWidget(deviceTimeText);
    deviceTimeLayout->addWidget(m_deviceTimeEditLine);

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
    auto *timeZoneColon = new ElaText(" : ", m_timeInfoWidget);
    timeZoneColon->setTextPixelSize(15);
    m_timeZoneMinuteComboBox = new ElaComboBox(m_timeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        m_timeZoneMinuteComboBox->addItem(QString::number(i));
    }
    m_timeZoneHourComboBox->setFixedHeight(30);
    timeZoneLayout->addWidget(timeZoneText);
    timeZoneLayout->addWidget(m_timeZoneHourComboBox);
    timeZoneLayout->addWidget(timeZoneColon);
    timeZoneLayout->addWidget(m_timeZoneMinuteComboBox);
    timeZoneLayout->addStretch();


    contentLayout->addWidget(deviceTimeWidget);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(systemTimeWidget);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(timeZoneWidget);

    auto *timeInfoLayout = new QVBoxLayout(m_timeInfoWidget);
    timeInfoLayout->setContentsMargins(0, 0, 0, 0);
    timeInfoLayout->addWidget(timeInfoText);
    timeInfoLayout->addSpacing(10);
    timeInfoLayout->addWidget(m_timeInfoContent);

    m_timeInfoContent->setFixedHeight(65 * 3);
}

void MainWindows::initModeInformation() {
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
    auto *modeTimeHourComboBox = new ElaComboBox(m_modeInfoWidget);
    modeTimeHourComboBox->addItems({"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
                                    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                                    "20", "21", "22", "23"});
    modeTimeHourComboBox->setFixedHeight(30);
    auto *modeTimeMinuteComboBox = new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        modeTimeMinuteComboBox->addItem(QString::number(i));
    }
    modeTimeMinuteComboBox->setFixedHeight(30);
    auto *modeTimeSecondComboBox = new ElaComboBox(m_modeInfoWidget);
    for (int i = 0; i < 60; ++i) {
        modeTimeSecondComboBox->addItem(QString::number(i));
    }
    modeTimeSecondComboBox->setFixedHeight(30);
    auto *modeTimeHourText = new ElaText("时", m_modeInfoWidget);
    modeTimeHourText->setTextPixelSize(15);
    auto *modeTimeMinuteText = new ElaText("分", m_modeInfoWidget);
    modeTimeMinuteText->setTextPixelSize(15);
    auto *modeTimeSecondText = new ElaText("秒", m_modeInfoWidget);
    modeTimeSecondText->setTextPixelSize(15);
    modeTimeLayout->addWidget(modeTimeDayText);
    modeTimeLayout->addWidget(m_modeCalendarPicker);
    modeTimeLayout->addSpacing(14);
    modeTimeLayout->addWidget(modeTimeHourComboBox);
    modeTimeLayout->addWidget(modeTimeHourText);
    modeTimeLayout->addWidget(modeTimeMinuteComboBox);
    modeTimeLayout->addWidget(modeTimeMinuteText);
    modeTimeLayout->addWidget(modeTimeSecondComboBox);
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
    contentLayout->addSpacing(10);
    contentLayout->addWidget(modeTimeWidget);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(stopModeWidget);

    auto *modeInfoLayout = new QVBoxLayout(m_modeInfoWidget);
    modeInfoLayout->setContentsMargins(0, 0, 0, 0);
    modeInfoLayout->addWidget(modeInfoText);
    modeInfoLayout->addSpacing(10);
    modeInfoLayout->addWidget(modeInfoContent);

    modeInfoContent->setFixedHeight(65 * 2);
    connect(m_modeRadioTimeMode, &ElaRadioButton::toggled, this, [=](bool checked) {
        modeTimeWidget->setVisible(checked);
        if (checked) {
            modeInfoContent->setFixedHeight(65 * 3);
        } else {
            modeInfoContent->setFixedHeight(65 * 2);
        }
        m_homePage->update();
    });
}

void MainWindows::onDIDSelectChanged(int index) {
    QString path = m_deviceComboBox->currentText();
    qDebug() << "onDIDSelectChanged" << path;
    if (hidDevice != nullptr) {
        hid_close(hidDevice);
        hidDevice = nullptr;
    }
    hidDevice = hid_open_path(path.toStdString().c_str());
    if (hidDevice == nullptr) {
        qDebug() << "Open HID Device Failed";
        return;
    }
    qDebug() << "Open HID Device Success";
    updateDeviceInfo();
}

void MainWindows::updateDeviceInfo() {
    if (hidDevice == nullptr) {
        return;
    }

    hid_device_info *pInfo = hid_get_device_info(hidDevice);
    m_deviceComboBox->setCurrentText(pInfo->path);
    m_deviceManufacturerEditLine->setText(QString::fromWCharArray(pInfo->manufacturer_string));
    m_deviceProductEditLine->setText(QString::fromWCharArray(pInfo->product_string));
    m_deviceIdEditLine->setText("0x" + QString::number(pInfo->product_id,16).toUpper());
    m_deviceTypeEditLine->setText("0x" + QString::number(pInfo->usage,16).toUpper());
    m_deviceVersionEditLine->setText("0x" + QString::number(pInfo->release_number,16).toUpper());
    m_hidVidLineEdit->setText("0x" + QString::number(pInfo->vendor_id,16).toUpper());
    m_hidPidLineEdit->setText("0x" + QString::number(pInfo->product_id,16).toUpper());
}

void MainWindows::updateDeviceList() {
    m_deviceComboBox->clear();
    hid_device_info *hid_info;
    hid_info = hid_enumerate(0x0, 0x0);
    for (; hid_info != nullptr; hid_info = hid_info->next) {
        m_deviceComboBox->addItem(hid_info->path);
    }
    hid_free_enumeration(hid_info);
}

void MainWindows::onDIDSelectChanged(int vid, int pid) {
    if (hidDevice != nullptr) {
        hid_close(hidDevice);
        hidDevice = nullptr;
    }
    hidDevice = hid_open(vid, pid, nullptr);
    if (hidDevice == nullptr) {
        qDebug() << "Open HID Device Failed";
        return;
    }
    qDebug() << "Open HID Device Success";
    updateDeviceInfo();
}

