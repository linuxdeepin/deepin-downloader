#include "dbinstance.h"

bool DBInstance::addTask(Task task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("insert into download_task values (?,?,?,?,?,?,?);");
    sql.addBindValue(task.m_taskId);
    sql.addBindValue(task.m_gid);
    sql.addBindValue(task.m_gidIndex);
    sql.addBindValue(task.m_url);
    sql.addBindValue(task.m_downloadPath);
    sql.addBindValue(task.m_downloadFilename);
    sql.addBindValue(task.m_createTime);
    if(!sql.exec())
    {
        qWarning()<<"Insert download_task table failed : " << sql.lastError();
        q.close();
        return false;
    }
    q.close();
    return true;
}

bool DBInstance::delTask(QString taskId)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString str = QString("delete from  download_task where task_id='%1';").arg(taskId);
    sql.prepare(str);
    if (!sql.exec()) {
        QSqlError error = sql.lastError();
        qWarning() << "Delete download_task failed : " << error;
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from  download_TaskStatus where task_id=?;");
    sql.addBindValue(taskId);
    if (!sql.exec()) {
        QSqlError error = sql.lastError();
        qWarning() << "Delete download_TaskStatus failed : " << error;
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from  UrlInfo where task_id=?;");
    sql.addBindValue(taskId);
    if (!sql.exec()) {
        QSqlError error = sql.lastError();
        qWarning() << "Delete UrlInfo failed : " << error;
        q.close();
        return false;
    }
    q.close();
    return true;
}

bool DBInstance::delAllTask()
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("delete from  download_task;");
    if (!sql.exec()) {
        qWarning() << "Delete download_task failed : " << sql.lastError();
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from download_TaskStatus;");
    if (!sql.exec()) {
        qWarning() << "Delete download_task failed : " << sql.lastError();
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from UrlInfo;");
    if (!sql.exec()) {
        qWarning() << "Delete download_task failed : " << sql.lastError();
        q.close();
        return false;
    }

    return true;
}

bool DBInstance::updateTaskByID(Task &task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("update  download_task set  gid=? , gid_index=? , url=? ,download_path=? , download_filename=? ,create_time=? where task_id= ?");
    sql.addBindValue(task.m_gid);
    sql.addBindValue(task.m_gidIndex);
    sql.addBindValue(task.m_url);
    sql.addBindValue(task.m_downloadPath);
    sql.addBindValue(task.m_downloadFilename);
    sql.addBindValue(task.m_createTime);
    sql.addBindValue(task.m_taskId);

    if (!sql.exec()) {
        qWarning() << "Update download_task table failed : " << sql.lastError();
        q.close();
        return false;
    }
    q.close();
    return true;
}

bool DBInstance::getTaskByID(QString taskId, Task &task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("select * from download_task where task_id=:index;");
    sql.addBindValue(taskId);
    if (!sql.exec()) {
        qDebug() << "Select download_task table failed : " << sql.lastError();
        q.close();
        return false;
    }
    while(sql.next())
    {

        task.m_taskId = sql.value(0).toString();
        task.m_gid = sql.value(1).toString(); //下载gid
        task.m_gidIndex = sql.value(2).toInt(); //位置index
        task.m_url = sql.value(3).toString(); //下载url地址
        task.m_downloadPath  = sql.value(4).toString(); //下载全路径包括文件名
        task.m_downloadFilename = sql.value(5).toString(); //下载文件名
        task.m_createTime = sql.value(6).toDateTime(); //任务创建时间

    }
    q.close();
    return true;
}

bool DBInstance::getAllTask(QList<Task> &taskList)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("select * from download_task;");
    if (!sql.exec()) {
        qDebug() << "getAllTask download_task table failed : " << sql.lastError();
        q.close();
        return false;
    }
    Task task;
    while (sql.next())
    {
        task.m_taskId = sql.value(0).toString();

        task.m_gid = sql.value(1).toString(); //下载gid
        task.m_gidIndex = sql.value(2).toInt(); //位置index
        task.m_url = sql.value(3).toString(); //下载url地址
        task.m_downloadPath  = sql.value(4).toString(); //下载全路径包括文件名
        task.m_downloadFilename = sql.value(5).toString(); //下载文件名
        task.m_createTime = sql.value(6).toDateTime(); //任务创建时间
        taskList.push_back(task);
    }
    q.close();
    return true;
}

bool DBInstance::isExistUrl(QString url, bool &ret)
{
    ret = false;
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString selectAllSql = "select count(*)  from download_task where download_task.url='" + url + "' ;";
    qDebug() << selectAllSql;
    sql.prepare(selectAllSql);
    if (!sql.exec()) {
        qWarning() << "select download_task,download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    while (sql.next()) {
        if (sql.value(0).toInt() >= 1) {
            ret = true;
        }
    }
    q.close();
    return true;
}

bool DBInstance::addTaskStatus(TaskStatus task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("insert into download_TaskStatus values (?,?,?,?,?,?,?,?,?);");
    sql.addBindValue(task.m_taskId);
    sql.addBindValue(task.m_downloadStatus);
    sql.addBindValue(task.m_modifyTime);
    sql.addBindValue(task.m_compeletedLength);
    sql.addBindValue(task.m_downloadSpeed);
    sql.addBindValue(task.m_totalLength);
    sql.addBindValue(task.m_percent);
    sql.addBindValue(task.m_totalFromSource);
    sql.addBindValue(task.m_finishTime);
    if(!sql.exec())
    {
        QSqlError error = sql.lastError();
        qWarning() << "insert download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    q.close();
    return true;
}
bool DBInstance::updateTaskStatusById(TaskStatus task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("update  download_TaskStatus set  download_status=? , modify_time=? ,compeletedLength=? , download_speed=? , totalLength=? ,percent=? , totalFromSource=? ,finish_time=? where task_id= ?");
    sql.addBindValue(task.m_downloadStatus);
    sql.addBindValue(task.m_modifyTime);
    sql.addBindValue(task.m_compeletedLength);
    sql.addBindValue(task.m_downloadSpeed);
    sql.addBindValue(task.m_totalLength);
    sql.addBindValue(task.m_percent);
    sql.addBindValue(task.m_totalFromSource);
    sql.addBindValue(task.m_finishTime);
    sql.addBindValue(task.m_taskId);
    if(!sql.exec())
    {
        qWarning()<<"update download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    q.close();
    return true;
}
bool DBInstance::getTaskStatusById(QString taskId, TaskStatus &task)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString selectAllAql = "select * from download_TaskStatus where task_id='" + taskId + "';";
    sql.prepare(selectAllAql);
    if (!sql.exec()) {
        qWarning() << "update download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    while(sql.next())
    {
        task.m_taskId = sql.value(0).toString();
        task.m_downloadStatus = sql.value(1).toInt(); //下载状态
        task.m_modifyTime = sql.value(2).toDateTime();
        task.m_compeletedLength = sql.value(3).toString();
        task.m_downloadSpeed = sql.value(4).toString();
        task.m_totalLength = sql.value(5).toString();
        task.m_percent = sql.value(6).toInt();
        task.m_totalFromSource = sql.value(7).toInt();
        task.m_finishTime = sql.value(8).toDateTime();
    }
    q.close();
    return true;
}
bool DBInstance::getAllTaskStatus(QList<TaskStatus> &taskList)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QString selectAllAql = "select * from download_TaskStatus;";
    QSqlQuery sql;
    sql.prepare(selectAllAql);
    if (!sql.exec()) {
        qWarning() << "get all download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    TaskStatus task;
    while (sql.next())
    {
        task.m_taskId = sql.value(0).toString();
        task.m_downloadStatus = sql.value(1).toInt(); //下载状态
        task.m_modifyTime = sql.value(2).toDateTime();
        task.m_compeletedLength = sql.value(3).toString();
        task.m_downloadSpeed = sql.value(4).toString();
        task.m_totalLength = sql.value(5).toString();
        task.m_percent = sql.value(6).toInt();
        task.m_totalFromSource = sql.value(7).toInt();
        task.m_finishTime = sql.value(8).toDateTime();
        taskList.push_back(task);
    }
    q.close();
    return true;
}

bool DBInstance::addUrl(UrlInfo url)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("insert into UrlInfo values (?,?,?,?,?,?);");
    sql.addBindValue(url.m_taskId);
    sql.addBindValue(url.m_url);
    sql.addBindValue(url.m_downloadType);
    sql.addBindValue(url.m_seedFile);
    sql.addBindValue(url.m_selectedNum);
    sql.addBindValue(url.m_infoHash);
    if (!sql.exec()) {
        qWarning() << "insert UrlInfo failed : " << sql.lastError();
        q.close();
        return false;
    }
    q.close();
    return true;
}
bool DBInstance::updateUrlById(UrlInfo url)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        q.close();
        return false;
    }
    QSqlQuery sql;
    sql.prepare("update  UrlInfo set url=?,download_type=?,seedFile=?,selectedNum=? ,infoHash=? where task_id= ?");
    sql.addBindValue(url.m_url);
    sql.addBindValue(url.m_downloadType);
    sql.addBindValue(url.m_seedFile);
    sql.addBindValue(url.m_selectedNum);
    sql.addBindValue(url.m_infoHash);
    sql.addBindValue(url.m_taskId);

    q.close();
    return true;
}
bool DBInstance::getUrlById(QString urdId, UrlInfo &url)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString selectAllAql = "select * from UrlInfo where task_id='" + urdId + "';";
    sql.prepare(selectAllAql);
    if (!sql.exec()) {
        qWarning() << "select UrlInfo failed : " << sql.lastError();
        q.close();
        return false;
    }
    while(sql.next())
    {
        url.m_taskId = sql.value(0).toString(); //任务id
        url.m_url = sql.value(1).toString(); // url 下载地址
        url.m_downloadType = sql.value(2).toString();//下载类型
        url.m_seedFile = sql.value(3).toString(); //种子文件
        url.m_selectedNum = sql.value(4).toString(); //选择的种子文件号码
        url.m_infoHash = sql.value(5).toString(); //种子文件hash值
    }
    q.close();
    return true;
}
bool DBInstance::getAllUrl(QList<UrlInfo> &urlList)
{
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString selectAllSql = "select * from UrlInfo;";
    sql.prepare(selectAllSql);
    if (!sql.exec()) {
        qDebug() << sql.lastError();
        q.close();
        return false;
    }
    UrlInfo url;
    while(sql.next())
    {
        url.m_taskId = sql.value(0).toString(); //任务id
        url.m_url = sql.value(1).toString(); // url 下载地址
        url.m_downloadType = sql.value(2).toString();//下载类型
        url.m_seedFile = sql.value(3).toString(); //种子文件
        url.m_selectedNum = sql.value(4).toString(); //选择的种子文件号码
        url.m_infoHash = sql.value(5).toString(); //种子文件hash值
        urlList.push_back(url);
    }
    q.close();
    return true;
}

