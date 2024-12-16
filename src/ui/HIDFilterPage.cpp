//
// Created by liu_zongchang on 2024/12/12 21:02.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include <QDebug>
#include "HIDFilterPage.h"
#include "ElaTreeView.h"
#include "HIDModel.h"
#include "TreeItem.h"


HIDFilterPage::HIDFilterPage(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 5, 5, 0);
    mainLayout->setSpacing(0);
    this->setMinimumSize(200, 300);
    m_hidTreeView = new ElaTreeView(this);
    mainLayout->addWidget(m_hidTreeView);
    m_hidTreeView->setHeaderHidden(true);
    m_hidModel = new HIDModel(this);
    m_hidTreeView->setModel(m_hidModel);
}

HIDFilterPage::~HIDFilterPage() = default;


void
HIDFilterPage::addHIDItem(const QString &manufacturer, const QString &product, const QString &vid, const QString &pid) {

}

void HIDFilterPage::removeHIDItem(const QString &manufacturer, const QString &product, const QString &vid,
                                  const QString &pid) {
}