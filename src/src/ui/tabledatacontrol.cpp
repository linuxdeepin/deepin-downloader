/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tabledatacontrol.cpp
 *
 * @brief 表格数据管理类
 *
 * @date 2020-06-10 17:56
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tabledatacontrol.h"
#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHeaderView>
#include <QModelIndex>
#include <QJsonObject>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QDesktopServices>
#include <QUuid>
#include <QSound>

#include "../database/dbinstance.h"
#include "global.h"
#include "../aria2/aria2rpcinterface.h"
#include "tableModel.h"
#include "headerView.h"
#include "itemDelegate.h"
#include "settings.h"
#include "topButton.h"
#include "tableView.h"
#include "deleteitemthread.h"
#include "messagebox.h"

using namespace Global;

tableDataControl::tableDataControl(TableView* pTableView, QObject *parent) :
    QObject(parent),
    m_pTableView(pTableView)
{
}


void tableDataControl::aria2MethodAdd(QJsonObject &json, QString &searchContent)
{
    QString id = json.value("id").toString();
    QString gId = json.value("result").toString();

    DataItem *finddata = m_pTableView->getTableModel()->find(id);

    if(finddata != nullptr) {
        finddata->gid = gId;
        finddata->taskId = id;
        QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        S_Task_Status get_status;
        S_Task_Status downloadStatus(finddata->taskId,
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
            DBInstance::updateTaskStatusById(downloadStatus);
        } else {
            DBInstance::addTaskStatus(downloadStatus);
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
        DBInstance::getTaskByID(id, getTaskInfo);
        S_Task task;
        if(getTaskInfo.m_taskId != "") {
            task = S_Task(getTaskInfo.m_taskId,
                          gId,
                          0,
                          getTaskInfo.m_url,
                          getTaskInfo.m_downloadPath,
                          getTaskInfo.m_downloadFilename,
                          time);
            DBInstance::updateTaskByID(task);
            data->fileName = getTaskInfo.m_downloadFilename;
        } else {
            task = S_Task(id, gId, 0, "", "", "Unknow", time);
            DBInstance::addTask(task);
        }
        data->savePath = getTaskInfo.m_downloadPath + "/" + getTaskInfo.m_downloadFilename;
        m_pTableView->getTableModel()->append(data);
        if((searchContent != "") && !data->fileName.contains(searchContent)) {
            TableModel *dtModel = m_pTableView->getTableModel();
            m_pTableView->setRowHidden(dtModel->rowCount(QModelIndex()), true);
        }
    }
}

void tableDataControl::aria2MethodStatusChanged(QJsonObject &json, int iCurrentRow, QString &searchContent)
{
    QJsonObject result = json.value("result").toObject();
    QJsonObject bittorrent = result.value("bittorrent").toObject();
    QString     mode;
    QString     bittorrent_name;
    QString     taskId = json.value("id").toString();
    QString     bittorrent_dir = "";

    if(!bittorrent.isEmpty()) {
        mode = bittorrent.value("mode").toString();
        if(mode == "multi") {
            bittorrent_dir = result.value("dir").toString();
        }
        QJsonObject btInfo = bittorrent.value("info").toObject();
        bittorrent_name = btInfo.value("name").toString();
        QString infoHash = result.value("infoHash").toString();
        S_Url_Info tbUrlInfo;
        S_Url_Info getUrlInfo;
        DBInstance::getUrlById(taskId, getUrlInfo);
        if(getUrlInfo.m_taskId != "") {
            if(getUrlInfo.m_infoHash.isEmpty()) {
                S_Url_Info *url_info = new S_Url_Info(getUrlInfo.m_taskId,
                                                      getUrlInfo.m_url,
                                                      getUrlInfo.m_downloadType,
                                                      getUrlInfo.m_seedFile,
                                                      getUrlInfo.m_selectedNum,
                                                      infoHash);
                DBInstance::updateUrlById(*url_info);
            }
        }
    }
    QJsonArray files = result.value("files").toArray();

    QString filePath;
    QString fileUri;
    for(int i = 0; i < files.size(); ++i) {
        QJsonObject file = files[i].toObject();
        filePath = file.value("path").toString();
        QJsonArray uri = file.value("uris").toArray();
        for(int j = 0; j < uri.size(); ++j) {
            QJsonObject uriObject = uri[j].toObject();
            fileUri = uriObject.value("uri").toString();
        }
    }

    QString gId = result.value("gid").toString();

    long totalLength = result.value("totalLength").toString().toLong();         //字节
    long completedLength = result.value("completedLength").toString().toLong(); //字节
    long downloadSpeed = result.value("downloadSpeed").toString().toLong();     //字节/每秒
    QString fileName = getFileName(filePath);
    QString statusStr = result.value("status").toString();

    int percent = 0;
    int status = 0;

    if((completedLength != 0) && (totalLength != 0)) {
        double tempPercent = completedLength * 100.0 / totalLength;
        percent = tempPercent;
        if((percent < 0) || (percent > 100)) {
            percent = 0;
        }
        if(completedLength == totalLength) {
            statusStr = "complete";
        }
    }

    DataItem *data = m_pTableView->getTableModel()->find(taskId);
    if(data == nullptr) {
        return;
    }
    if(statusStr == "active") {
        status = Global::Status::Active;
        QFileInfo fInfo(filePath);
        if(!fInfo.isFile()) {
            if(Settings::getInstance()->getAutoDeleteFileNoExistentTaskState()){  // 删除文件不存在的任务
                Aria2RPCInterface::Instance()->remove(data->gid);
                DBInstance::delTask(taskId);

                QFileInfo fileinfo(data->savePath);
                if(fileinfo.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
                    QDir tar(data->savePath);
                    tar.removeRecursively();
                } else {
                    QString ariaTempFile = data->savePath + ".aria2";
                    if(!data->savePath.isEmpty()) {
                        QFile::remove(data->savePath);
                        if(QFile::exists(ariaTempFile)) {
                            QThread::sleep(1);
                            QFile::remove(ariaTempFile);
                        }
                    }
                }

                m_pTableView->getTableModel()->removeItem(data);
                return;
            }
            status = Global::Status::Error;
            MessageBox *msg = new MessageBox();
            msg->setUnusual(taskId);
            connect(msg, &MessageBox::unusualConfirmSig, this, &tableDataControl::getUnusualConfirm);
            msg->exec();
            qDebug() << "文件不存在，";
            return;
        }
    } else if(statusStr == "waiting") {
        status = Global::Status::Waiting;
    } else if(statusStr == "paused") {
        status = Global::Status::Paused;
    } else if(statusStr == "error") {
        status = Global::Status::Error;
        dealNotificaitonSettings(statusStr, fileName);
    } else if(statusStr == "complete") {
        status = Global::Status::Complete;

        //下载文件为种子文件
        if(fileName.endsWith(".torrent")) {
            emit signalAutoDownloadBt(filePath);
        }

        //下载文件为磁链种子文件
        QString infoHash = result.value("infoHash").toString();
        if(filePath.startsWith("[METADATA]")) {
            QString dir = result.value("dir").toString();

            emit signalAutoDownloadBt(dir + "/" + infoHash + ".torrent");
            fileName = infoHash + ".torrent";
        }

        //
        dealNotificaitonSettings(statusStr, fileName);
        if(Settings::getInstance()->getDownloadFinishedOpenState()) {
            QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode));
        }
    } else if(statusStr == "removed") {
        status = Global::Status::Removed;
    }

    data->gid = gId;
    data->totalLength = formatFileSize(totalLength);
    data->completedLength = formatFileSize(completedLength);
    data->speed = (downloadSpeed != 0) ? formatDownloadSpeed(downloadSpeed) : "0kb/s";

    if(bittorrent.isEmpty()) {
        if(!fileName.isEmpty() && (data->fileName != fileName)) {
            data->fileName = fileName;
        }

        //                if(data->fileName==QObject::tr("Unknown"))
        //                {
        //                    data->fileName = (fileName.isEmpty()) ?
        // Global::UNKNOWN : fileName;
        //                }
        data->status = status;
    } else {
        // data->fileName = (bittorrent_name.isEmpty()) ? Global::UNKNOWN :
        // bittorrent_name;
        if(mode == "multi") {
            filePath = bittorrent_dir + "/" + bittorrent_name;
        }
        if((totalLength != 0) && (totalLength == completedLength)) {
            data->status = Complete;
            dealNotificaitonSettings("complete", filePath);
        } else {
            data->status = status;
        }

        fileUri = "";
    }
    data->percent = percent;
    data->total = totalLength;
    if(filePath != "") {
        data->savePath = filePath;
    } else {
        data->savePath = getDownloadSavepathFromConfig();
    }

    data->url = fileUri;
    data->time = "";

    if((totalLength != completedLength) && (totalLength != 0) &&
       (data->status == Global::Status::Active)) {
        QTime t(0, 0, 0);
        t = t.addSecs((totalLength - completedLength * 1.0) / downloadSpeed);
        data->time = t.toString("mm:ss");
    } else if((totalLength == 0) && (data->status == Global::Status::Active)) {
        data->time = ("--:--");
    } else {
        if(data->time == "") {
            data->time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }

        //              updatetimer->stop();
    }
    S_Task task;
    S_Task getTask;
    DBInstance::getTaskByID(data->taskId, getTask);
    if(getTask.m_taskId != "") {
        if(getTask.m_url != "") {
            data->url = getTask.m_url;
        }
    }

    m_pTableView->update();
    m_pTableView->reset();
    S_Task_Status getTaskStatus;
    DBInstance::getTaskStatusById(data->taskId, getTaskStatus);

    QDateTime getTime = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
    S_Task_Status saveTaskStatus(data->taskId,
                                 data->status,
                                 getTime,
                                 data->completedLength,
                                 data->speed,
                                 data->totalLength,
                                 data->percent,
                                 data->total,
                                 getTime);
    if(getTaskStatus.m_taskId == "") {
        DBInstance::addTaskStatus(saveTaskStatus);
    } else {
        if((getTaskStatus.m_downloadStatus != data->status) || (getTaskStatus.m_percent != data->speed)) {
            DBInstance::updateTaskStatusById(saveTaskStatus);
        }
    }
    m_pTableView->refreshTableView(iCurrentRow);
    if((data->status == Complete) && (searchContent != "")) {
        searchEditTextChanged(searchContent);
    }
}

