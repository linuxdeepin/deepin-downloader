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
#include <unistd.h>
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
#include <QDBusInterface>
#include <QSharedMemory>
#include <QBuffer>
#include <QMimeDatabase>
#include <QTimer>

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

TableDataControl::TableDataControl(TableView *pTableView, QObject *parent)
    : QObject(parent)
    , m_DownloadTableView(pTableView)
{
}

void TableDataControl::setRecycleTable(TableView *pRecycleTable)
{
    m_RececleTableView = pRecycleTable;
}

void TableDataControl::removeDownloadListJob(DownloadDataItem *pData, bool isAddToRecycle)
{
    QFileInfo fileinfo(pData->savePath);
    if (fileinfo.isDir() && pData->savePath.contains(pData->fileName) && !pData->fileName.isEmpty()) {
        QDir tar(pData->savePath);
        tar.removeRecursively();
    } else {
        QString ariaTempFile = pData->savePath + ".aria2";
        if (!pData->savePath.isEmpty()) {
            QFile::remove(pData->savePath);
            if (QFile::exists(ariaTempFile)) {
                QThread::msleep(100);
                QFile::remove(ariaTempFile);
            }
        }
    }
    if (isAddToRecycle) {
        DeleteDataItem *delData = new DeleteDataItem;
        delData->taskId = pData->taskId;
        delData->gid = pData->gid;
        delData->url = pData->url;
        delData->fileName = pData->fileName;
        delData->savePath = pData->savePath;
        delData->Ischecked = false;
        delData->status = Global::DownloadJobStatus::Removed;
        delData->totalLength = pData->totalLength;
        delData->completedLength = pData->completedLength;
        delData->deleteTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        delData->finishTime = pData->time;
        m_RececleTableView->getTableModel()->append(delData);

        TaskStatus getStatus;
        TaskStatus downloadStatus(pData->taskId,
                                  Global::DownloadJobStatus::Removed,
                                  QDateTime::currentDateTime(),
                                  pData->completedLength,
                                  pData->speed,
                                  pData->totalLength,
                                  pData->percent,
                                  pData->total,
                                  QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss"));
        if (DBInstance::getTaskStatusById(pData->taskId, getStatus)) {
            DBInstance::updateTaskStatusById(downloadStatus);
        } else {
            DBInstance::addTaskStatus(downloadStatus);
        }
    } else {
        DBInstance::delTask(pData->taskId);
    }

    m_DownloadTableView->getTableModel()->removeItem(pData);

    return;
}

void TableDataControl::aria2MethodAdd(QJsonObject &json, QString &searchContent)
{
    QString id = json.value("id").toString();
    if (id == "dht.dat" || id == "dht6.dat") {
        return;
    }
    QString gId = json.value("result").toString();

    DownloadDataItem *findData = m_DownloadTableView->getTableModel()->find(id);

    if (findData != nullptr) {
        findData->gid = gId;
        findData->taskId = id;
        QDateTime finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        TaskStatus downloadStatus(findData->taskId,
                                  Global::DownloadJobStatus::Active,
                                  QDateTime::currentDateTime(),
                                  findData->completedLength,
                                  findData->speed,
                                  findData->totalLength,
                                  findData->percent,
                                  findData->total,
                                  finishTime);

        TaskStatus task;
        DBInstance::getTaskStatusById(findData->taskId, task);
        if (!task.taskId.isEmpty()) {
            DBInstance::updateTaskStatusById(downloadStatus);
        } else {
            DBInstance::addTaskStatus(downloadStatus);
        }
        findData->status = Global::DownloadJobStatus::Active;
    } else {
        // 获取下载信息
        // aria2c->tellStatus(gId, gId);
        Aria2RPCInterface::instance()->getFiles(gId, id);
        DownloadDataItem *data = new DownloadDataItem;
        data->taskId = id;
        data->gid = gId;
        data->Ischecked = 0;
        data->completedLength = "0KB";
        QDateTime time = QDateTime::currentDateTime();
        data->createTime = time.toString("yyyy-MM-dd hh:mm:ss");

        TaskInfo getTaskInfo;
        DBInstance::getTaskByID(id, getTaskInfo);
        TaskInfo task;
        if (!getTaskInfo.taskId.isEmpty()) {
            task = TaskInfo(getTaskInfo.taskId,
                            gId,
                            0,
                            getTaskInfo.url,
                            getTaskInfo.downloadPath,
                            getTaskInfo.downloadFilename,
                            time);
            DBInstance::updateTaskByID(task);
            data->fileName = getTaskInfo.downloadFilename;
        } else {
            task = TaskInfo(id, gId, 0, "", "", "Unknow", time);
            DBInstance::addTask(task);
        }
        data->savePath = getTaskInfo.downloadPath; // + "/" + getTaskInfo.m_downloadFilename;
        data->url = getTaskInfo.url;
        m_DownloadTableView->getTableModel()->append(data);
        m_DownloadTableView->getTableHeader()->onHeaderChecked(false);
        if ((!searchContent.isEmpty()) && !data->fileName.contains(searchContent)) {
            TableModel *dtModel = m_DownloadTableView->getTableModel();
            m_DownloadTableView->setRowHidden(dtModel->rowCount(), true);
        }
        qDebug() << "aria2MethodAdd: " << getTaskInfo.url << "    " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    }
}

void TableDataControl::aria2MethodStatusChanged(QJsonObject &json, int iCurrentRow, QString &searchContent)
{
    QJsonObject result = json.value("result").toObject();
    QJsonObject bittorrent = result.value("bittorrent").toObject();
    QString mode;
    QString bitTorrentName;
    QString taskId = json.value("id").toString();
    QString bitTorrentDir = "";
    QJsonArray files = result.value("files").toArray();
    QString filePath = files[0].toObject().value("path").toString();
    QString fileUri = files[0].toObject().value("uris").toArray()[0].toObject().value("uri").toString();

    QString gId = result.value("gid").toString();

    long totalLength = result.value("totalLength").toString().toLong(); //字节
    long completedLength = result.value("completedLength").toString().toLong(); //字节
    long downloadSpeed = result.value("downloadSpeed").toString().toLong(); //字节/每秒
    QString fileName = getFileName(filePath);
    QString statusStr = result.value("status").toString();
    QString errorCode = result.value("errorCode").toString();
    int percent = 0;
    int status = 0;

    if (filePath.startsWith("[METADATA]")) {
        QString dir = result.value("dir").toString();
        QString infoHash = result.value("infoHash").toString();
        filePath = dir + "/" + infoHash + ".torrent";
    }
    if ((completedLength != 0) && (totalLength != 0)) {
        double tempPercent = completedLength * 100.0 / totalLength;
        percent = static_cast<int>(tempPercent);
        if ((percent < 0) || (percent > 100)) {
            percent = 0;
        }
        if (completedLength == totalLength) {
            statusStr = "complete";
        }
    }

    DownloadDataItem *data = m_DownloadTableView->getTableModel()->find(taskId);
    if (data == nullptr) {
        return;
    }
    if (statusStr == "active") {
        status = Global::DownloadJobStatus::Active;
        QDateTime createTime = QDateTime::fromString(data->createTime, "yyyy-MM-dd hh:mm:ss");
        createTime = createTime.addSecs(5);
        QDateTime currentTime = QDateTime::currentDateTime();
        if ((!QFileInfo::exists(data->savePath)) && (createTime < currentTime)
            && (completedLength > 0) && (!fileName.contains("[METADATA]"))) {
            Aria2RPCInterface::instance()->remove(data->gid);
            if (Settings::getInstance()->getAutoDeleteFileNoExistentTaskState()) { // 删除文件不存在的任务
                removeDownloadListJob(data);
                return;
            }
            data->status = Global::DownloadJobStatus::Error;
            MessageBox msg;
            msg.setUnusual(taskId);
            connect(&msg, &MessageBox::unusualConfirm, this, &TableDataControl::onUnusualConfirm);
            msg.exec();
            //qDebug() << "文件不存在，";
            return;
        }
    } else if (statusStr == "waiting") {
        status = Global::DownloadJobStatus::Waiting;
        downloadSpeed = -1;
    } else if (statusStr == "paused") {
        status = Global::DownloadJobStatus::Paused;
        downloadSpeed = -2;
    } else if (statusStr == "error") {
        //        if ("12" == errorCode) {
        //        }
        status = Global::DownloadJobStatus::Error;
        downloadSpeed = -3;
        dealNotificaitonSettings(statusStr, fileName, errorCode);
    } else if (statusStr == "complete") {
        data->status = Global::DownloadJobStatus::Complete;
        status = Global::DownloadJobStatus::Complete;
        //下载文件为种子文件
        if (fileName.endsWith(".torrent")) {
            data->status = Global::DownloadJobStatus::Complete;
            if (Settings::getInstance()->getAutoOpennewTaskWidgetState()) {
                emit AutoDownloadBt(filePath);
                clearShardMemary();
            }
        }

        //下载文件为磁链种子文件
        QString infoHash = result.value("infoHash").toString();
        bool isMetaData = false;
        if (fileName.startsWith("[METADATA]")) {
            isMetaData = true;
            QString dir = result.value("dir").toString();
            data->status = Global::DownloadJobStatus::Complete;
            fileName = infoHash + ".torrent";
            filePath = dir + "/" + fileName;
            data->savePath = dir + "/" + fileName;
            data->fileName = fileName;
            //if(Settings::getInstance()->getAutoOpennewTaskWidgetState()){

            if (QFile::exists(filePath)) {
                emit AutoDownloadBt(dir + "/" + infoHash + ".torrent");
            } else {
                QTimer::singleShot(3000, this, [=]() {
                    emit AutoDownloadBt(dir + "/" + infoHash + ".torrent");
                });
            }
            //}
        }

        //
        dealNotificaitonSettings(statusStr, fileName, errorCode);
        if (Settings::getInstance()->getDownloadFinishedOpenState() && (!isMetaData) && (!fileName.endsWith(".torrent"))) {
            QString urlDecode = QUrl::fromPercentEncoding(filePath.toUtf8());
            urlDecode = "file:///" + urlDecode;
            QUrl url = QUrl(urlDecode, QUrl::TolerantMode);
            QDesktopServices::openUrl(url);
        }
        if (!checkTaskStatus()) {
            emit whenDownloadFinish();
        }
    } else if (statusStr == "removed") {
        status = Global::DownloadJobStatus::Removed;
    }
    if (nullptr == m_DownloadTableView->getTableModel()->find(taskId)) {
        return;
    }
    data->gid = gId;
    if (totalLength > 0) {
        data->totalLength = formatFileSize(totalLength);
    }
    if (completedLength > 0) {
        data->completedLength = formatFileSize(completedLength);
    }
    data->speed = (downloadSpeed != 0) ? formatDownloadSpeed(downloadSpeed) : "0KB/s";
    if (bittorrent.isEmpty()) {
        if (!fileName.isEmpty() && (data->fileName != fileName)) {
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
        if (mode == "multi") {
            filePath = bitTorrentDir + "/" + bitTorrentName;
        }
        if ((totalLength != 0) && (totalLength == completedLength)) {
            data->status = DownloadJobStatus::Complete;
            dealNotificaitonSettings("complete", filePath, errorCode);
        } else {
            data->status = status;
        }

        fileUri = "";
    }
    data->percent = percent;
    data->total = static_cast<int>(totalLength);
    if (!filePath.isEmpty()) {
        data->savePath = filePath;
    } else {
        data->savePath = getDownloadSavepathFromConfig();
    }

    data->url = fileUri;
    data->time = "";

    if ((totalLength != completedLength) && (totalLength != 0)
        && (data->status == Global::DownloadJobStatus::Active)) {
        QTime t(0, 0, 0);
        double d = 0;
        if (downloadSpeed > 0) {
            d = (totalLength - completedLength * 1.0) / downloadSpeed;
        }
        if (d > 60 * 60 * 24) {
            data->time = tr(" > 1 Day");
        } else {
            t = t.addSecs(static_cast<int>(d));
            data->time = t.toString("hh:mm:ss");
        }

    } else if ((totalLength == 0) && (data->status == Global::DownloadJobStatus::Active)) {
        data->time = ("--:--");
    } else if (data->time.isEmpty()) {
        data->time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    TaskInfo task;
    TaskInfo getTask;
    DBInstance::getTaskByID(taskId, getTask);
    if ((!getTask.taskId.isEmpty()) && (!getTask.url.isEmpty())) {
        data->url = getTask.url;
    }

    //m_pDownloadTableView->update();
    //m_pDownloadTableView->reset();
    TaskStatus getTaskStatus;
    DBInstance::getTaskStatusById(data->taskId, getTaskStatus);

    QDateTime getTime = QDateTime::currentDateTime();
    TaskStatus saveTaskStatus(data->taskId,
                              data->status,
                              getTime,
                              data->completedLength,
                              data->speed,
                              data->totalLength,
                              data->percent,
                              data->total,
                              getTime);
    if (getTaskStatus.taskId.isEmpty()) {
        DBInstance::addTaskStatus(saveTaskStatus);
    } else if ((getTaskStatus.downloadStatus != data->status) || (getTaskStatus.percent != data->speed)) {
        DBInstance::updateTaskStatusById(saveTaskStatus);
    }

    m_DownloadTableView->refreshTableView(iCurrentRow);
    if ((data->status == Complete) && (!searchContent.isEmpty())) {
        searchEditTextChanged(searchContent);
    }
}

void TableDataControl::aria2MethodShutdown(QJsonObject &json)
{
    QString result = json.value("result").toString();

    if (result == "OK") {
        // m_bShutdownOk = true;
        qDebug() << "close downloadmanager";
        //m_pTableView->close();
        //DApplication::exit();
    }
}

void TableDataControl::aria2MethodGetFiles(QJsonObject &json, int iCurrentRow)
{
    QString id = json.value("id").toString();
    DownloadDataItem *data = m_DownloadTableView->getTableModel()->find(id);

    if (data == nullptr) { // id等于gid
        data = new DownloadDataItem();
        QJsonArray ja = json.value("result").toArray();
        QJsonObject jo = ja.at(0).toObject();
        data->totalLength = jo.value("length").toString(); // 文件大小
        data->savePath = jo.value("path").toString(); //下载路径，带文件名
        data->fileName = data->savePath.mid(data->savePath.lastIndexOf('/') + 1);
        QJsonArray uris = jo.value("uris").toArray();
        data->url = uris.at(0).toObject().value("uri").toString(); //下载链接
        data->taskId = id;
        m_DownloadTableView->getTableModel()->append(data);
    }
    m_DownloadTableView->reset();
    m_DownloadTableView->refreshTableView(iCurrentRow);
}

void TableDataControl::aria2MethodUnpause(QJsonObject &json, int iCurrentRow)
{
    QString gId = json.value("result").toString();
    QString taskId = json.value("id").toString();

    DownloadDataItem *data = m_DownloadTableView->getTableModel()->find(taskId);

    if (data != nullptr) {
        data->status = Global::DownloadJobStatus::Active;
        m_DownloadTableView->refreshTableView(iCurrentRow);
    }
}

void TableDataControl::aria2MethodUnpauseAll(QJsonObject &json, int iCurrentRow)
{
    Q_UNUSED(json);
    const QList<Global::DownloadDataItem *> &pItemList = m_DownloadTableView->getTableModel()->dataList();

    foreach (DownloadDataItem *pItem, pItemList) {
        if (pItem->status != Global::DownloadJobStatus::Complete) {
            pItem->status = Global::DownloadJobStatus::Active;
        }
        m_DownloadTableView->refreshTableView(iCurrentRow);
    }
}

void TableDataControl::aria2GetGlobalStatus(QJsonObject &json)
{
    static QList<long long> speedList;
    QJsonObject ja = json.value("result").toObject();
    long long speed = ja.value("downloadSpeed").toString().toLong();
    speedList.append(speed);
    if (speedList.count() >= 5) {
        long long aveSpeed = speedList.at(0) + speedList.at(1) + speedList.at(2) + speedList.at(3) + speedList.at(4);
        aveSpeed /= 5;
        QString speedStr;
        if (Settings::getInstance()->getAutoDownloadBySpeed(speedStr)) {
            if ((aveSpeed / 1024) < speedStr.toInt()) {
                emit addMaxDownloadTask(1);
            }
        }
        speedList.clear();
    }
}

void TableDataControl::aria2MethodRemove(QJsonObject &json)
{
    Q_UNUSED(json);
}

void TableDataControl::aria2MethodForceRemove(QJsonObject &json)
{
    QString id = json.value("id").toString();

    if (id.startsWith("REDOWNLOAD_")) { // 重新下载前的移除完成后
        QStringList sp = id.split("_");
        if (sp.size() >= 3) {
            QString taskId = sp.at(2);
            int rd = sp.at(1).toInt();
            QThread::msleep(100);
            emit RedownloadJob(taskId, rd);
        }
    }
}

void TableDataControl::saveDataBeforeClose()
{
    const QList<DownloadDataItem *> &dataList = m_DownloadTableView->getTableModel()->dataList();
    const QList<DeleteDataItem *> &recyclelist = m_DownloadTableView->getTableModel()->recyleList();

    for (int j = 0; j < recyclelist.size(); j++) {
        DeleteDataItem *pDelData = recyclelist.at(j);
        QDateTime deltime = QDateTime::fromString(pDelData->deleteTime, "yyyy-MM-dd hh:mm:ss");
        TaskInfo task(pDelData->taskId, pDelData->gid, 0, pDelData->url, pDelData->savePath,
                      pDelData->fileName, deltime);

        DBInstance::updateTaskByID(task);
    }
    for (int i = 0; i < dataList.size(); i++) {
        DownloadDataItem *data = dataList.at(i);
        QDateTime time = QDateTime::fromString(data->createTime, "yyyy-MM-dd hh:mm:ss");

        TaskInfo task(data->taskId, data->gid, 0, data->url, data->savePath,
                      data->fileName, time);
        DBInstance::updateTaskByID(task);
        QDateTime finishTime;
        if (data->status == Global::DownloadJobStatus::Complete) {
            finishTime = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
        } else {
            finishTime = QDateTime::currentDateTime();
        }
        TaskStatus getStatus;
        int status;
        if ((data->status == Global::DownloadJobStatus::Complete) || (data->status == Global::DownloadJobStatus::Removed)) {
            status = data->status;
        } else {
            status = Global::DownloadJobStatus::Lastincomplete;
        }

        TaskStatus downloadStatus(data->taskId, status, finishTime, data->completedLength, data->speed,
                                  data->totalLength, data->percent, data->total, finishTime);

        if (DBInstance::getTaskStatusById(data->taskId, getStatus) != false) {
            DBInstance::updateTaskStatusById(downloadStatus);
        } else {
            DBInstance::addTaskStatus(downloadStatus);
        }
    }
}

QString TableDataControl::getFileName(const QString &url)
{
    return QString(url).right(url.length() - url.lastIndexOf('/') - 1);
}

void TableDataControl::dealNotificaitonSettings(QString statusStr, QString fileName, QString errorCode)
{
    bool downloadInfoNotify = Settings::getInstance()->getDownloadInfoSystemNotifyState();
    if (!downloadInfoNotify) {
        return;
    }
    QDBusInterface tInterNotify("com.deepin.dde.Notification",
                                "/com/deepin/dde/Notification",
                                "com.deepin.dde.Notification",
                                QDBusConnection::sessionBus());
    QList<QVariant> arg;
    QString in0(tr("downloader")); //下载器
    uint in1 = 101;
    QString in2;
    in2 = "downloader";
    QString in3;
    QString in4;
    QStringList in5;
    QVariantMap in6;
    if (statusStr == "error") {
        in3 = tr("Download failed"); //下载失败
        in4 = QString(tr("%1 download failed. Network error.")).arg(fileName); //【%1下载失败，网络故障。】
        in5 << "_cancel" << tr("Cancel") << "_view" << tr("View");
        in6["x-deepin-action-_view"] = "Downloader";
        qDebug() << in4 + ("    errorCode: ") + errorCode;
        if ("12" == errorCode) {
        }
    } else {
        in3 = tr("Download complated"); //下载完成
        in4 = QString(tr("%1 download finished.")).arg(fileName); //【【%1下载完成。】
    }

    int in7 = 5000;
    arg << in0 << in1 << in2 << in3 << in4 << in5 << in6 << in7;
    tInterNotify.callWithArgumentList(QDBus::AutoDetect, "Notify", arg);

    //        QString   showInfo;
    //        if(statusStr == "error") {
    //            showInfo = fileName + tr(" download failed, network error");
    //            qDebug() << showInfo + (" errorCode: ") + errorCode;
    //        } else {
    //            showInfo = fileName + tr(" download finished");
    //        }
    //        QProcess *p = new QProcess;
    //        p->start("notify-send", QStringList() << showInfo);
    //        p->waitForStarted();
    //        p->waitForFinished();
}

QString TableDataControl::formatFileSize(long size)
{
    QString result = "";

    if (size < 1024) {
        result = QString::number(size) + "B";
    } else if (size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + "KB";
    } else if (size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + "MB";
    } else if (size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + "GB";
    }

    return result;
}

QString TableDataControl::getDownloadSavepathFromConfig()
{
    return Settings::getInstance()->getDownloadSavePath();
    ;
}

QString TableDataControl::formatDownloadSpeed(long size)
{
    QString result = "";

    if (size < 0) {
        result = QString::number(size) + " KB/s";
    } else if (size < 1024) {
        result = QString::number(size) + " B/s";
    } else if (size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + " KB/s";
    } else if (size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + " MB/s";
    } else if (size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + " GB/s";
    }

    return result;
}

bool TableDataControl::checkFileExist(QString &filePath)
{
    QFileInfo fInfo(filePath);

    return fInfo.exists();
}

void TableDataControl::onUnusualConfirm(int index, const QString &taskId)
{
    DownloadDataItem *pItem = m_DownloadTableView->getTableModel()->find(taskId);
    if (nullptr == pItem) {
        return;
    }

    BtTaskInfo info;
    bool isBttask = false;
    if (pItem->url.isEmpty()) {
        DBInstance::getBtTaskById(taskId, info);
        isBttask = !info.taskId.isEmpty();
    }

    if (0 == index) {
        if (isBttask) {
            QMap<QString, QVariant> opt;
            QString path = pItem->savePath.left(pItem->savePath.lastIndexOf("/"));
            opt.insert("dir", path);
            opt.insert("select-file", info.selectedNum);
            QString fileName = pItem->fileName;
            removeDownloadListJob(pItem, false);
            emit DownloadUnusuaBtJob(info.seedFile, opt, fileName, info.infoHash);
        } else {
            QString url = pItem->url;
            QString savepath = pItem->savePath.left(pItem->savePath.lastIndexOf("/"));
            QMimeDatabase db;
            QString mime = db.suffixForFileName(pItem->fileName);
            QString fileName = pItem->fileName.mid(0, pItem->fileName.lastIndexOf(mime) - 1);
            removeDownloadListJob(pItem, false);
            emit DownloadUnusuaHttpJob(url, savepath, fileName, mime);
        }
    } else {
        removeDownloadListJob(pItem);
    }
}

void TableDataControl::onAria2Remove(QString gId, QString id)
{
}

void TableDataControl::searchEditTextChanged(QString text)
{
    TableModel *pModel = m_DownloadTableView->getTableModel();

    if (text.isEmpty()) {
        for (int i = 0; i < pModel->rowCount(); i++) {
            m_DownloadTableView->setRowHidden(i, false);
            pModel->setData(pModel->index(i, 0), false, TableModel::IsHide);
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    } else {
        for (int i = 0; i < pModel->rowCount(); i++) {
            m_DownloadTableView->setRowHidden(i, false);
            pModel->setData(pModel->index(i, 0), false, TableModel::IsHide);
            QString fileName = pModel->data(pModel->index(i, 1), TableModel::FileName).toString();
            if (!fileName.contains(text, Qt::CaseInsensitive)) {
                m_DownloadTableView->setRowHidden(i, true);
                pModel->setData(pModel->index(i, 0), true, TableModel::IsHide);
            }
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    }
    m_DownloadTableView->reset();
}

int TableDataControl::onDelAction(int currentTab)
{
    int selectedCount = 0;
    if (currentTab == 2) {
        m_RecycleDeleteList.clear();
        const QList<DeleteDataItem *> &pList = m_DownloadTableView->getTableModel()->recyleList();
        for (int i = 0; i < pList.size(); ++i) {
            if ((pList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                m_RecycleDeleteList.append(pList.at(i));
                selectedCount++;
            }
        }
    } else {
        m_DeleteList.clear();
        const QList<DownloadDataItem *> &pSelectList = m_DownloadTableView->getTableModel()->renderList();
        for (int i = 0; i < pSelectList.size(); ++i) {
            DownloadDataItem *data;
            if (currentTab == 1) {
                if (pSelectList.at(i)->status == Complete) {
                    if ((pSelectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_DeleteList.append(data);
                        ++selectedCount;
                    }
                }
            } else {
                if (pSelectList.at(i)->status != Complete) {
                    if ((pSelectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_DeleteList.append(data);
                        ++selectedCount;
                    }
                }
            }
        }
    }
    return selectedCount;
}

int TableDataControl::RedownloadDownloadAndFinishList(QList<Global::DownloadDataItem *> &reloadList)
{
    int selectedCount = 0;

    reloadList.clear();
    const QList<DownloadDataItem *> &selectList = m_DownloadTableView->getTableModel()->renderList();
    for (int i = 0; i < selectList.size(); ++i) {
        if (selectList.at(i)->status == Complete || selectList.at(i)->status == Error) {
            if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                DownloadDataItem *data = selectList.at(i);
                if (data->status == Global::Error) {
                    RedownloadErrorItem(data);
                    return 0;
                }
                reloadList.append(data);
                ++selectedCount;
            }
        }
    }
    return selectedCount;
}

void TableDataControl::RedownloadErrorItem(DownloadDataItem *errorItem)
{
    Aria2RPCInterface::instance()->remove(errorItem->gid);
    DBInstance::delTask(errorItem->taskId);
    emit DownloadUnusuaHttpJob(errorItem->url, errorItem->savePath,
                               errorItem->fileName, errorItem->fileName.split(".").last());

    m_DownloadTableView->getTableModel()->removeItem(errorItem);
}

int TableDataControl::RedownloadTrashList(QList<DeleteDataItem *> &reloadList)
{
    int selectedCount = 0;
    reloadList.clear();
    const QList<DeleteDataItem *> &selectList = m_DownloadTableView->getTableModel()->recyleList();
    for (int i = 0; i < selectList.size(); ++i) {
        if (selectList.at(i)->status == Removed) {
            if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                DeleteDataItem *data = selectList.at(i);
                reloadList.append(data);
                ++selectedCount;
            }
        }
    }
    return selectedCount;
}

void TableDataControl::onReturnOriginAction()
{
}

void TableDataControl::onClearRecyleAction()
{
}

int TableDataControl::onCopyUrlAction(int currentTab, QString &copyUrl)
{
    int selectedCount = 0;
    QString url;

    if (currentTab == 2) {
        const QList<DeleteDataItem *> &recycleSelectList = m_DownloadTableView->getTableModel()->recyleList();
        for (int i = 0; i < recycleSelectList.size(); ++i) {
            DeleteDataItem *data = recycleSelectList.at(i);
            if ((data->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                BtTaskInfo taskInfo;
                DBInstance::getBtTaskById(data->taskId, taskInfo);
                if (!taskInfo.taskId.isEmpty()) {
                    if (taskInfo.downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + taskInfo.infoHash;
                    }
                } else {
                    url = data->url;
                }

                ++selectedCount;
                if (copyUrl.isEmpty()) {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    } else {
        const QList<DownloadDataItem *> &selectList = m_DownloadTableView->getTableModel()->renderList();

        for (int i = 0; i < selectList.size(); ++i) {
            DownloadDataItem *data = nullptr;
            bool isSelect = false;
            if ((currentTab == 1) && (selectList.at(i)->status == Complete)) {
                if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selectedCount;
                    isSelect = true;
                }
            } else {
                if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selectedCount;
                    isSelect = true;
                }
            }
            if (isSelect) {
                BtTaskInfo taskInfo;
                QString url;
                DBInstance::getBtTaskById(data->taskId, taskInfo);
                if (!taskInfo.taskId.isEmpty()) {
                    if (taskInfo.downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + taskInfo.infoHash;
                    }
                } else {
                    url = data->url;
                }
                if (copyUrl.isEmpty()) {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    }
    return selectedCount;
}

int TableDataControl::onDeletePermanentAction(int currentTab)
{
    int selectedCount = 0;

    if (currentTab == 2) {
        m_RecycleDeleteList.clear();
        const QList<DeleteDataItem *> &recycleSelectList = m_DownloadTableView->getTableModel()->recyleList();
        for (int i = 0; i < recycleSelectList.size(); ++i) {
            if ((recycleSelectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                m_RecycleDeleteList.append(recycleSelectList.at(i));
                selectedCount++;
            }
        }
        if (m_DownloadTableView->getTableModel()->recyleList().isEmpty()) {
            m_DownloadTableView->getTableHeader()->onHeaderChecked(false);
        }
    } else {
        m_DeleteList.clear();
        const QList<DownloadDataItem *> &selectList = m_DownloadTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); ++i) {
            DownloadDataItem *data;
            if (currentTab == 1) {
                if (selectList.at(i)->status == Complete) {
                    if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_DeleteList.append(data);
                        ++selectedCount;
                    }
                }
            } else {
                if (selectList.at(i)->status != Complete) {
                    if ((selectList.at(i)->Ischecked == 1) && !m_DownloadTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_DeleteList.append(data);
                        ++selectedCount;
                    }
                }
            }
        }
    }

    if (m_DownloadTableView->getTableModel()->recyleList().isEmpty()) {
        m_DownloadTableView->getTableHeader()->onHeaderChecked(false);
    }
    return selectedCount;
}

void TableDataControl::onDeleteDownloadListConfirm(bool ischecked, bool permanent, TableView *pRecycleTableView)
{
    QString gid;
    QString ariaTempFile;
    QString savePath;
    QString taskId;
    bool ifDeleteLocal = permanent || ischecked;

    QList<DownloadDataItem> threadDeleteList;
    for (int i = 0; i < m_DeleteList.size(); i++) {
        DownloadDataItem tempdata;
        tempdata.status = m_DeleteList.at(i)->status;
        tempdata.percent = m_DeleteList.at(i)->percent;
        tempdata.total = m_DeleteList.at(i)->total;
        tempdata.Ischecked = m_DeleteList.at(i)->Ischecked;
        tempdata.taskId = m_DeleteList.at(i)->taskId;
        tempdata.fileName = m_DeleteList.at(i)->fileName;
        tempdata.completedLength = m_DeleteList.at(i)->completedLength;
        tempdata.totalLength = m_DeleteList.at(i)->totalLength;
        tempdata.savePath = m_DeleteList.at(i)->savePath;
        tempdata.speed = m_DeleteList.at(i)->speed;
        tempdata.gid = m_DeleteList.at(i)->gid;
        tempdata.url = m_DeleteList.at(i)->url;
        tempdata.time = m_DeleteList.at(i)->time;
        tempdata.createTime = m_DeleteList.at(i)->createTime;
        threadDeleteList.append(tempdata);
    }

    DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadDeleteList,
                                                               m_DownloadTableView,
                                                               ifDeleteLocal,
                                                               "download_delete");
    //connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, this, &tableDataControl::Aria2RemoveSlot);

    qDebug() << "subThread: " << QThread::currentThreadId();
    connect(pDeleteItemThread, &DeleteItemThread::Aria2Remove, this, [](QString gId, QString id) {
        qDebug() << "subThread: " << QThread::currentThreadId();
        Aria2RPCInterface::instance()->forceRemove(gId, id);
    });
    connect(pDeleteItemThread, &DeleteItemThread::removeFinished, this, &TableDataControl::removeFinished);
    pDeleteItemThread->start();
    //pDeleteItemThread->deleteLater();

    for (int i = 0; i < m_DeleteList.size(); i++) {
        DownloadDataItem *data = m_DeleteList.at(i);
        savePath = data->savePath;
        gid = data->gid;
        taskId = data->taskId;
        QDateTime finishTime;
        if (data->status == Complete) {
            finishTime = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
        } else {
            finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        }

        TaskStatus getStatus;
        TaskStatus downloadStatus(data->taskId,
                                  Global::DownloadJobStatus::Removed,
                                  QDateTime::currentDateTime(),
                                  data->completedLength,
                                  data->speed,
                                  data->totalLength,
                                  data->percent,
                                  data->total,
                                  finishTime);

        if (permanent || ischecked) {
            BtTaskInfo info;
            DBInstance::getBtTaskById(taskId, info);
            if (info.downloadType == "torrent") {
                clearShardMemary();
            }
            DBInstance::delTask(taskId);
            Aria2RPCInterface::instance()->purgeDownloadResult(data->gid);
        }

        if (!permanent && !ischecked) {
            DeleteDataItem *delData = new DeleteDataItem;
            delData->taskId = data->taskId;
            delData->gid = data->gid;
            delData->url = data->url;
            delData->fileName = data->fileName;
            delData->savePath = data->savePath;
            delData->status = DownloadJobStatus::Removed;
            delData->Ischecked = false;
            delData->totalLength = data->totalLength;
            delData->completedLength = data->completedLength;
            delData->deleteTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            delData->finishTime = data->time;

            pRecycleTableView->getTableModel()->append(delData);
            m_DownloadTableView->update();

            if (DBInstance::getTaskStatusById(delData->taskId, getStatus)) {
                DBInstance::updateTaskStatusById(downloadStatus);
            } else {
                DBInstance::addTaskStatus(downloadStatus);
            }
        }

        m_DownloadTableView->getTableModel()->removeItem(data);
        QThread::usleep(10);
    }
    if (m_DownloadTableView->getTableModel()->recyleList().isEmpty()) {
        m_DownloadTableView->getTableHeader()->onHeaderChecked(false);
    }
}

void TableDataControl::onDeleteRecycleListConfirm(bool ischecked, bool permanent)
{
    bool ifDeleteLocal = permanent || ischecked;
    QList<DeleteDataItem> threadRecycleDeleteList;
    for (int i = 0; i < m_RecycleDeleteList.size(); i++) {
        DeleteDataItem tempdata;
        tempdata.status = m_RecycleDeleteList.at(i)->status;
        tempdata.Ischecked = m_RecycleDeleteList.at(i)->Ischecked;
        tempdata.taskId = m_RecycleDeleteList.at(i)->taskId;
        tempdata.fileName = m_RecycleDeleteList.at(i)->fileName;
        tempdata.completedLength = m_RecycleDeleteList.at(i)->completedLength;
        tempdata.savePath = m_RecycleDeleteList.at(i)->savePath;
        tempdata.gid = m_RecycleDeleteList.at(i)->gid;
        tempdata.url = m_RecycleDeleteList.at(i)->url;
        tempdata.totalLength = m_RecycleDeleteList.at(i)->totalLength;
        tempdata.deleteTime = m_RecycleDeleteList.at(i)->deleteTime;
        tempdata.finishTime = m_RecycleDeleteList.at(i)->finishTime;
        threadRecycleDeleteList.append(tempdata);
    }
    DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadRecycleDeleteList,
                                                               m_DownloadTableView,
                                                               ifDeleteLocal,
                                                               "recycle_delete");
    connect(pDeleteItemThread, &DeleteItemThread::Aria2Remove, [=](QString gId, QString id) {
        Aria2RPCInterface::instance()->forceRemove(gId, id);
    });
    connect(pDeleteItemThread, &DeleteItemThread::removeFinished, this, &TableDataControl::removeFinished);
    pDeleteItemThread->start();

    for (int i = 0; i < m_RecycleDeleteList.size(); i++) {
        DeleteDataItem *data = m_RecycleDeleteList.at(i);
        DBInstance::delTask(data->taskId);
        m_DownloadTableView->getTableModel()->removeItem(data);
    }
    if (m_DownloadTableView->getTableModel()->recyleList().isEmpty()) {
        m_DownloadTableView->getTableHeader()->onHeaderChecked(false);
    }
}

void TableDataControl::downloadListRedownload(QString id)
{
    DownloadDataItem *data = m_DownloadTableView->getTableModel()->find(id);
    if (data == nullptr) {
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

    if (defaultSavepath != data->savePath) {
        int nameLength = fileName.size();
        int filePathLength = filePath.size();
        int folderPathLength = filePathLength - nameLength - 1;
        savePath = filePath.left(folderPathLength);
    } else {
        savePath = defaultSavepath;
    }

    //m_pTableView->getTableModel()->removeItem(data);
    //DBInstance::delTask(taskId);

    BtTaskInfo taskInfo;
    DBInstance::getBtTaskById(taskId, taskInfo);
    if (!taskInfo.taskId.isEmpty()) {
        if (taskInfo.downloadType == "torrent") {
            QMap<QString, QVariant> opt;
            opt.insert("dir", savePath);
            opt.insert("select-file", taskInfo.selectedNum);

            // aria2c->addTorrent(get_UrlInfo->seedFile,opt,get_UrlInfo->task_id);
            TaskInfo addTask(taskInfo.taskId,
                             "",
                             0,
                             "",
                             "",
                             fileName,
                             QDateTime::currentDateTime());
            DBInstance::addTask(addTask);
            Aria2RPCInterface::instance()->addTorrent(taskInfo.seedFile, opt, taskInfo.taskId);
        }
    } else {
        QUuid uuid = QUuid::createUuid();
        QString strId = uuid.toString();
        QMap<QString, QVariant> opt;
        //        int count = DBInstance::getSameNameCount(data->fileName);
        //        QString outFileName;
        //        if (count > 0) {
        //            QString name1 = data->fileName.mid(0, fileName.lastIndexOf('.'));
        //            name1 += QString("_%1").arg(count);
        //            outFileName = name1 + fileName.mid(fileName.lastIndexOf('.'), fileName.length());
        //        }
        opt.insert("dir", savePath);
        opt.insert("out", data->fileName);
        Aria2RPCInterface::instance()->addUri(url, opt, strId);

        QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
        if (!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            const QByteArray filenameByte = filename.toLatin1();
            QString filenameDecode = QUrl::fromPercentEncoding(filenameByte);
            filename = filenameDecode;
        }

        TaskInfo addTask(strId, "", 0, url, filePath, filename, QDateTime::currentDateTime());
        DBInstance::addTask(addTask);
    }
}

void TableDataControl::recycleListRedownload(QString id)
{
    DeleteDataItem *data = m_DownloadTableView->getTableModel()->find(id, 2);
    if (data == nullptr) {
        return;
    }
    QString url = data->url;
    QString taskId = data->taskId;
    QString ariaTempFile = data->savePath + ".aria2";
    //    if(!data->savePath.isEmpty()) {
    //        QFileInfo fi(data->savePath);
    //        if(fi.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
    //            QDir tar(data->savePath);
    //            tar.removeRecursively();
    //        } else {
    //            QFile::remove(data->savePath);
    //        }
    //        if(QFile::exists(ariaTempFile)) {
    //            QFile::remove(ariaTempFile);
    //        }
    //    }
    QString filePath = data->savePath;
    QString fileName = data->fileName;
    QString defaultSavepath = getDownloadSavepathFromConfig();
    QString savePath;
    if (defaultSavepath != data->savePath) {
        int nameLength = fileName.size();
        int filePathLength = filePath.size();
        int folderPathLength = filePathLength - nameLength - 1;
        savePath = filePath.left(folderPathLength);
    } else {
        savePath = defaultSavepath;
    }

    // m_pTableView->getTableModel()->removeItem(data);

    // DBInstance::delTask(taskId);
    QUuid uuid = QUuid::createUuid();
    QString strId = uuid.toString();
    BtTaskInfo taskInfo;
    DBInstance::getBtTaskById(taskId, taskInfo);
    if (!taskInfo.taskId.isEmpty()) {
        if (taskInfo.downloadType == "torrent") {
            if (!taskInfo.infoHash.isEmpty()) {
                QFile::remove(taskInfo.infoHash + ".torrent");
            }
            QMap<QString, QVariant> opt;
            opt.insert("dir", savePath);
            opt.insert("select-file", taskInfo.selectedNum);

            // aria2c->addTorrent(get_UrlInfo->seedFile,opt,get_UrlInfo->task_id);

            TaskInfo addTask(taskInfo.taskId,
                             "",
                             0,
                             "",
                             "",
                             fileName,
                             QDateTime::currentDateTime());

            DBInstance::addTask(addTask);
            Aria2RPCInterface::instance()->addTorrent(taskInfo.seedFile, opt, taskInfo.taskId);
        }
    } else {
        QMap<QString, QVariant> opt;
        //        int count = DBInstance::getSameNameCount(data->fileName);
        //        QString outFileName;
        //        if (count > 0) {
        //            QString name1 = data->fileName.mid(0, fileName.lastIndexOf('.'));
        //            name1 += QString("_%1").arg(count);
        //            outFileName = name1 + fileName.mid(fileName.lastIndexOf('.'), fileName.length());
        //        }
        opt.insert("dir", savePath);
        opt.insert("out", data->fileName);
        Aria2RPCInterface::instance()->addUri(url, opt, strId);
        QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
        if (!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            const QByteArray filenameByte = filename.toLatin1();
            QString filenameDecode = QUrl::fromPercentEncoding(filenameByte);
            filename = filenameDecode;
        }
        TaskInfo addTask(strId, "", 0, url, filePath, filename, QDateTime::currentDateTime());
        DBInstance::addTask(addTask);
    }
}

void TableDataControl::clearShardMemary()
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey("downloader");
    if (sharedMemory.attach()) //设置成单例程序
    {
        sharedMemory.lock();
        char *to = static_cast<char *>(sharedMemory.data());
        int num = sharedMemory.size();
        memset(to, 0, num);
        sharedMemory.unlock();
    }
}

bool TableDataControl::checkTaskStatus()
{
    const QList<DownloadDataItem *> &dataList = m_DownloadTableView->getTableModel()->dataList();
    for (const auto *item : dataList) {
        if ((item->status == Global::DownloadJobStatus::Active) || (item->status == Global::DownloadJobStatus::Waiting)) {
            return true;
        }
    }
    return false;
}
