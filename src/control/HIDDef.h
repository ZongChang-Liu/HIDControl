//
// Created by liu_zongchang on 2024/12/16 13:23.
// Email 1439797751@qq.com
//
//

#ifndef HID_DEF_H
#define HID_DEF_H
#pragma execution_character_set(push, "utf-8")
#include <QObject>

#define OFFSET_QUERY
#define OFFSET_UPDATE
namespace HIDDef
{
    Q_NAMESPACE

    enum DeviceStatus
    {
        Device_Opened = 0,
        Device_Closed = 1,
        Device_Error = 2,
    };

    enum TempType
    {
        Celsius = 0,
        Fahrenheit = 1,
    };
    Q_ENUM_NS(TempType)

    enum CmdCode
    {
        Setting_Temp_Type = 0x0001,
        Setting_Value_Threshold = 0x0002,
        Setting_Log_Interval = 0x0003,
        Setting_Start_Delay = 0x0004,
        Setting_Device_Time = 0x0005,
        Setting_Device_Timezone = 0x0006,
        Setting_Start_Mode = 0x0007,
        Setting_Stop_Mode = 0x0008,

        Query_Device_Info = 0x0101,
        Query_Device_Model = 0x0102,
        Query_Device_Version = 0x0103,
        Query_Device_ValueNum = 0x0104,
        Query_Log_Info = 0x0105,
        Query_Device_Time = 0x0106,

        Update_Device_Value = 0x0201,
    };


}


#endif //HID_DEF_H
