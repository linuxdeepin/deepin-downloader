// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <QTimer>

#include "../database/dbinstance.h"

DeleteItemThread::DeleteItemThread()
{
    qDebug() << "DeleteItemThread default constructor called";
}

DeleteItemThread::DeleteItemThread(QList<DeleteDataItem*> &recycleDeleteList,
                                   TableView *recycleTableview,
                                   bool ifDeleteLocal,
                                   QString deleteType)
{
    qDebug() << "DeleteItemThread constructor called with recycle list size:" << recycleDeleteList.size() 
             << "delete local:" << ifDeleteLocal << "delete type:" << deleteType;
    m_RecycleDeleteList = recycleDeleteList;
    m_RecycleTableview = recycleTableview;
    m_IfDeleteLocal = ifDeleteLocal;
    m_StrDeleteType = deleteType;
    qDebug() << "[DeleteItemThread] Recycle constructor ended";
}

DeleteItemThread::DeleteItemThread(QList<DownloadDataItem*> &deleteList,
                                   TableView *downloadingTableview,
                                   bool ifDeleteLocal,
                                   QString deleteType)
{
    qDebug() << "DeleteItemThread constructor called with download list size:" << deleteList.size()
             << "delete local:" << ifDeleteLocal << "delete type:" << deleteType;
    m_DeleteList = deleteList;
    m_DownloadingTableview = downloadingTableview;
    m_IfDeleteLocal = ifDeleteLocal;
    m_StrDeleteType = deleteType;
    qDebug() << "[DeleteItemThread] Download constructor ended";
}

void DeleteItemThread::deleteRecycleData()
{
    qDebug() << "Starting to delete recycle data, count:" << m_RecycleDeleteList.size();
    if (m_IfDeleteLocal) {
        qDebug() << "[DeleteItemThread] Deleting local files";
        for (int i = 0; i < m_RecycleDeleteList.size(); i++) {
            QString savePath = m_RecycleDeleteList.at(i)->savePath;
            QString gid = m_RecycleDeleteList.at(i)->gid;
            QString taskId = m_RecycleDeleteList.at(i)->taskId;
            QString filename = m_RecycleDeleteList.at(i)->fileName;
            if (!savePath.isEmpty()) {
                QFileInfo fileinfo(savePath);
                if (fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    // qDebug() << "[DeleteItemThread] Deleting directory recursively:" << savePath;
                    QDir tar(m_RecycleDeleteList.at(i)->savePath);
                    tar.removeRecursively();
                    if (QFile::exists(savePath + ".aria2")) {
                        // qDebug() << "[DeleteItemThread] Removing aria2 temp file:" << savePath + ".aria2";
                        QFile::remove(savePath + ".aria2");
                        QTimer::singleShot(3000, [=]() {
                            QFile::remove(savePath + ".aria2");
                        });
                    }
                } else {
                    // qDebug() << "[DeleteItemThread] Deleting single file:" << savePath;
                    QString ariaTempFile = savePath + ".aria2";
                    if (!savePath.isEmpty()) {
                        if (m_RecycleDeleteList.at(i)->url.isEmpty()) { //bt任务
                            // qDebug() << "[DeleteItemThread] Processing BT task deletion";
                            TaskInfoHash info;
                            DBInstance::getBtTaskById(m_RecycleDeleteList.at(i)->taskId, info);
                            QString torrentPath = info.filePath;
                            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
                            QString mode = btInfo.mode;
                            if (m_RecycleDeleteList.at(i)->savePath.contains(btInfo.name)) {
                                // qDebug() << "[DeleteItemThread] Deleting BT task directory:" << m_RecycleDeleteList.at(i)->savePath;
                                deleteDirectory(m_RecycleDeleteList.at(i)->savePath);
                            }
                        } else {
                            // qDebug() << "[DeleteItemThread] Processing regular task deletion";
                            deleteDirectory(m_RecycleDeleteList.at(i)->savePath);
                        }
                        if (QFile::exists(ariaTempFile)) {
                            QFile::remove(savePath + ".aria2");
                            QTimer::singleShot(3000, [=]() {
                                QFile::remove(savePath + ".aria2");
                            });
                        }
                    }
                }
            }
        }
    }
    emit removeFinished();
    qDebug() << "[DeleteItemThread] deleteRecycleData function ended";
}

