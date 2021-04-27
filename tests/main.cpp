#include "gtest/gtest.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include "dbinstance.h"
#include "settings.h"
#include "dlmapplication.h"

#if defined(CMAKE_SAFETYTEST_ARG_ON)
#include <sanitizer/asan_interface.h>
#endif


int main(int argc, char *argv[])
{
    Settings::getInstance()->initWidget();

    //qputenv("QT_QPA_PLATFORM", "offscreen");
    DlmApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

#if defined(CMAKE_SAFETYTEST_ARG_ON)
    __sanitizer_set_report_path("asan.log");
#endif
    return ret;
}
