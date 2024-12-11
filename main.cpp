#include <QApplication>
#include <QDebug>
#include "ui/MainWindows.h"
#include "ElaApplication.h"
#include <hidapi.h>

#ifdef _WIN32
    #pragma comment (lib,"setupapi.lib")
#endif

int main(int argc, char *argv[]) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    //根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif
    QApplication a(argc, argv);

    hid_device_info *hid_info;//usb链表
    /*打开指定VID PID设备*/
    hid_info = hid_enumerate(0x0, 0x0);
    /*遍历所有信息并打印*/
    for(;hid_info != nullptr;hid_info = hid_info->next){
        qDebug() << "设备接口号" << hid_info->interface_number;
        qDebug() << "设备路径" << hid_info->path;
        qDebug() << "设备序列号" << hid_info->serial_number;
        qDebug() << "设备制造商" << QString::fromWCharArray(hid_info->manufacturer_string);
        qDebug() << "设备产品" << QString::fromWCharArray(hid_info->product_string);
        qDebug() << "设备版本" << hid_info->release_number;
    }
    /*释放链表*/
    hid_free_enumeration(hid_info);

    eApp->init();
    MainWindows w;
    w.show();
    return QApplication::exec();
}
