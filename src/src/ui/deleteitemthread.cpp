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

DeleteItemThread::DeleteItemThread(QList<DelDataItem*> recycleDeleteList,
                                   TableView         *recycleTableview,
                                   Aria2RPCInterface *aria2c,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    this->m_pRecycleDeleteList = recycleDeleteList;
    this->m_pRecycleTableview = recycleTableview;
    this->m_pAria2c = aria2c;
    this->m_bIfDeleteLocal = ifDeleteLocal;
    this->m_StrDeleteType = deleteType;
}

DeleteItemThread::DeleteItemThread(QList<DataItem*>    deleteList,
                                   TableView         *downloadingTableview,
                                   Aria2RPCInterface *aria2c,
                                   bool               ifDeleteLocal,
                                   QString            deleteType)
{
    this->m_DeleteList = deleteList;
    this->m_pDownloadingTableview = downloadingTableview;
    this->m_pAria2c = aria2c;
    this->m_bIfDeleteLocal = ifDeleteLocal;
    this->m_StrDeleteType = deleteType;
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
    for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
        QString save_path = m_pRecycleDeleteList.at(i)->savePath;
        QString gid = m_pRecycleDeleteList.at(i)->gid;
        QString taskId = m_pRecycleDeleteList.at(i)->taskId;
        QString filename = m_pRecycleDeleteList.at(i)->fileName;


        if(m_bIfDeleteLocal) {
            if(save_path != "") {
                QFileInfo fileinfo(save_path);
                if(fileinfo.isDir() && save_path.contains(filename) && !filename.isEmpty()) {
                    QDir tar(m_pRecycleDeleteList.at(i)->savePath);
                    tar.removeRecursively();
                } else {
                    QString aria_temp_file = save_path + ".aria2";
                    if(!save_path.isEmpty()) {
                        QFile::remove(save_path);
                        if(QFile::exists(aria_temp_file)) {
                            QThread::sleep(1);
                            QFile::remove(aria_temp_file);
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
        QString gid = m_DeleteList.at(i)->gid;
        QString task_id = m_DeleteList.at(i)->taskId;
        QString save_path = m_DeleteList.at(i)->savePath;
        QString filename = m_DeleteList.at(i)->fileName;
        emit    signalAria2Remove(m_DeleteList.at(i)->gid, "");

        if(m_bIfDeleteLocal) {
            m_pAria2c->pause(gid, task_id);

            if(save_path != "") {
                QFileInfo fileinfo(save_path);
                if(fileinfo.isDir() && save_path.contains(filename) && !filename.isEmpty()) {
                    QDir tar(save_path);
                    tar.removeRecursively();
                } else {
                    QString aria_temp_file = save_path + ".aria2";
                    if(!save_path.isEmpty()) {
                        QFile::remove(save_path);
                        if(QFile::exists(aria_temp_file)) {
                            QThread::sleep(1);
                            QFile::remove(aria_temp_file);
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
