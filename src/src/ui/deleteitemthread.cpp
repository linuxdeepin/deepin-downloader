/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file deleteitemthread.cpp
 *
 * @brief 删除任务类，是一个独立线程
 *
 * @date 2020-06-09 10:01
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

#include "deleteitemthread.h"

#include <QDebug>
#include <QFile>
#include <QDir>

DeleteItemThread::DeleteItemThread()
{
}

DeleteItemThread::DeleteItemThread(QList<DeleteDataItem> recycleDeleteList,
                                   TableView         *recycleTableview,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    m_RecycleDeleteList = recycleDeleteList;
    m_RecycleTableview = recycleTableview;
    m_IfDeleteLocal = ifDeleteLocal;
    m_StrDeleteType = deleteType;
}

DeleteItemThread::DeleteItemThread(QList<DownloadDataItem>    deleteList,
                                   TableView         *downloadingTableview,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    m_DeleteList = deleteList;
    m_DownloadingTableview = downloadingTableview;
    m_IfDeleteLocal = ifDeleteLocal;
    m_StrDeleteType = deleteType;
}

//删除文件夹
bool DeleteItemThread::DelDir(const QString &path)
{
    if(path.isEmpty()) {
        return false;
    }
    QDir dir(path);
    if(!dir.exists()) {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); // 设置过滤
    QFileInfoList fileList = dir.entryInfoList();           // 获取所有的文件信息
    foreach(QFileInfo file, fileList) {                     // 遍历文件信息
        if(file.isFile()) {                                 // 是文件，删除
            file.dir().remove(file.fileName());
        } else {  // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

void DeleteItemThread::deleteRecycleData()
{
    if(m_IfDeleteLocal) {
        for(int i = 0; i < m_RecycleDeleteList.size(); i++) {
            QString savePath = m_RecycleDeleteList.at(i).savePath;
            QString gid = m_RecycleDeleteList.at(i).gid;
            QString taskId = m_RecycleDeleteList.at(i).taskId;
            QString filename = m_RecycleDeleteList.at(i).fileName;
            if(!savePath.isEmpty()) {
                QFileInfo fileinfo(savePath);
                if(fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    QDir tar(m_RecycleDeleteList.at(i).savePath);
                    tar.removeRecursively();
                    if(QFile::exists(savePath + ".aria2")) {
                        QThread::msleep(200);
                        QFile::remove(savePath + ".aria2");
                    }
                } else {
                    QString ariaTempFile = savePath + ".aria2";
                    if(!savePath.isEmpty()) {
                        deleteDirectory(savePath);
                        if(QFile::exists(ariaTempFile)) {
                            QThread::msleep(200);
                            QFile::remove(ariaTempFile);
                        }
                    }
                }
            }
        }
    }
    emit removeFinished();
}

void DeleteItemThread::deleteDownloadData()
{
    for(int i = 0; i < m_DeleteList.size(); ++i) {
        QString gid = m_DeleteList.at(i).gid;
        QString taskId = m_DeleteList.at(i).taskId;
        QString savePath = m_DeleteList.at(i).savePath;
        QString filename = m_DeleteList.at(i).fileName;
        emit    Aria2Remove(m_DeleteList.at(i).gid, "");
        if(m_IfDeleteLocal) {
            Aria2RPCInterface::instance()->pause(gid, taskId);

            if(!savePath.isEmpty()) {
                QFileInfo fileinfo(savePath);
                if(fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    QDir tar(savePath);
                    tar.removeRecursively();
                    if(QFile::exists(savePath + ".aria2")) {
                        QThread::msleep(200);
                        QFile::remove(savePath + ".aria2");
                    }
                } else {
                    QString ariaTempFile = savePath + ".aria2";
                    if(!savePath.isEmpty()) {
                        deleteDirectory(savePath);
                        if(QFile::exists(ariaTempFile)) {
                            QThread::msleep(200);
                            QFile::remove(ariaTempFile);
                        }
                    }
                }
            }
        }
    }
    emit removeFinished();
}

void DeleteItemThread::run()
{
    if(m_StrDeleteType == "recycle_delete") {
        deleteRecycleData();
    }
    if(m_StrDeleteType == "download_delete") {
        deleteDownloadData();
    }
}

bool DeleteItemThread::deleteDirectory(const QString &path)
{
    QFileInfo info(path);
    if(info.isFile()){
        QFile::remove(path);
        return true;
    }
    if (path.isEmpty())
    {
        return false;
    }

    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
        {
            fi.dir().remove(fi.fileName());
        }
        else
        {
            deleteDirectory(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}
