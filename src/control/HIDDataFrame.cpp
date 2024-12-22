//
// Created by liu_zongchang on 2024/12/16 13:19.
// Email 1439797751@qq.com
//
//
#include <QDebug>
#include <iostream>
#include "HIDDataFrame.h"

#include "CRC16.h"

void HIDDataFrame::reset()
{
    m_userType = 0;
    m_userId = 0;
    m_code = 0;
    m_date = QByteArray();
}

unsigned char* HIDDataFrame::createCommand(HIDDataFrame& frame)
{
    frame.m_length = sizeof(HIDDataFrame::m_frameHead) + sizeof(frame.m_frameNumber) + sizeof (frame.m_length)+ sizeof(frame.m_userType) + sizeof(frame.m_userId) +
                 sizeof(frame.m_code) + frame.m_date.size() + 2 + sizeof(HIDDataFrame::m_frameTail);
    auto* cmdData = new unsigned char[frame.m_length];
    memset(cmdData, 0, frame.m_length);

    frame.m_frameNumber++;
    if (frame.m_frameNumber > 0xFFFF) {
        frame.m_frameNumber = 0;
    }

    //head 8bit
    cmdData[0] = HIDDataFrame::m_frameHead;

    //frame number 16bit
    cmdData[1] = frame.m_frameNumber;
    cmdData[2] = frame.m_frameNumber >> 8;


    //frame length 16bit
    cmdData[3] = frame.m_length;
    cmdData[4] = frame.m_length >> 8;

    //user type 8bit
    cmdData[5] = frame.m_userType;

    //user id 32bit
    cmdData[6] = frame.m_userId;
    cmdData[7] = frame.m_userId >> 8;
    cmdData[8] = frame.m_userId >> 16;
    cmdData[9] = frame.m_userId >> 24;

    //command code 16bit
    cmdData[10] = frame.m_code;
    cmdData[11] = frame.m_code >> 8;

    if (!frame.m_date.isEmpty()) {
        for (int i = 12; i < frame.m_date.size() + 12; ++i) {
            cmdData[i] = frame.m_date[i - 12];
        }
    }

    //crc 16bit
    uint16_t crc = CRC16::calculate(cmdData, frame.m_length - 3);
    cmdData[frame.m_length - 3] = crc;
    cmdData[frame.m_length - 2] = crc >> 8;

    //tail 8bit
    cmdData[frame.m_length - 1] = HIDDataFrame::m_frameTail;
    //打印数组大小
    qDebug() << "HIDDataFrame----->" << __func__ << frame;
    return cmdData;
}

bool HIDDataFrame::parseCommand(const unsigned char* data, int length, HIDDataFrame& frame)
{
    const QByteArray cmdData((char*)data, length);

    //先检查head和tail
    int indexHead = cmdData.indexOf(static_cast<int8_t>(m_frameHead));
    if (indexHead == -1) {
        return false;
    }

    int indexTail = cmdData.indexOf(static_cast<int8_t>(m_frameTail));
    if (indexTail == -1) {
        return false;
    }

    while (indexTail != -1) {
        auto* frameData = new unsigned char[indexTail - indexHead + 1];
        memcpy(frameData, data + indexHead, indexTail - indexHead + 1);
        int frameLength = frameData[3] | frameData[4] << 8;
        if (frameLength != indexTail - indexHead + 1) {
            indexTail = cmdData.indexOf(m_frameTail, indexTail + 1);
            continue;
        }
        qDebug() << "HIDDataFrame----->" << __func__ << QByteArray(reinterpret_cast<const char*>(frameData), frameLength).toHex();
        const uint16_t crc = frameData[frameLength - 3] | frameData[frameLength - 2] << 8;
        uint16_t crcCal = CRC16::calculate(frameData, frameLength - 3);
        if (crc != crcCal) {
            qDebug() << "HIDDataFrame----->" << __func__ << "crc error";
            indexTail = cmdData.indexOf(m_frameTail, indexTail + 1);
            continue;
        }

        frame.m_frameNumber = frameData[1] | frameData[2] << 8;
        frame.m_length = frameLength;
        frame.m_userType = frameData[5];
        frame.m_userId = frameData[6] | frameData[7] << 8 | frameData[8] << 16 | frameData[9] << 24;
        frame.m_code = frameData[10] | frameData[11] << 8;
        frame.m_date = QByteArray(reinterpret_cast<char*>(frameData) + 12, frameLength - 15);
        return true;
    }

    return false;
}

QDebug operator<<(QDebug debug, const HIDDataFrame &frame) {
    debug.nospace() << "HIDDataFrame["
                    << "frameNumber:" << frame.m_frameNumber
                    << ",userType:" << QString::number(frame.m_userType, 16)
                    << ",userId:" << QString::number(frame.m_userId, 16)
                    << ",code:" << QString::number(frame.m_code, 16)
                    << ",date:" << frame.m_date.toHex()
                    << "]";
    return debug;
}
