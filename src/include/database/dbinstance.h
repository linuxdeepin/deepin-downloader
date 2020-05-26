#ifndef DBINSTANCE_H
#define DBINSTANCE_H
#include "database.h"
#include "dbdefine.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

/*
 * @class DBInstance
 * @brief 数据库接口库
 *
 * 使用示例：
 * DataBase为单例模式
 * 在DBInstance类中写数据库接口，在外部进行调用
   DBInstance::addTask(_Task);
 */

class DBInstance
{
public:

    /*
     * task表操作
     */
    static bool addTask(S_Task);    //添加任务
    static bool delTask(QString id);    //根据id删除任务
    static bool delAllTask();       //删除所有任务
    static bool updateTaskByID(S_Task &_task);   //根据id更新S_Task任务
    static bool getTaskByID(QString id, S_Task &);  //根据id获取任务
    static bool getAllTask(QList<S_Task> &);    //得到所有任务
    static bool isExistUrl();                   //不知道
    /*
     * task_status表操作
     */
    static bool addTaskStatus(S_Task_Status);   //添加S_Task_Status任务状态
    static bool updateTaskStatusById(S_Task_Status task);    //根据id更新S_Task_Status任务
    static bool getTaskStatusById(QString id, S_Task_Status &task); //通过id得到任务状态信息
    static bool getAllTaskStatus(QList<S_Task_Status> & taskList);  //获取所有任务状态信息
    static bool delTaskStatusById(QString id);  //通过id删除任务状态信息
    static bool delAllTaskStatus();             //删除所有任务状态信息
    /*
     * url_info表操作
     */
    static bool addUrl(S_Url_Info);           //添加一条url
    static bool updateUrlById(S_Url_Info _url);    //根据id更新Url
    static bool getUrlById(QString urlId, S_Url_Info &url);       //根据id得到Url
    static bool getAllUrl(QList<S_Url_Info> &urlList);        //得到所有Url
    static bool delUrlById(QString id);       //根据id删除url
    static bool delAllUrl();        //删除所有url

};

#endif // DBINSTANCE_H
