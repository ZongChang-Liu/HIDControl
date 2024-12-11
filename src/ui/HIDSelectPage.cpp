//
// Created by liu_zongchang on 2024/12/11 23:54.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QPainter>
#include "HIDSelectPage.h"
#include "ElaTheme.h"
#include "ElaText.h"

HIDSelectPage::HIDSelectPage(QWidget *parent) : QWidget(parent) {
    this->setMinimumSize(200, 100);
    auto* centerVLayout = new QVBoxLayout(this);
    this->setLayout(centerVLayout);
    this->layout()->setContentsMargins(5, 15, 5, 5);
    this->layout()->setSpacing(10);
    this->layout()->setAlignment(Qt::AlignTop);

    auto *vidWidget = new QWidget(this);
    vidWidget->setLayout(new QHBoxLayout());
    vidWidget->layout()->setAlignment(Qt::AlignCenter);
    vidWidget->layout()->setContentsMargins(5, 5, 5, 5);
    vidWidget->layout()->setSpacing(5);
    auto *vidText = new ElaText("VID:", this);
    vidText->setTextPixelSize(15);
    vidWidget->layout()->addWidget(vidText);

    m_hidVidLineEdit = new ElaLineEdit(this);
    m_hidVidLineEdit->setFixedHeight(30);
    m_hidVidLineEdit->setPlaceholderText("VID");
    vidWidget->layout()->addWidget(m_hidVidLineEdit);

    auto *pidWidget = new QWidget(this);
    pidWidget->setLayout(new QHBoxLayout());
    pidWidget->layout()->setAlignment(Qt::AlignCenter);
    pidWidget->layout()->setContentsMargins(5, 5, 5, 5);
    pidWidget->layout()->setSpacing(5);
    auto *pidText = new ElaText("PID:", this);
    pidText->setTextPixelSize(15);
    pidWidget->layout()->addWidget(pidText);

    m_hidPidLineEdit = new ElaLineEdit(this);
    m_hidPidLineEdit->setFixedHeight(30);
    m_hidPidLineEdit->setPlaceholderText("PID");
    pidWidget->layout()->addWidget(m_hidPidLineEdit);


    m_hidSelectComboBox = new ElaComboBox(this);

    centerVLayout->addWidget(vidWidget);
    centerVLayout->addWidget(pidWidget);
    centerVLayout->addWidget(m_hidSelectComboBox);
    centerVLayout->addStretch();
}

HIDSelectPage::~HIDSelectPage() = default;
