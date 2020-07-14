#ifndef DBINSTANCE_H
#define DBINSTANCE_H
#include "database.h"
#include "dbdefine.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

/**
* @file %{CurrentDocument:dbinstance.h}
* @brief 数据库接口库
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date %{CurrentDate:2020-05-26} %{CurrentTime:17:59}
* @copyright 2020-%{CurrentDate:2020} Uniontech Technology Co., Ltd.
*/

class DBInstance
{
public:
    /**
     * @brief 添加任务
     * @param task 任务信息
     * @return true 执行成功 false执行失败
     */
    static bool addTask(Task task);
    /**
     * @brief 删除任务
     * @param id 根据id删除数据库中任务
     * @return true 执行成功 false执行失败
     */
    static bool delTask(QString id);
    /**
     * @brief 删除数据库中所有任务
     * @return true 执行成功 false执行失败
     */
    static bool delAllTask();
    /**
     * @brief 更新任务
     * @param _task 传入任务，根据类中id更新数据库中任务信息
     * @return true 执行成功 false执行失败
     */
    static bool updateTaskByID(Task &_task);
    /**
     * @brief 更新任务
     * @param id 需要查询任务id
     * @param _task 传出根据id所得任务信息
     * @return true 执行成功 false执行失败
     */
    static bool getTaskByID(QString id, Task &_task);
    /**
     * @brief 获取所有任务
     * @param _taskList 传出QList<S_Task>任务
     * @return true 执行成功 false执行失败
     */
    static bool getAllTask(QList<Task> &_taskList); //得到所有任务
    /**
     * @brief 判断Url是否存在
     * @param url 需要查询url
     * @param ret 传出参数，真为存在，假为不存在
     * @return true 执行成功 false执行失败
     */
    static bool isExistUrl(QString url, bool &ret);
    /**
     * @brief 判断Url是否存在
     * @param hash 需要查询hash
     * @param ret 传出参数，真为存在，假为不存在
     * @return true 执行成功 false执行失败
     */
    static bool isExistBtInHash(QString hash, bool &ret);
    /**
     * @brief 添加任务状态
     * @param task 任务状态
     * @return true 执行成功 false执行失败
     */
    static bool addTaskStatus(TaskStatus); //添加S_Task_Status任务状态
    /**
     * @brief 更新任务状态
     * @param @param _task 传入任务状态，根据类中id更新数据库中任务状态信息
     * @return true 执行成功 false执行失败
     */
    static bool updateTaskStatusById(TaskStatus task); //根据id更新S_Task_Status任务
    /**
     * @brief 获取任务状态
     * @param id 需要查询任务状态id
     * @param task 传出根据id所得任务状态信息
     * @return true 执行成功 false执行失败
     */
    static bool getTaskStatusById(QString id, TaskStatus &task); //通过id得到任务状态信息
    /**
     * @brief 获取所有任务状态
     * @return true 执行成功 false执行失败
     */
    static bool getAllTaskStatus(QList<TaskStatus> &taskList); //获取所有任务状态信息

    /**
     * @brief 添加url
     * @param _url url信息
     * @return true 执行成功 false执行失败
     */
    static bool addUrl(UrlInfo _url); //添加一条url
    /**
     * @brief 更新任务
     * @param id 需要查询任务id
     * @param _task 传出根据id所得任务信息
     * @return true 执行成功 false执行失败
     */
    static bool updateUrlById(UrlInfo _url); //根据id更新Url
    /**
     * @brief 获取url信息
     * @param id 需要查url信息id
     * @param task 传出根据id所得url信息
     * @return true 执行成功 false执行失败
     */
    static bool getUrlById(QString urlId, UrlInfo &url); //根据id得到Url
    /**
     * @brief 获取所有url
     * @param urlList 传出所有任务
     * @return true 执行成功 false执行失败
     */
    static bool getAllUrl(QList<UrlInfo> &urlList); //得到所有Url

    /**
     * @brief 获取重复名称个数
     * @param filename 文件名称
     * @return 重复名称个数
     */
    static int getSameNameCount(QString filename);
};

#endif // DBINSTANCE_H
