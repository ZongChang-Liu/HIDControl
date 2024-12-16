//
// Created by liu_zongchang on 2024/12/16 0:48.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_HID_MODEL_H
#define HID_CONTROL_HID_MODEL_H


#include <QAbstractItemModel>

class TreeItem;
class HIDModel : public QAbstractItemModel{
    Q_OBJECT
public:
    explicit HIDModel(QObject* parent = nullptr);
    ~HIDModel() override;
    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex index(int row, int column) const;
    int rowCount(const QModelIndex& parent) const override;
    int rowCount() const;
    int columnCount(const QModelIndex& parent) const override;
    int columnCount() const;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant data(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    bool setData(const QModelIndex& index, const QVariant& value);
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation) const;

    int getItemCount() const;
private:
    QMap<QString, TreeItem*> _itemsMap;
    TreeItem* _rootItem{nullptr};
};


#endif //HID_CONTROL_HID_MODEL_H
