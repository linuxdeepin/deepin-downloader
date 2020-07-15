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
    sql.addBindValue(task.taskId);
    sql.addBindValue(task.gid);
    sql.addBindValue(task.gidIndex);
    sql.addBindValue(task.url);
    sql.addBindValue(task.downloadPath);
    sql.addBindValue(task.downloadFilename);
    sql.addBindValue(task.createTime);
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
    sql.prepare("delete from  download_taskstatus where task_id=?;");
    sql.addBindValue(taskId);
    if (!sql.exec()) {
        QSqlError error = sql.lastError();
        qWarning() << "Delete download_TaskStatus failed : " << error;
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from  urlInfo where task_id=?;");
    sql.addBindValue(taskId);
    if (!sql.exec()) {
        QSqlError error = sql.lastError();
        qWarning() << "Delete urlInfo failed : " << error;
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
    sql.prepare("delete from download_taskstatus;");
    if (!sql.exec()) {
        qWarning() << "Delete download_task failed : " << sql.lastError();
        q.close();
        return false;
    }
    sql.clear();
    sql.prepare("delete from urlInfo;");
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
    sql.addBindValue(task.gid);
    sql.addBindValue(task.gidIndex);
    sql.addBindValue(task.url);
    sql.addBindValue(task.downloadPath);
    sql.addBindValue(task.downloadFilename);
    sql.addBindValue(task.createTime);
    sql.addBindValue(task.taskId);

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

        task.taskId = sql.value(0).toString();
        task.gid = sql.value(1).toString(); //下载gid
        task.gidIndex = sql.value(2).toInt(); //位置index
        task.url = sql.value(3).toString(); //下载url地址
        task.downloadPath  = sql.value(4).toString(); //下载全路径包括文件名
        task.downloadFilename = sql.value(5).toString(); //下载文件名
        task.createTime = sql.value(6).toDateTime(); //任务创建时间

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
        task.taskId = sql.value(0).toString();

        task.gid = sql.value(1).toString(); //下载gid
        task.gidIndex = sql.value(2).toInt(); //位置index
        task.url = sql.value(3).toString(); //下载url地址
        task.downloadPath  = sql.value(4).toString(); //下载全路径包括文件名
        task.downloadFilename = sql.value(5).toString(); //下载文件名
        task.createTime = sql.value(6).toDateTime(); //任务创建时间
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
    sql.prepare("insert into download_taskstatus values (?,?,?,?,?,?,?,?,?);");
    sql.addBindValue(task.taskId);
    sql.addBindValue(task.downloadStatus);
    sql.addBindValue(task.modifyTime);
    sql.addBindValue(task.compeletedLength);
    sql.addBindValue(task.downloadSpeed);
    sql.addBindValue(task.totalLength);
    sql.addBindValue(task.percent);
    sql.addBindValue(task.totalFromSource);
    sql.addBindValue(task.finishTime);
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
    sql.prepare("update  download_taskstatus set  download_status=? , modify_time=? ,compeletedLength=? , download_speed=? , totalLength=? ,percent=? , totalFromSource=? ,finish_time=? where task_id= ?");
    sql.addBindValue(task.downloadStatus);
    sql.addBindValue(task.modifyTime);
    sql.addBindValue(task.compeletedLength);
    sql.addBindValue(task.downloadSpeed);
    sql.addBindValue(task.totalLength);
    sql.addBindValue(task.percent);
    sql.addBindValue(task.totalFromSource);
    sql.addBindValue(task.finishTime);
    sql.addBindValue(task.taskId);
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
    QString selectAllAql = "select * from download_taskstatus where task_id='" + taskId + "';";
    sql.prepare(selectAllAql);
    if (!sql.exec()) {
        qWarning() << "update download_TaskStatus failed : " << sql.lastError();
        q.close();
        return false;
    }
    while(sql.next())
    {
        task.taskId = sql.value(0).toString();
        task.downloadStatus = sql.value(1).toInt(); //下载状态
        task.modifyTime = sql.value(2).toDateTime();
        task.compeletedLength = sql.value(3).toString();
        task.downloadSpeed = sql.value(4).toString();
        task.totalLength = sql.value(5).toString();
        task.percent = sql.value(6).toInt();
        task.totalFromSource = sql.value(7).toInt();
        task.finishTime = sql.value(8).toDateTime();
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
    QString selectAllAql = "select * from download_taskstatus;";
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
        task.taskId = sql.value(0).toString();
        task.downloadStatus = sql.value(1).toInt(); //下载状态
        task.modifyTime = sql.value(2).toDateTime();
        task.compeletedLength = sql.value(3).toString();
        task.downloadSpeed = sql.value(4).toString();
        task.totalLength = sql.value(5).toString();
        task.percent = sql.value(6).toInt();
        task.totalFromSource = sql.value(7).toInt();
        task.finishTime = sql.value(8).toDateTime();
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
    sql.prepare("insert into urlInfo values (?,?,?,?,?,?);");
    sql.addBindValue(url.taskId);
    sql.addBindValue(url.url);
    sql.addBindValue(url.downloadType);
    sql.addBindValue(url.seedFile);
    sql.addBindValue(url.selectedNum);
    sql.addBindValue(url.infoHash);
    if (!sql.exec()) {
        qWarning() << "insert urlInfo failed : " << sql.lastError();
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
    sql.prepare("update  urlInfo set url=?,download_type=?,seedFile=?,selectedNum=? ,infoHash=? where task_id= ?");
    sql.addBindValue(url.url);
    sql.addBindValue(url.downloadType);
    sql.addBindValue(url.seedFile);
    sql.addBindValue(url.selectedNum);
    sql.addBindValue(url.infoHash);
    sql.addBindValue(url.taskId);
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
    QString selectAllAql = "select * from urlInfo where task_id='" + urdId + "';";
    sql.prepare(selectAllAql);
    if (!sql.exec()) {
        qWarning() << "select urlInfo failed : " << sql.lastError();
        q.close();
        return false;
    }
    while(sql.next())
    {
        url.taskId = sql.value(0).toString(); //任务id
        url.url = sql.value(1).toString(); // url 下载地址
        url.downloadType = sql.value(2).toString();//下载类型
        url.seedFile = sql.value(3).toString(); //种子文件
        url.selectedNum = sql.value(4).toString(); //选择的种子文件号码
        url.infoHash = sql.value(5).toString(); //种子文件hash值
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
    QString selectAllSql = "select * from urlInfo;";
    sql.prepare(selectAllSql);
    if (!sql.exec()) {
        qDebug() << sql.lastError();
        q.close();
        return false;
    }
    UrlInfo url;
    while(sql.next())
    {
        url.taskId = sql.value(0).toString(); //任务id
        url.url = sql.value(1).toString(); // url 下载地址
        url.downloadType = sql.value(2).toString();//下载类型
        url.seedFile = sql.value(3).toString(); //种子文件
        url.selectedNum = sql.value(4).toString(); //选择的种子文件号码
        url.infoHash = sql.value(5).toString(); //种子文件hash值
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
    QString selectAllSql = "select count(*)  from urlInfo where urlInfo.infoHash='" + hash + "' ;";
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