void tableDataControl::aria2MethodShutdown(QJsonObject &json)
{
    QString result = json.value("result").toString();

    if(result == "OK") {
        // m_bShutdownOk = true;
        qDebug() << "close downloadmanager";
        //m_pTableView->close();
        DApplication::exit();
    }
}

void tableDataControl::aria2MethodGetFiles(QJsonObject &json, int iCurrentRow)
{
    QString   id = json.value("id").toString();
    DataItem *data = m_pTableView->getTableModel()->find(id);

    if(data == nullptr) { // id等于gid
        data = new DataItem();
        QJsonArray  ja = json.value("result").toArray();
        QJsonObject jo = ja.at(0).toObject();
        data->totalLength = jo.value("length").toString().toLong(); // 文件大小
        data->savePath = jo.value("path").toString();               //下载路径，带文件名
        data->fileName = data->savePath.mid(data->savePath.lastIndexOf('/') + 1);
        QJsonArray uris = jo.value("uris").toArray();
        data->url = uris.at(0).toObject().value("uri").toString();  //下载链接
        data->taskId = id;
        m_pTableView->getTableModel()->append(data);
    }
    m_pTableView->reset();
    m_pTableView->refreshTableView(iCurrentRow);
}

void tableDataControl::aria2MethodUnpause(QJsonObject &json, int iCurrentRow)
{
    QString gId = json.value("result").toString();
    QString taskId = json.value("id").toString();

    DataItem *data = m_pTableView->getTableModel()->find(taskId);

    if(data != nullptr) {
        data->status = Global::Status::Active;
        m_pTableView->refreshTableView(iCurrentRow);
    }
}

