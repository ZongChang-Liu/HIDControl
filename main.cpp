﻿#include <QApplication>
#include <QDebug>
#include "ui/MainWindows.h"
#include "ElaApplication.h"
#include "control/CRC16.h"
#include "control/HIDDataFrame.h"

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
    eApp->init();
    MainWindows w;
    w.resize(1000, 700);
    w.show();
    return QApplication::exec();
}
