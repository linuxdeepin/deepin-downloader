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
    /**
     * @brief 获取数据库类指针
     * @return 数据库类指针
     */
    static DataBase &Instance();
    /**
     * @brief 关闭数据库
     */
    static void close();
    /**
     * @brief 获取数据库类中 QSqlDatabase
     * @return 已打开数据库类
     */
    QSqlDatabase &getDB();


private:
    /**
     * @brief 私有默认构造，禁止外部构造
     */
    DataBase();
    /**
     * @brief 私有拷贝构造，禁止外部构造
     */
    DataBase(const DataBase&){};
    /**
     * @brief 私有赋值构造，禁止外部构造
     */
    DataBase & operator=(const DataBase &){ return *this; };

private:
    QSqlDatabase m_db;
};

#endif // DATABASE_H
