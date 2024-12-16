//
// Created by liu_zongchang on 2024/12/16 0:37.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_TREE_ITEM_H
#define HID_CONTROL_TREE_ITEM_H


#include <QObject>

class TreeItem : public QObject{
    Q_OBJECT
    Q_PROPERTY(QList<TreeItem*> m_childrenItems READ getChildrenItems WRITE setChildrenItems NOTIFY sigChildrenItemsChanged)
    Q_PROPERTY(bool m_isChecked READ getIsChecked WRITE setIsChecked NOTIFY sigIsCheckedChanged)
    Q_PROPERTY(TreeItem* m_parentItem READ getParentItem WRITE setParentItem NOTIFY sigParentItemChanged)
public:
    explicit TreeItem(const QString& itemTitle, TreeItem* parent = nullptr);
    ~TreeItem() override;

    QList<TreeItem*> getChildrenItems() const;
    void setChildrenItems(const QList<TreeItem*>& childrenItems);
    Q_SIGNAL void sigChildrenItemsChanged();

    bool getIsChecked() const;
    void setIsChecked(bool isChecked);
    Q_SIGNAL void sigIsCheckedChanged();

    TreeItem* getParentItem() const;
    void setParentItem(TreeItem* parentItem);
    Q_SIGNAL void sigParentItemChanged();

    QString getItemKey() const;
    QString getItemTitle() const;

    void setChildChecked(bool isChecked);
    Qt::CheckState getChildCheckState();

    void appendChildItem(TreeItem* childItem);
    void removeChildItem(TreeItem* childItem);

    bool getIsHasChild() const;
    int getRow() const;
private:
    QString m_itemKey = "";
    QString m_itemTitle = "";
    QList<TreeItem*> m_childrenItems;
    bool m_isChecked{false};
    TreeItem* m_parentItem{nullptr};
    bool m_isExpanded{false};
};


#endif //HID_CONTROL_TREE_ITEM_H
