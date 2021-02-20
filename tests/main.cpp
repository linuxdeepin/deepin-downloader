#include "gtest/gtest.h"
#include <QApplication>
#include <QDir>
#include <iostream>
int main(int argc, char *argv[])
{
    std::cout << "---------------------------------------" << std::endl;
    std::cout << QDir::current().absolutePath().toStdString() << std::endl;
    std::cout << QDir::home().absolutePath().toStdString() << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}
