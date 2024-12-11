//
// Created by liu_zongchang on 2024/12/12 0:02.
// Email 1439797751@qq.com
//
//

#include <QVBoxLayout>
#include "LogPage.h"
#include "ElaListView.h"
#include "ElaLog.h"



LogModel::LogModel(QObject *parent) : QAbstractListModel(parent) {

}

LogModel::~LogModel() = default;

void LogModel::setLogList(const QStringList &logList) {
    this->beginResetModel();
    this->m_logList = logList;
    this->endResetModel();
}

QStringList LogModel::getLogList() const {
    return this->m_logList;
}

void LogModel::appendLogList(const QString &log) {
    this->beginResetModel();
    this->m_logList.append(log);
    this->endResetModel();
}

void LogModel::clearLogList() {
    this->beginResetModel();
    this->m_logList.clear();
    this->endResetModel();
}

int LogModel::rowCount(const QModelIndex &parent) const {
    return this->m_logList.count();
}

QVariant LogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }
    if (index.row() >= this->m_logList.count()) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return this->m_logList.at(index.row());
    }
    return {};
}


LogPage::LogPage(QWidget *parent) : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 5, 5, 0);
    auto* logView = new ElaListView(this);
    logView->setIsTransparent(true);
    m_logModel = new LogModel(this);
    logView->setModel(m_logModel);
    mainLayout->addWidget(logView);
    connect(ElaLog::getInstance(), &ElaLog::logMessage, this, [=](const QString& log) {
        m_logModel->appendLogList(log);
    });
    m_logModel->appendLogList("测试条例 1");
    m_logModel->appendLogList("测试条例 2");
    m_logModel->appendLogList("测试条例 3");
    m_logModel->appendLogList("测试条例 4");
}

LogPage::~LogPage() = default;