void DeleteItemThread::deleteDownloadData()
{
    qDebug() << "Starting to delete download data, count:" << m_DeleteList.size();
    for (int i = 0; i < m_DeleteList.size(); ++i) {
        QString gid = m_DeleteList.at(i)->gid;
        QString taskId = m_DeleteList.at(i)->taskId;
        QString savePath = m_DeleteList.at(i)->savePath;
        QString filename = m_DeleteList.at(i)->fileName;
        emit Aria2Remove(m_DeleteList.at(i)->gid, "");
        if (m_IfDeleteLocal) {
            Aria2RPCInterface::instance()->pause(gid, taskId);

            if (!savePath.isEmpty()) {
                QFileInfo fileinfo(savePath);
                if (fileinfo.isDir() && savePath.contains(filename) && !filename.isEmpty()) {
                    // qDebug() << "[DeleteItemThread] Deleting download directory:" << savePath;
                    if (m_DeleteList.at(i)->url.isEmpty()) { //bt任务
                        // qDebug() << "[DeleteItemThread] Processing BT download task deletion";
                        TaskInfoHash info;
                        DBInstance::getBtTaskById(m_DeleteList.at(i)->taskId, info);
                        QString torrentPath = info.filePath;
                        Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
                        QString mode = btInfo.mode;
                        if (m_DeleteList.at(i)->savePath.contains(btInfo.name)) {
                            // qDebug() << "[DeleteItemThread] Deleting BT download directory:" << m_DeleteList.at(i)->savePath;
                            deleteDirectory(m_DeleteList.at(i)->savePath);
                        }
                    } else {
                        // qDebug() << "[DeleteItemThread] Processing regular download task deletion";
                        deleteDirectory(m_DeleteList.at(i)->savePath);
                    }
                    if (QFile::exists(savePath + ".aria2")) {
                        QFile::remove(savePath + ".aria2");
                        QTimer::singleShot(3000, [=]() {
                            QFile::remove(savePath + ".aria2");
                            if (QFileInfo::exists(savePath)) {
                                deleteDirectory(savePath);
                            }
                        });
                    }
                } else {
                    // qDebug() << "[DeleteItemThread] Deleting single download file:" << savePath;
                    QString ariaTempFile = savePath + ".aria2";
                    if (!savePath.isEmpty()) {
                        if (m_DeleteList.at(i)->url.isEmpty()) { //bt任务
                            // qDebug() << "[DeleteItemThread] Processing BT single file deletion";
                            TaskInfoHash info;
                            DBInstance::getBtTaskById(m_DeleteList.at(i)->taskId, info);
                            QString torrentPath = info.filePath;
                            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
                            QString mode = btInfo.mode;
                            if (m_DeleteList.at(i)->savePath.contains(btInfo.name)) {
                                // qDebug() << "[DeleteItemThread] Deleting BT single file:" << m_DeleteList.at(i)->savePath;
                                deleteDirectory(m_DeleteList.at(i)->savePath);
                            }
                        } else {
                            // qDebug() << "[DeleteItemThread] Processing regular single file deletion";
                            deleteDirectory(m_DeleteList.at(i)->savePath);
                        }
                        if (QFile::exists(ariaTempFile)) {
                            QFile::remove(savePath + ".aria2");
                            QTimer::singleShot(3000, [=]() {
                                QFile::remove(savePath + ".aria2");
                            });
                        }
                    }
                }
            }
        }
    }
    emit removeFinished();
    qDebug() << "[DeleteItemThread] deleteDownloadData function ended";
}

void DeleteItemThread::run()
{
    qDebug() << "Delete thread started, delete type:" << m_StrDeleteType;
    if (m_StrDeleteType == "recycle_delete") {
        qDebug() << "[DeleteItemThread] Executing recycle delete operation";
        deleteRecycleData();
    }
    if (m_StrDeleteType == "download_delete") {
        qDebug() << "[DeleteItemThread] Executing download delete operation";
        deleteDownloadData();
    }
    QEventLoop loop;
    QTimer::singleShot(5000, &loop, SLOT(quit()));
    loop.exec();
    qDebug() << "Delete thread finished";
}

bool DeleteItemThread::deleteDirectory(const QString &path)
{
    qDebug() << "Attempting to delete directory:" << path;
    QFileInfo info(path);
    if (info.isFile()) {
        qDebug() << "[DeleteItemThread] Deleting file:" << path;
        QFile::remove(path);
        return true;
    }
    if (path.isEmpty()) {
        qWarning() << "Empty path provided for deletion";
        return false;
    }

    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "Directory" << path << "does not exist, nothing to delete";
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    for (QFileInfo fi : fileList) {
        if (fi.isFile()) {
            // qDebug() << "[DeleteItemThread] Deleting file in directory:" << fi.fileName();
            fi.dir().remove(fi.fileName());
        } else {
            // qDebug() << "[DeleteItemThread] Recursively deleting subdirectory:" << fi.absoluteFilePath();
            deleteDirectory(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}
