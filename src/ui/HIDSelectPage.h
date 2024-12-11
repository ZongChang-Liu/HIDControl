//
// Created by liu_zongchang on 2024/12/11 23:54.
// Email 1439797751@qq.com
//
//

#ifndef HID_CONTROL_HID_SELECT_PAGE_H
#define HID_CONTROL_HID_SELECT_PAGE_H


#include "QWidget.h"
#include "ElaComboBox.h"
#include "Def.h"
#include "ElaLineEdit.h"

class HIDSelectPage : public QWidget{
    Q_OBJECT
public:
    explicit HIDSelectPage(QWidget *parent = nullptr);
    ~HIDSelectPage() override;
private:
    ElaLineEdit* m_hidVidLineEdit{nullptr};
    ElaLineEdit* m_hidPidLineEdit{nullptr};
    ElaComboBox* m_hidSelectComboBox{nullptr};
};


#endif //HID_CONTROL_HID_SELECT_PAGE_H
