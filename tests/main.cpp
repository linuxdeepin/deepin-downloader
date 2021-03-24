#include "gtest/gtest.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include "dbinstance.h"
#include "settings.h"
#include "dlmapplication.h"
int main(int argc, char *argv[])
{
//    QSqlDatabase q = DataBase::Instance().getDB();
//    q.setDatabaseName(QDir::current().absolutePath() + "/../data/downloader.db");
//    q.open();

//    Settings::getInstance()->setupCOnfigFile();
//    QString configPath = QDir::current().absolutePath() + "/../docs/config.conf";
//    Settings::getInstance()->m_configPath = configPath;
//    QString iniConfigPath = QString("%1/%3/usrConfig.conf")
//            .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
//            .arg("downloader_test");
//    QFile::remove(iniConfigPath);
    //QFile::remove(configPath);
    Settings::getInstance()->initWidget();

    //qputenv("QT_QPA_PLATFORM", "offscreen");
    DlmApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}
