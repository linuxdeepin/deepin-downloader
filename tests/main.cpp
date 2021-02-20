#include "gtest/gtest.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include "dbinstance.h"
#include "settings.h"
int main(int argc, char *argv[])
{
    QSqlDatabase q = DataBase::Instance().getDB();
    q.setDatabaseName(QDir::current().absolutePath() + "/../data/downloader.db");
    q.open();

    Settings::getInstance()->setupCOnfigFile();
    Settings::getInstance()->m_configPath = QDir::current().absolutePath() + "/../docs/config.conf";
    Settings::getInstance()->initWidget();

    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}