void tableDataControl::aria2MethodUnpauseAll(QJsonObject &json, int iCurrentRow)
{
    QList<Global::DataItem *> pItemList = m_pTableView->getTableModel()->dataList();

    foreach(DataItem * pItem, pItemList){
        if(pItem->status != Global::Status::Complete) {
            pItem->status = Global::Status::Active;
        }
        m_pTableView->refreshTableView(iCurrentRow);
    }
}

void tableDataControl::aria2MethodForceRemove(QJsonObject &json)
{
    QString id = json.value("id").toString();

    if(id.startsWith("REDOWNLOAD_")) { // 重新下载前的移除完成后
        QStringList sp = id.split("_");
        QString     taskId = sp.at(2);
        int rd = sp.at(1).toInt();
        QThread::msleep(500);
        emit signalRedownload(taskId, rd);
    }
}

void tableDataControl::saveDataBeforeClose()
{
    QList<DataItem *> dataList = m_pTableView->getTableModel()->dataList();
    QList<DelDataItem *> recyclelist = m_pTableView->getTableModel()->recyleList();

    if(recyclelist.size() > 0) {
        for(int j = 0; j < recyclelist.size(); j++) {
            DelDataItem *pDelData = recyclelist.at(j);
            QDateTime    deltime = QDateTime::fromString(pDelData->deleteTime, "yyyy-MM-dd hh:mm:ss");
            S_Task task(pDelData->taskId, pDelData->gid, 0, pDelData->url, pDelData->savePath,
                        pDelData->fileName, deltime);

            DBInstance::updateTaskByID(task);
        }
    }
    if(dataList.size() > 0) {
        for(int i = 0; i < dataList.size(); i++) {
            DataItem *data = dataList.at(i);
            QDateTime time = QDateTime::fromString(data->createTime, "yyyy-MM-dd hh:mm:ss");


            S_Task task(data->taskId, data->gid, 0, data->url, data->savePath,
                        data->fileName, time);

            DBInstance::updateTaskByID(task);
            QDateTime finishTime;
            if(data->status == Global::Status::Complete) {
                finishTime = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
            } else {
                finishTime = QDateTime::currentDateTime();
            }
            S_Task_Status get_status;
            int status;
            if((data->status == Global::Status::Complete) || (data->status == Global::Status::Removed)) {
                status = data->status;
            } else {
                status = Global::Status::Lastincomplete;
            }

            S_Task_Status downloadStatus(data->taskId, status, finishTime, data->completedLength, data->speed,
                                          data->totalLength,data->percent,data->total,finishTime);

            if(DBInstance::getTaskStatusById(data->taskId, get_status) != false) {
                DBInstance::updateTaskStatusById(downloadStatus);
            } else {
                DBInstance::addTaskStatus(downloadStatus);
            }
        }
    }
}

