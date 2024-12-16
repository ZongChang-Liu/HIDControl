//
// Created by liu_zongchang on 2024/12/16 13:27.
// Email 1439797751@qq.com
//
//

#include "CRC16.h"

unsigned short CRC16::calculate(const unsigned char *date, const unsigned short len) {
    unsigned short crc = 0xFFFF;
    for (unsigned short i = 0; i < len; i++) {
        crc ^= date[i];
        for (unsigned char j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = crc >> 1 ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool CRC16::check(const unsigned char *str, const unsigned short len) {
    const unsigned short crc = calculate(str, len - 2);
    return crc == (str[len - 2] << 8 | str[len - 1]);
}
