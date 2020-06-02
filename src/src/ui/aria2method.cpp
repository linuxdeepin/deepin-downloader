#include "aria2method.h"

#include <QJsonObject>
#include <QDateTime>

#include "../database/dbinstance.h"
#include "global.h"
#include "../aria2/aria2rpcinterface.h"

using namespace Global;
aria2Method::aria2Method()
{

}

void aria2Method::aria2MethodAdd(QJsonObject &json)
{
    QString id = json.value("id").toString();
    QString gId = json.value("result").toString();

    DataItem *finddata = m_pDownLoadingTableView->getTableModel()->find(id);
    if(finddata != nullptr) {
        finddata->gid = gId;
        finddata->taskId = id;
        QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        S_Task_Status  get_status;
        S_Task_Status *download_status = new S_Task_Status(finddata->taskId,
                                                                               Global::Status::Active,
                                                                               QDateTime::currentDateTime(),
                                                                               finddata->completedLength,
                                                                               finddata->speed,
                                                                               finddata->totalLength,
                                                                               finddata->percent,
                                                                               finddata->total,
                                                                               finish_time);


        S_Task_Status task;
        DBInstance::getTaskStatusById(finddata->taskId, task);
        if(task.m_taskId != "") {
            DBInstance::updateTaskStatusById(*download_status);
        } else {
            DBInstance::addTaskStatus(*download_status);
        }
        finddata->status = Global::Status::Active;
    } else {
        // 获取下载信息
        // aria2c->tellStatus(gId, gId);
        Aria2RPCInterface::Instance()->getFiles(gId, id);
        DataItem *data = new DataItem;
        data->taskId = id;
        data->gid = gId;
        data->Ischecked = 0;
        QDateTime time = QDateTime::currentDateTime();
        data->createTime = time.toString("yyyy-MM-dd hh:mm:ss");

        S_Task getTaskInfo;
        S_Task  taskInfo;
        DBInstance::getTaskByID(id, getTaskInfo);
        S_Task *task;
        if(getTaskInfo.m_taskId != "") {
            task = new S_Task(getTaskInfo.m_taskId,
                               gId,
                               0,
                               getTaskInfo.m_url,
                               getTaskInfo.m_downloadPath,
                               getTaskInfo.m_downloadFilename,
                               time);
            DBInstance::updateTaskByID(*task);
            data->fileName = getTaskInfo.m_downloadFilename;
        } else {
            task = new S_Task(id, gId, 0, "", "", "Unknow", time);
            DBInstance::addTask(*task);
        }
        m_pDownLoadingTableView->getTableModel()->append(data);
        if((m_searchContent != "") && !data->fileName.contains(m_searchContent)) {
            TableModel *dtModel = this->m_pDownLoadingTableView->getTableModel();
            m_pDownLoadingTableView->setRowHidden(dtModel->rowCount(QModelIndex()), true);
        }
    }
}

void aria2Method::aria2MethodStatusChanged(QJsonObject &json)
{

}

void aria2Method::aria2MethodShutdown(QJsonObject &json)
{

}

void aria2Method::aria2MethodGetFiles(QJsonObject &json)
{

}

void aria2Method::aria2MethodUnpause(QJsonObject &json)
{

}

void aria2Method::aria2MethodForceRemove(QJsonObject &json)
{

}
