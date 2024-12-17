//
// Created by liu_zongchang on 2024/12/16 13:19.
// Email 1439797751@qq.com
//
//
#include <QDebug>
#include "HIDDataFrame.h"

#include "CRC16.h"

unsigned char* HIDDataFrame::createCommand(HIDDataFrame& frame)
{
    frame.m_length = sizeof(frame.m_frameHead) + sizeof(frame.m_frameNumber) + sizeof(frame.m_userType) + sizeof(frame.m_userId) +
                 sizeof(frame.m_code) + frame.m_date.size() + 2 + sizeof(frame.m_frameTail);
    auto* cmdData = new unsigned char[frame.m_length];
    memset(cmdData, 0, frame.m_length);

    frame.m_frameNumber++;
    if (frame.m_frameNumber > 0xFFFF) {
        frame.m_frameNumber = 0;
    }

    //head 8bit
    cmdData[0] = frame.m_frameHead;

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
    unsigned short crc = CRC16::calculate(cmdData, frame.m_length - 3);
    cmdData[frame.m_length - 3] = crc;
    cmdData[frame.m_length - 2] = crc >> 8;

    //tail 8bit
    cmdData[frame.m_length - 1] = frame.m_frameTail;
    //打印数组大小
    qDebug() << "HIDDataFrame----->" << __func__ << QByteArray(reinterpret_cast<const char*>(cmdData), (int)frame.m_length).toHex();
    return cmdData;
}

bool HIDDataFrame::parseCommand(const QByteArray& data, HIDDataFrame& frame)
{
    qDebug() << "HIDDataFrame----->" << __func__ << data.toHex();
    try {
        if (data.isEmpty()) {
            qDebug() << "HIDDataFrame---->\n" << __func__
                     << "data error";
            return false;
        }

        if (data.length() < 4) {
            qDebug() << "HIDDataFrame---->\n" << __func__
                     << "length error";
            return false;
        }

        const auto head = static_cast<unsigned char>(data[0]);
        if (head != 0xAA) {
            qDebug() << "HIDDataFrame---->\n" << __func__
                     << "head error";
            return false;
        }

        const auto tail = static_cast<unsigned char>(data[data.size() - 1]);
        if (tail != 0xFF) {
            qDebug() << "HIDDataFrame---->\n" << __func__
                     << "tail error";
            return false;
        }

        QByteArray crc = data.mid(1, data.size() - 3);
        if (!CRC16::check(reinterpret_cast<const unsigned char*>(crc.data()), crc.size())) {
            qDebug() << "HIDDataFrame---->\n" << __func__
                     << "crc error";
            return false;
        }

        frame.m_frameNumber = data[2] << 8 | data[1];

        const unsigned int length = static_cast<unsigned char>(data[4]) << 8 | static_cast<unsigned char>(data[5]);
        if (length != data.size()) {
            return false;
        }

        frame.m_userType = static_cast<unsigned char>(data[5]);

        frame.m_userId = static_cast<unsigned char>(data[9]) << 24 | static_cast<unsigned char>(data[8]) << 16 |
                         static_cast<unsigned char>(data[7]) << 8 | static_cast<unsigned char>(data[6]);

        frame.m_code = static_cast<unsigned char>(data[11]) << 8 | static_cast<unsigned char>(data[10]);

        frame.m_date = data.mid(12, data.size() - 22);
    } catch (const std::exception &e) {
        qDebug() << "HIDDataFrame---->\n" << __func__
                 << e.what();
        return false;
    }


    return true;
}

QDebug operator<<(QDebug debug, const HIDDataFrame &frame) {
    debug.nospace() << "HIDDataFrame["
                    << "frameNumber:" << frame.m_frameNumber
                    << ",userType:" << frame.m_userType
                    << ",userId:" << frame.m_userId
                    << ",code:" << frame.m_code
                    << ",date:" << frame.m_date.toHex()
                    << "]";
    return debug;
}