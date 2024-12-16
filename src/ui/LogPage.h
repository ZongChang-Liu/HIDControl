
//
// Created by liu_zongchang on 2024/12/12 0:02.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_LOG_PAGE_H
#define HID_CONTROL_LOG_PAGE_H


#include <QAbstractListModel>
#include <QWidget>

class LogModel : public QAbstractListModel{
    Q_OBJECT
public:
    explicit LogModel(QObject* parent = nullptr);
    ~LogModel() override;
    void setLogList(const QStringList& logList);
    QStringList getLogList() const;
    void appendLogList(const QString& log);
    void clearLogList();
protected:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
private:
    QStringList m_logList;
};

class ElaListView;
class LogPage : public QWidget {
    Q_OBJECT
public:
    explicit LogPage(QWidget *parent = nullptr);
    ~LogPage() override;
    void appendLog(const QString& log);
private:
    LogModel* m_logModel{nullptr};
    ElaListView* m_logListView{nullptr};
};


#endif //HID_CONTROL_LOG_PAGE_H
