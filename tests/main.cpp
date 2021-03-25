#include "gtest/gtest.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include "dbinstance.h"
#include "settings.h"
#include "dlmapplication.h"
int main(int argc, char *argv[])
{
    Settings::getInstance()->initWidget();

    //qputenv("QT_QPA_PLATFORM", "offscreen");
    DlmApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}