QString tableDataControl::getFileName(const QString &url)
{
    return QString(url).right(url.length() - url.lastIndexOf('/') - 1);
}

void tableDataControl::dealNotificaitonSettings(QString statusStr, QString fileName)
{
    // 获取免打扰模式值
    QVariant undisturbed_mode_switchbutton = Settings::getInstance()->m_pSettings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    if(undisturbed_mode_switchbutton.toBool()) {
        bool topStatus = m_pTableView->isTopLevel();
        bool maxStatus = m_pTableView->isMaximized();
        if(topStatus && maxStatus) {
            return;
        }
    }

    bool afterDownloadPlayTone = Settings::getInstance()->getDownloadFinishedPlayToneState();
    if(afterDownloadPlayTone) {
        QSound::play(":/media/wav/downloadfinish.wav");
    } else {
        qDebug() << " not in select down load finsh wav" << endl;
    }

    bool downloadInfoNotify = Settings::getInstance()->getDownloadInfoSystemNotifyState();
    if(downloadInfoNotify) {
        QProcess *p = new QProcess;
        QString   showInfo;
        if(statusStr == "error") {
            showInfo = fileName + tr(" download failed, network error");
        } else {
            showInfo = fileName + tr(" download finished");
        }
        p->start("notify-send", QStringList() << showInfo);
        p->waitForStarted();
        p->waitForFinished();
    }
}

QString tableDataControl::formatFileSize(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + "B";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + "KB";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + "MB";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + "GB";
    }

    return result;
}

QString tableDataControl::getDownloadSavepathFromConfig()
{
    return Settings::getInstance()->getDownloadSavePath();;
}

QString tableDataControl::formatDownloadSpeed(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + " B/s";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + " KB/s";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + " MB/s";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + " GB/s";
    }

    return result;
}

bool tableDataControl::checkFileExist(QString &filePath)
{
    QFileInfo fInfo(filePath);

    return fInfo.isFile();
}

void tableDataControl::getUnusualConfirm(int index, const QString &taskId)
{
    DataItem *pItem = m_pTableView->getTableModel()->find(taskId);
    if( nullptr == pItem){
        return;
    }

    QStringList strlist;
    strlist.append(pItem->url);
    Aria2RPCInterface::Instance()->remove(pItem->gid,pItem->taskId);
    m_pTableView->getTableModel()->removeItem(pItem);
    DBInstance::delTask(taskId);
    if(0 == index){
        emit signalDownload(strlist, Settings::getInstance()->getDownloadSavePath());
    }
}