int DBInstance::getSameNameCount(QString filename)
{
    int count = 0;
    QSqlDatabase sqlDatabase = DataBase::Instance().getDB();
    if (!sqlDatabase.open()) {
        qDebug() << sqlDatabase.lastError();
        return 0;
    }

    QSqlQuery sql;
    QString sqlStr =  QString("select count(download_filename) from download_task where download_filename like '" + filename + "%';").arg(filename);
    sql.prepare(sqlStr);
    if (!sql.exec()) {
        qWarning() << "select count(download_filename) failed : " << sql.lastError();
        qWarning() << sqlStr;
        sqlDatabase.close();
        return 0;
    }
    qWarning() << sqlStr;
    while(sql.next())
    {
        count = sql.value(0).toInt();
    }
    return count;
}

bool DBInstance::isExistBtInHash(QString hash, bool &ret)
{
    ret = false;
    QSqlDatabase q = DataBase::Instance().getDB();
    if (!q.open()) {
        qDebug() << q.lastError();
        return false;
    }
    QSqlQuery sql;
    QString selectAllSql = "select count(*)  from UrlInfo where UrlInfo.infoHash='" + hash + "' ;";
    qDebug() << selectAllSql;
    sql.prepare(selectAllSql);
    if (!sql.exec()) {
        qWarning() << "select download_task,download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    while (sql.next()) {
        if (sql.value(0).toInt() >= 1) {
            ret = true;
        }
    }
    q.close();
    return true;
}
