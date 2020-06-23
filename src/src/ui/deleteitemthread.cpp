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

DeleteItemThread::DeleteItemThread(QList<DelDataItem> recycleDeleteList,
                                   TableView         *recycleTableview,
                                   Aria2RPCInterface *aria2c,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    m_pRecycleDeleteList = recycleDeleteList;
    m_pRecycleTableview = recycleTableview;
    m_pAria2c = aria2c;
    m_bIfDeleteLocal = ifDeleteLocal;
    m_StrDeleteType = deleteType;
}

DeleteItemThread::DeleteItemThread(QList<DataItem>    deleteList,
                                   TableView         *downloadingTableview,
                                   Aria2RPCInterface *aria2c,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    m_DeleteList = deleteList;
    m_pDownloadingTableview = downloadingTableview;
    m_pAria2c = aria2c;
    m_bIfDeleteLocal = ifDeleteLocal;
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
    if(m_bIfDeleteLocal) {
        for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
            QString savePath = m_pRecycleDeleteList.at(i).savePath;
            QString gid = m_pRecycleDeleteList.at(i).gid;
            QString taskId = m_pRecycleDeleteList.at(i).taskId;
            QString filename = m_pRecycleDeleteList.at(i).fileName;
            if(savePath != "") {
                QFileInfo fileinfo(savePath);
                if(fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    QDir tar(m_pRecycleDeleteList.at(i).savePath);
                    tar.removeRecursively();
                } else {
                    QString ariaTempFile = savePath + ".aria2";
                    if(!savePath.isEmpty()) {
                        QFile::remove(savePath);
                        if(QFile::exists(ariaTempFile)) {
                            QThread::sleep(1);
                            QFile::remove(ariaTempFile);
                        }
                    }
                }
            }
        }
    }
}

void DeleteItemThread::deleteDownloadData()
{
    for(int i = 0; i < m_DeleteList.size(); ++i) {
        QString gid = m_DeleteList.at(i).gid;
        QString taskId = m_DeleteList.at(i).taskId;
        QString savePath = m_DeleteList.at(i).savePath;
        QString filename = m_DeleteList.at(i).fileName;
        //emit    signalAria2Remove(m_DeleteList.at(i).gid, m_DeleteList.at(i).taskId);
        Aria2RPCInterface::Instance()->remove(m_DeleteList.at(i).gid, "");
        if(m_bIfDeleteLocal) {
            m_pAria2c->pause(gid, taskId);

            if(savePath != "") {
                QFileInfo fileinfo(savePath);
                if(fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    QDir tar(savePath);
                    tar.removeRecursively();
                } else {
                    QString ariaTempFile = savePath + ".aria2";
                    if(!savePath.isEmpty()) {
                        QFile::remove(savePath);
                        if(QFile::exists(ariaTempFile)) {
                            QThread::sleep(1);
                            QFile::remove(ariaTempFile);
                        }
                    }
                }
            }
        }
    }
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
