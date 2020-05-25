#ifndef DBINSTANCE_H
#define DBINSTANCE_H
#include "database.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

/*
 * @class DBInstance
 * @brief 数据库接口库
 *
 * 使用示例：
 * DataBase为单例模式
 * 在DBInstance类中写数据库接口，在外部进行调用
   DBInstance::getData();
 */

class DBInstance
{
public:
    static bool getData();  //测试函数
};

#endif // DBINSTANCE_H
