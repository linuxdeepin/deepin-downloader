#ifndef DATABASE_H
#define DATABASE_H

#include <QStandardPaths>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>

/*
 * @class DataBase
 * @brief 链接数据库单例类
 */

const QString DATABASE_VERISON = "1";       //数据库版本
const QString UOS_DOWNLOAD_DATABASE_FILENAME = "uos-downloadmanager-task"+DATABASE_VERISON+".db";   //数据库名字
const QString UOS_DONWLOAD_DATABASE_PATH = "/usr/share/uos-downloadmanager/database/";              //备份数据库路径
const QString UOS_DOWNLOAD_DATABASE_OLD_FILENAME = "uos-downloadmanager-task.db";                   //备份数据库名字

class DataBase
{
public:
    static DataBase &Instance();    //打开数据库
    static void close();
    QSqlDatabase &getDB();


private:
    DataBase();         //禁止构造函数
    DataBase(const DataBase&){};//禁止拷贝构造函数
    DataBase & operator=(const DataBase &){ return *this; };//禁止赋值拷贝构造函数
    QSqlDatabase m_db;
};

#endif // DATABASE_H
