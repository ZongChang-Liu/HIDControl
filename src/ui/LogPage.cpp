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
    if (!this->m_logList.isEmpty() && this->m_logList.last() == log) {
        return;
    }
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
    m_logListView = new ElaListView(this);
    m_logListView->setIsTransparent(true);
    m_logModel = new LogModel(this);
    m_logListView->setModel(m_logModel);
    mainLayout->addWidget(m_logListView);
    connect(ElaLog::getInstance(), &ElaLog::logMessage, this, [=](const QString& log) {
        m_logModel->appendLogList(log);
    });
}

void LogPage::appendLog(const QString &log) {
    m_logModel->appendLogList(log);
    m_logListView->scrollToBottom();
}

LogPage::~LogPage() = default;
