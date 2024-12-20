//
// Created by liu_zongchang on 2024/12/16 13:19.
// Email 1439797751@qq.com
//
//

#ifndef HID_DATA_FRAME_H
#define HID_DATA_FRAME_H

#include <QByteArray>


class HIDDataFrame {
public:
    explicit HIDDataFrame() = default;
    ~HIDDataFrame() = default;
    uint8_t m_userType{0};
    uint32_t m_userId{0};
    uint16_t m_code{0};
    QByteArray m_date{};
    uint16_t m_length{0};

    static uint8_t* createCommand(HIDDataFrame &frame);

    static bool parseCommand(const uint8_t* data,int length, HIDDataFrame &frame);

    friend QDebug operator<<(QDebug debug, const HIDDataFrame &frame);

private:
    static const uint8_t m_frameHead{0xAA};
    static const uint8_t m_frameTail{0x55};
    uint16_t m_frameNumber{0};
};



#endif //HID_DATA_FRAME_H
