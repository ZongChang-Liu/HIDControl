//
// Created by liu_zongchang on 2024/12/16 13:19.
// Email 1439797751@qq.com
//
//
#include <QDebug>
#include "HIDDataFrame.h"

#include "CRC16.h"

QByteArray HIDDataFrame::createCommand(HIDDataFrame& frame)
{
    QByteArray cmd;

    //frame number 16bit
    frame.m_frameNumber++;
    if (frame.m_frameNumber > 0xFFFF) {
        frame.m_frameNumber = 0;
    }
    cmd.append(static_cast<char>(frame.m_frameNumber));
    cmd.append(static_cast<char>(frame.m_frameNumber >> 8));


    //frame length 16bit
    cmd.append(static_cast<char>(0));
    cmd.append(static_cast<char>(0));

    //user type 8bit
    cmd.append(static_cast<char>(frame.m_userType));

    //user id 32bit
    cmd.append(static_cast<char>(frame.m_userId));
    cmd.append(static_cast<char>(frame.m_userId >> 8));
    cmd.append(static_cast<char>(frame.m_userId >> 16));
    cmd.append(static_cast<char>(frame.m_userId >> 24));

    //command code 16bit
    cmd.append(static_cast<char>(frame.m_code));
    cmd.append(static_cast<char>(frame.m_code >> 8));

    //data
    cmd.append(frame.m_date);

    //crc 16bit
    const unsigned short crc = CRC16::calculate(reinterpret_cast<const unsigned char*>(cmd.data()), cmd.size());
    cmd.append(static_cast<char>(crc));
    cmd.append(static_cast<char>(crc >> 8));

    //head 8bit
    cmd.prepend(static_cast<char>(frame.m_frameHead));
    //tail 8bit
    cmd.append(static_cast<char>(frame.m_frameTail));

    //frame length
    cmd[4] = static_cast<char>(cmd.size());
    cmd[3] = static_cast<char>(cmd.size() >> 8);
    cmd.prepend(0x01);
    qDebug() << "HIDDataFrame----->" << __func__ << cmd.toHex();
    return cmd;
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