void tableDataControl::searchEditTextChanged(QString text)
{
    TableModel *pModel = m_pTableView->getTableModel();

    if(text == "") {
        for(int i = 0; i < pModel->rowCount(QModelIndex()); i++) {
            m_pTableView->setRowHidden(i, false);
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    } else {
        for(int i = 0; i < pModel->rowCount(QModelIndex()); i++) {
            m_pTableView->setRowHidden(i, false);
            QString fileName = pModel->data(pModel->index(i, 1), TableModel::FileName).toString();
            if(!fileName.contains(text, Qt::CaseInsensitive)) {
                m_pTableView->setRowHidden(i, true);
            }
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    }
    m_pTableView->reset();
}

int tableDataControl::onDelAction(int currentLab)
{
    int selectedCount = 0;
    if(currentLab == 2) {
        QList<DelDataItem *> pList;
        m_pRecycleDeleteList.clear();
        pList = m_pTableView->getTableModel()->recyleList();
        for(int i = 0; i < pList.size(); ++i) {
            if((pList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                m_pRecycleDeleteList.append(pList.at(i));
                selectedCount++;
            }
        }
    } else {
        QList<DataItem *> pSelectList;
        m_pDeleteList.clear();
        pSelectList = m_pTableView->getTableModel()->renderList();
        for(int i = 0; i < pSelectList.size(); ++i) {
            DataItem *data;
            if(currentLab == 1) {
                if(pSelectList.at(i)->status == Complete) {
                    if((pSelectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            } else {
                if(pSelectList.at(i)->status != Complete) {
                    if((pSelectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            }
        }
    }
    return selectedCount;
}

int tableDataControl::RedownloadDownloadAndFinishList(QList<Global::DataItem*> &reloadList)
{
    int selectedCount = 0;

    QList<DataItem *> selectList;
    reloadList.clear();
    selectList = m_pTableView->getTableModel()->renderList();
    for(int i = 0; i < selectList.size(); ++i) {
        if(selectList.at(i)->status == Complete || selectList.at(i)->status == Error) {
            if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                DataItem *data = selectList.at(i);
                reloadList.append(data);
                ++selectedCount;
            }
        }
    }
    return selectedCount;
}

int tableDataControl::RedownloadTrashList(QList<DelDataItem *> &reloadList)
{
    int selectedCount = 0;
    QList<DelDataItem *> selectList;
    reloadList.clear();
    selectList = m_pTableView->getTableModel()->recyleList();
    for(int i = 0; i < selectList.size(); ++i) {
        if(selectList.at(i)->status == Removed) {
            if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                DelDataItem *data = selectList.at(i);
                reloadList.append(data);
                ++selectedCount;
            }
        }
    }
    return selectedCount;
}

void tableDataControl::onReturnOriginAction()
{


}

void tableDataControl::onOpenFileAction()
{

}

int tableDataControl::onOpenFolderAction(int currentLab)
{
    QList<DataItem *> selectList;
    QList<DelDataItem *> delList;
    int selectedCount = 0;
    if(currentLab == 2) {
        delList = m_pTableView->getTableModel()->recyleList();
        for(int j = 0; j < delList.size(); ++j) {
            if((delList.at(j)->Ischecked == 1) && !m_pTableView->isRowHidden(j)) {
                DelDataItem *deldata = delList.at(j);
                selectedCount++;
                QString file_path = deldata->savePath;
                QString file_name = deldata->fileName;
                QString default_savePath = getDownloadSavepathFromConfig();
                QString file_folder_path;

                if(default_savePath != deldata->savePath) {
                    int name_length = file_name.size();
                    int file_path_length = file_path.size();
                    int folder_path_length = file_path_length - name_length - 1;
                    file_folder_path = file_path.left(folder_path_length);
                } else {
                    file_folder_path = default_savePath;
                }

                QString path = QString("file:///") + file_folder_path;
                QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
            }
        }
    } else {
        selectList = m_pTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(currentLab == 1) {
                if(selectList.at(i)->status == Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        DataItem *data = selectList.at(i);
                        ++selectedCount;

                        QString file_path = data->savePath;
                        QString file_name = data->fileName;
                        QString default_savePath = getDownloadSavepathFromConfig();
                        QString file_folder_path;

                        if(default_savePath != data->savePath) {
                            int name_length = file_name.size();
                            int file_path_length = file_path.size();
                            int folder_path_length = file_path_length - name_length - 1;
                            file_folder_path = file_path.left(folder_path_length);
                        } else {
                            file_folder_path = default_savePath;
                        }

                        QString path = QString("file:///") + file_folder_path;
                        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                    }
                }
            } else {
                if(selectList.at(i)->status != Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        DataItem *data = selectList.at(i);
                        ++selectedCount;

                        QString file_path = data->savePath;
                        QString file_name = data->fileName;
                        QString default_savePath = getDownloadSavepathFromConfig();
                        QString file_folder_path;

                        if(default_savePath != data->savePath) {
                            int name_length = file_name.size();
                            int file_path_length = file_path.size();
                            int folder_path_length = file_path_length - name_length - 1;
                            file_folder_path = file_path.left(folder_path_length);
                        } else {
                            file_folder_path = default_savePath;
                        }


                        QString path = QString("file:///") + file_folder_path;
                        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                    }
                }
            }
        }
    }
    return selectedCount;
}

void tableDataControl::onRenameAction()
{

}

void tableDataControl::onClearRecyleAction()
{

}

int tableDataControl::onCopyUrlAction(int currentLab, QString &copyUrl)
{
    QList<DelDataItem *> recycle_selectList;
    int selectedCount = 0;
    QString url;

    if(currentLab == 2) {
        recycle_selectList = m_pTableView->getTableModel()->recyleList();
        for(int i = 0; i < recycle_selectList.size(); ++i) {
            DelDataItem *data = recycle_selectList.at(i);
            if((data->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                S_Url_Info getUrlInfo;
                DBInstance::getUrlById(data->taskId, getUrlInfo);
                if(getUrlInfo.m_taskId != "") {
                    if(getUrlInfo.m_downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + getUrlInfo.m_infoHash;
                    }
                } else {
                    url = data->url;
                }

                ++selectedCount;
                if(copyUrl == "") {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    } else {
        QList<DataItem *> selectList;

        selectList = m_pTableView->getTableModel()->renderList();

        for(int i = 0; i < selectList.size(); ++i) {
            DataItem *data;
            bool isSelect = false;
            if((currentLab == 1) && (selectList.at(i)->status == Complete)) {
                if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selectedCount;
                    isSelect = true;
                }
            } else {
                if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selectedCount;
                    isSelect = true;
                }
            }
            if(isSelect) {
                S_Url_Info getUrlInfo;
                QString    url;
                DBInstance::getUrlById(data->taskId, getUrlInfo);
                if(getUrlInfo.m_taskId != "") {
                    if(getUrlInfo.m_downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + getUrlInfo.m_infoHash;
                    }
                } else {
                    url = data->url;
                }
                if(copyUrl == "") {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    }
    return selectedCount;
}

int tableDataControl::onDeletePermanentAction(int currentLab)
{
    int selectedCount = 0;

    if(currentLab == 2) {
        QList<DelDataItem *> recycle_selectList;
        m_pRecycleDeleteList.clear();
        recycle_selectList = m_pTableView->getTableModel()->recyleList();
        for(int i = 0; i < recycle_selectList.size(); ++i) {
            if((recycle_selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                m_pRecycleDeleteList.append(recycle_selectList.at(i));
                selectedCount++;
            }
        }
    } else {
        QList<DataItem *> selectList;
        m_pDeleteList.clear();
        selectList = m_pTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            DataItem *data;
            if(currentLab == 1) {
                if(selectList.at(i)->status == Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            } else {
                if(selectList.at(i)->status != Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            }
        }
    }
    return selectedCount;
}

void tableDataControl::onDeleteDownloadListConfirm(bool ischecked, bool permanent, TableView* pRecycleTableView)
{
    QString gid;
    QString aria_temp_file;
    QString save_path;
    QString taskId;
    bool    ifDeleteLocal = permanent || ischecked;

    QList<DataItem> threadDeleteList;
    for(int i = 0; i < m_pDeleteList.size(); i++) {
        DataItem tempdata;

        tempdata.status = m_pDeleteList.at(i)->status;
        tempdata.percent = m_pDeleteList.at(i)->percent;
        tempdata.total = m_pDeleteList.at(i)->total;
        tempdata.Ischecked = m_pDeleteList.at(i)->Ischecked;
        tempdata.taskId = m_pDeleteList.at(i)->taskId;
        tempdata.fileName = m_pDeleteList.at(i)->fileName;
        tempdata.completedLength =  m_pDeleteList.at(i)->completedLength;
        tempdata.totalLength =  m_pDeleteList.at(i)->totalLength;
        tempdata.savePath = m_pDeleteList.at(i)->savePath;
        tempdata.speed = m_pDeleteList.at(i)->speed;
        tempdata.gid = m_pDeleteList.at(i)->gid;
        tempdata.url = m_pDeleteList.at(i)->url;
        tempdata.time = m_pDeleteList.at(i)->time;
        tempdata.createTime = m_pDeleteList.at(i)->createTime;

        threadDeleteList.append(tempdata);
    }

    DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadDeleteList,
                                                               m_pTableView,
                                                               Aria2RPCInterface::Instance(),
                                                               ifDeleteLocal,
                                                               "download_delete");
    pDeleteItemThread->start();

    for(int i = 0; i < m_pDeleteList.size(); i++) {
        DataItem *data = m_pDeleteList.at(i);
        save_path = data->savePath;
        gid = data->gid;
        taskId = data->taskId;
        QDateTime finish_time;
        if(data->status == Complete) {
            finish_time = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
        } else {
            finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        }

        S_Task_Status getStatus;
        S_Task_Status downloadStatus(data->taskId,
                                     Global::Status::Removed,
                                     QDateTime::currentDateTime(),
                                     data->completedLength,
                                     data->speed,
                                     data->totalLength,
                                     data->percent,
                                     data->total,
                                     finish_time);


        if(permanent || ischecked) {
            DBInstance::delTask(taskId);

            // Aria2RPCInterface::Instance()->purgeDownloadResult(data->gid);
        }

        if(!permanent && !ischecked) {
            DelDataItem *delData = new DelDataItem;
            delData->taskId = data->taskId;
            delData->gid = data->gid;
            delData->url = data->url;
            delData->fileName = data->fileName;
            delData->savePath = data->savePath;
            delData->Ischecked = false;
            delData->totalLength = data->totalLength;
            delData->completedLength = data->completedLength;
            delData->deleteTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            delData->finishTime = data->time;

            pRecycleTableView->getTableModel()->append(delData);
            m_pTableView->update();

            if(DBInstance::getTaskStatusById(delData->taskId, getStatus)) {
                DBInstance::updateTaskStatusById(downloadStatus);
            } else {
                DBInstance::addTaskStatus(downloadStatus);
            }
        }

        m_pTableView->getTableModel()->removeItem(data);
    }
}

void tableDataControl::onDeleteRecycleListConfirm(bool ischecked, bool permanent)
{

    bool    ifDeleteLocal = permanent || ischecked;
    QList<DelDataItem> threadRecycleDeleteList;
    for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
        DelDataItem tempdata;
        tempdata.status = m_pRecycleDeleteList.at(i)->status;
        tempdata.Ischecked = m_pRecycleDeleteList.at(i)->Ischecked;
        tempdata.taskId = m_pRecycleDeleteList.at(i)->taskId;
        tempdata.fileName = m_pRecycleDeleteList.at(i)->fileName;
        tempdata.completedLength =  m_pRecycleDeleteList.at(i)->completedLength;
        tempdata.savePath = m_pRecycleDeleteList.at(i)->savePath;
        tempdata.gid = m_pRecycleDeleteList.at(i)->gid;
        tempdata.url = m_pRecycleDeleteList.at(i)->url;
        tempdata.totalLength = m_pRecycleDeleteList.at(i)->totalLength;
        tempdata.deleteTime = m_pRecycleDeleteList.at(i)->deleteTime;
        tempdata.finishTime = m_pRecycleDeleteList.at(i)->finishTime;
        threadRecycleDeleteList.append(tempdata);
    }
    DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadRecycleDeleteList,
                                                               m_pTableView,
                                                               Aria2RPCInterface::Instance(),
                                                               ifDeleteLocal,
                                                               "recycle_delete");
    connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, [=](QString gId, QString id){
        Aria2RPCInterface::Instance()->remove(gId, id);
    });
    pDeleteItemThread->start();

    for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
        DelDataItem *data = new DelDataItem;
        data = m_pRecycleDeleteList.at(i);
        DBInstance::delTask(data->taskId);
        m_pTableView->getTableModel()->removeItem(data);
    }
}

void tableDataControl::downloadListRedownload(QString id)
{
    DataItem *data = m_pTableView->getTableModel()->find(id);
    if(data == nullptr) {
        return;
    }
    QString url = data->url;
    QString ariaTempFile = data->savePath + ".aria2";
    QString taskId = data->taskId;
//    if(!data->savePath.isEmpty()) {
//        QFileInfo fi(data->savePath);
//        if(fi.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
//            QDir tar(data->savePath);
//            tar.removeRecursively();
//        } else {
//            QFile::remove(data->savePath);
//        }
//        if(QFile::exists(ariaTempFile)) {
//            QThread::sleep(1);
//            QFile::remove(ariaTempFile);
//        }
//    }

    QString filePath = data->savePath;
    QString fileName = data->fileName;
    QString defaultSavepath = getDownloadSavepathFromConfig();
    QString savePath;

    if(defaultSavepath != data->savePath) {
        int nameLength = fileName.size();
        int filePathLength = filePath.size();
        int folderPathLength = filePathLength - nameLength - 1;
        savePath = filePath.left(folderPathLength);
    } else {
        savePath = defaultSavepath;
    }

    //m_pTableView->getTableModel()->removeItem(data);
    //DBInstance::delTask(taskId);

    S_Url_Info getUrlInfo;
    DBInstance::getUrlById(taskId, getUrlInfo);
    if(getUrlInfo.m_taskId != "") {
        if(getUrlInfo.m_downloadType == "torrent") {
            QMap<QString, QVariant> opt;
            opt.insert("dir",          savePath);
            opt.insert("select-file", getUrlInfo.m_selectedNum);

            // aria2c->addTorrent(get_url_info->seedFile,opt,get_url_info->task_id);
            S_Task addTask(getUrlInfo.m_taskId,
                           "",
                           0,
                           "",
                           "",
                           fileName,
                           QDateTime::currentDateTime());
            DBInstance::addTask(addTask);
            Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
        }
    } else {
        QUuid   uuid = QUuid::createUuid();
        QString strId = uuid.toString();
        QMap<QString, QVariant> opt;
        int count = DBInstance::getSameNameCount(data->fileName);
        QString outFileName;
        if(count > 0) {
            QString name1 = data->fileName.mid(0, fileName.lastIndexOf('.'));
            name1 += QString("_%1").arg(count);
            outFileName = name1 + fileName.mid(fileName.lastIndexOf('.'), fileName.length());
        }
        opt.insert("dir", savePath);
        opt.insert("out", outFileName);
        Aria2RPCInterface::Instance()->addUri(url, opt, strId);

        QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
        if(!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            const QByteArray filename_byte = filename.toLatin1();
            QString filename_decode = QUrl::fromPercentEncoding(filename_byte);
            filename = filename_decode;
        }

        S_Task addTask(strId, "", 0, url, filePath, filename, QDateTime::currentDateTime());
        DBInstance::addTask(addTask);
    }
}

void tableDataControl::recycleListRedownload(QString id)
{
    DelDataItem *data = m_pTableView->getTableModel()->find(id, 2);
    if(data == nullptr) {
        return;
    }
    QString url = data->url;
    QString taskId = data->taskId;
    QString ariaTempFile = data->savePath + ".aria2";
    if(!data->savePath.isEmpty()) {
        QFileInfo fi(data->savePath);
        if(fi.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
            QDir tar(data->savePath);
            tar.removeRecursively();
        } else {
            QFile::remove(data->savePath);
        }
        if(QFile::exists(ariaTempFile)) {
            QFile::remove(ariaTempFile);
        }
    }
    QString filePath = data->savePath;
    QString fileName = data->fileName;
    QString defaultSavepath = getDownloadSavepathFromConfig();
    QString savePath;
    if(defaultSavepath != data->savePath) {
        int name_length = fileName.size();
        int filePathLength = filePath.size();
        int folderPathLength = filePathLength - name_length - 1;
        savePath = filePath.left(folderPathLength);
    } else {
        savePath = defaultSavepath;
    }


   // m_pTableView->getTableModel()->removeItem(data);

   // DBInstance::delTask(taskId);
    QUuid   uuid = QUuid::createUuid();
    QString strId = uuid.toString();
    S_Url_Info getUrlInfo;
    DBInstance::getUrlById(taskId, getUrlInfo);
    if(getUrlInfo.m_taskId != "") {
        if(getUrlInfo.m_downloadType == "torrent") {
            if(!getUrlInfo.m_infoHash.isEmpty()) {
                QFile::remove(getUrlInfo.m_infoHash + ".torrent");
            }
            QMap<QString, QVariant> opt;
            opt.insert("dir",          savePath);
            opt.insert("select-file", getUrlInfo.m_selectedNum);

            // aria2c->addTorrent(get_url_info->seedFile,opt,get_url_info->task_id);

            S_Task addTask(getUrlInfo.m_taskId,
                           "",
                           0,
                           "",
                           "",
                           fileName,
                           QDateTime::currentDateTime());

            DBInstance::addTask(addTask);
            Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
        }
    } else {
        QMap<QString, QVariant> opt;
        int count = DBInstance::getSameNameCount(data->fileName);
        QString outFileName;
        if(count > 0) {
            QString name1 = data->fileName.mid(0, fileName.lastIndexOf('.'));
            name1 += QString("_%1").arg(count);
            outFileName = name1 + fileName.mid(fileName.lastIndexOf('.'), fileName.length());
        }
        opt.insert("dir", savePath);
        opt.insert("out", outFileName);
        Aria2RPCInterface::Instance()->addUri(url, opt, strId);
        QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
        if(!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            const QByteArray filename_byte = filename.toLatin1();
            QString filename_decode = QUrl::fromPercentEncoding(filename_byte);
            filename = filename_decode;
        }
        S_Task addTask(strId, "", 0, url, filePath, filename, QDateTime::currentDateTime());
        DBInstance::addTask(addTask);
    }
}

