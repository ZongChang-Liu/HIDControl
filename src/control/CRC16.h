//
// Created by liu_zongchang on 2024/12/16 13:27.
// Email 1439797751@qq.com
//
//

#ifndef CRC16_H
#define CRC16_H

class CRC16 {
public:
    static unsigned short calculate(const unsigned char *date,unsigned short len);
    static bool check(const unsigned char *str, unsigned short len);
};



#endif //CRC16_H
