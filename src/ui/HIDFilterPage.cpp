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
    TreeItem *rootItem = m_hidModel->getRootItem();
    if (m_hidModel->rowCount() == 0) {
        auto *manufacturerItem = new TreeItem(manufacturer, rootItem);
        rootItem->appendChildItem(manufacturerItem);

        auto *productItem = new TreeItem(product, manufacturerItem);
        manufacturerItem->appendChildItem(productItem);

        QString vidPid = "VID:0x" + vid + " PID:0x" + pid;
        auto *vidPidItem = new TreeItem(vidPid, productItem);
        productItem->appendChildItem(vidPidItem);
        return;
    }

    bool isExist = false;
    TreeItem *manufacturerItem = nullptr;
    for (int i = 0; i < rootItem->getChildrenItems().count(); i++) {
        manufacturerItem = rootItem->getChildrenItems().at(i);
        if (manufacturerItem->getItemTitle() == manufacturer) {
            isExist = true;
            break;
        }
    }
    if (!isExist) {
        manufacturerItem = new TreeItem(manufacturer, rootItem);
        rootItem->appendChildItem(manufacturerItem);
    }

    bool isProductExist = false;
    TreeItem *productItem = nullptr;
    for (int i = 0; i < manufacturerItem->getChildrenItems().count(); i++) {
        productItem = manufacturerItem->getChildrenItems().at(i);
        if (productItem->getItemTitle() == product) {
            isProductExist = true;
            break;
        }
    }

    if (!isProductExist) {
        productItem = new TreeItem(product, manufacturerItem);
        manufacturerItem->appendChildItem(productItem);
    }

    QString vidPid = "VID:0x" + vid + " PID:0x" + pid;
    for (int i = 0; i < productItem->getChildrenItems().count(); i++) {
        if (productItem->getChildrenItems().at(i)->getItemTitle() == vidPid) {
            return;
        }
    }

    auto *vidPidItem = new TreeItem(vidPid, productItem);
    productItem->appendChildItem(vidPidItem);
}

void HIDFilterPage::removeHIDItem(const QString &manufacturer, const QString &product, const QString &vid,
                                  const QString &pid) {
    TreeItem *rootItem = m_hidModel->getRootItem();
    TreeItem *manufacturerItem = nullptr;
    TreeItem *productItem = nullptr;
    TreeItem *vidPidItem = nullptr;
    for (int i = 0; i < rootItem->getChildrenItems().count(); i++) {
        manufacturerItem = rootItem->getChildrenItems().at(i);
        if (manufacturerItem->getItemTitle() == manufacturer) {
            for (int j = 0; j < manufacturerItem->getChildrenItems().count(); j++) {
                productItem = manufacturerItem->getChildrenItems().at(j);
                if (productItem->getItemTitle() == product) {
                    for (int k = 0; k < productItem->getChildrenItems().count(); k++) {
                        vidPidItem = productItem->getChildrenItems().at(k);
                        if (vidPidItem->getItemTitle() == "VID:0x" + vid + " PID:0x" + pid) {
                            productItem->removeChildItem(vidPidItem);
                            if (productItem->getChildrenItems().count() == 0) {
                                manufacturerItem->removeChildItem(productItem);
                                if (manufacturerItem->getChildrenItems().count() == 0) {
                                    rootItem->removeChildItem(manufacturerItem);
                                }
                            }
                            return;
                        }
                    }
                }
            }
        }
    }

}