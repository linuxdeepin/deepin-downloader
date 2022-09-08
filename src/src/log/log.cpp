// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2020-2020} Uniontech Technology Co., Ltd.
 *
 * @file log.cpp
 *
 * @brief 日志 接口类
 *
 * @date 2020-05-26 11:55
 *
 * Author: denglinglong  <denglinglong@uniontech.com>
 *
 * Maintainer: denglinglong  <denglinglong@uniontech.com>
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

#include "log.h"
#include <iostream>
static int s_logLevel = QtDebugMsg;
static quint64 _logDaysRemain = DEFALT_REMAIN_TIME;
static int _rotateSize = MAXLOGSIZE;
static quint64 _remainDisk = DEFALT_REMAIN_SIZE;

void setLogPath(const QString &path)
{
    s_logPath = path;
}
void setLogDir(const QString &dir)
{
    _logDir = dir;
}
void setLogLevel(int level)
{
    s_logLevel = level;
}

bool static ensureDirExist(const QString &dirPath)
{
    QDir dir(dirPath);
    if (dir.exists()) {
        return true;
    }

    return dir.mkpath(dirPath);
}
QFileInfoList GetLogList()
{
    QStringList nameFilter;
    nameFilter << "*.log";
    QDir logDir(_logDir);
    auto logList = logDir.entryInfoList(nameFilter, QDir::Files, QDir::Name);
    return logList;
}
void CheckLogTime()
{
    auto curTime = QDateTime::currentDateTime();
    auto logList = GetLogList();
    for (int i = 0; i < logList.size() - 1; ++i) {
        auto logInfo = logList[i];
        auto createdTime = logInfo.created();
        auto elapseDays = createdTime.daysTo(curTime);
        if (elapseDays > _logDaysRemain) {
            auto logPath = logInfo.absoluteFilePath();
            QDir dir;
            dir.remove(logPath);
        }
    }
}
quint64 GetDiskFreeSpace()
{
    QStorageInfo storage = QStorageInfo::root();
    storage.refresh(); //获得最新磁盘信息

    if (storage.isReadOnly())
        qDebug() << "isReadOnly:" << storage.isReadOnly();
    else {
        qDebug() << "name:" << storage.name();
        qDebug() << "fileSystemType:" << storage.fileSystemType();
        qDebug() << "size:" << storage.bytesTotal() / 1000 / 1000 << "MB";
        qDebug() << "availableSize:" << storage.bytesAvailable() / 1000 / 1000 << "MB";
    }
    return (quint64)storage.bytesAvailable();
}
void CheckFreeDisk()
{
    auto freeSpace = GetDiskFreeSpace();

    if (freeSpace < _remainDisk) {
        auto logList = GetLogList();
        for (int i = 0; i < logList.size() - 1; ++i) {
            QDir dir;
            dir.remove(logList[i].absolutePath());
            freeSpace = GetDiskFreeSpace();
            if (freeSpace > _remainDisk) {
                break;
            }
        }
    }
}
bool CheckRotateSize()
{
    bool ret = _logFile.size() >= _rotateSize;
    return ret;
}
bool CheckRotateTimePoint()
{
    QFileInfo curLogInfo(_logFile);
    auto curLogCreateDate = curLogInfo.created();
    auto curDate = QDateTime::currentDateTime();
    auto dayElapse = curLogCreateDate.daysTo(curDate);
    if (dayElapse >= 1) {
        return true;
    }

    return false;
}
void CloseLog()
{
    _logFile.close();
}

void WriteVersion()
{
    if (CheckRotateSize() || CheckRotateTimePoint()) {
        CloseLog();
        CreateNewLog();
    }
    s_logMutex.lock();
    QFile outFile(s_logPath);
    QFileInfo fileInfo(outFile);
    if (!ensureDirExist(fileInfo.absoluteDir().absolutePath()))
        return;

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream ts(&outFile);
    auto appName = QCoreApplication::applicationName();
    auto version = QCoreApplication::applicationVersion();
    ts << appName << " " << version << endl;
    outFile.close();
    s_logMutex.unlock();
}
void CreateNewLog()
{
    auto appName = QCoreApplication::applicationName();
    auto version = QCoreApplication::applicationVersion();
    auto curTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz");
    auto logName = _logDir + appName + "_" + curTime + ".log";
    _logFile.setFileName(logName);
    setLogPath(logName);
    _logFile.open(QIODevice::ReadWrite | QIODevice::Append);
    //日志头写入App\系统参数
    WriteVersion();
}

void customLogMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    if (type < s_logLevel) {
        return;
    }

    QString logInfo;
    switch (type) {
    case QtDebugMsg:
        logInfo = QString("Debug:");
        break;
    case QtWarningMsg:
        logInfo = QString("Warning:");
        break;
    case QtCriticalMsg:
        logInfo = QString("Critical:");
        break;
    case QtFatalMsg:
        logInfo = QString("Fatal:");
    }
    QString contextInfo = QString("File:(%1) Line:(%2)").arg(QString(ctx.file)).arg(ctx.line);
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString currentDate = QString("(%1)").arg(currentDateTime);
    QString message = QString("%1 %2 %3 %4").arg(logInfo).arg(contextInfo).arg(msg).arg(currentDate);
    QString stdmessage = QString("%1 %2 %3").arg(contextInfo).arg(msg).arg(currentDate);
    s_logMutex.lock();
    QFile outFile(s_logPath);
    QFileInfo fileInfo(outFile);
    if (!ensureDirExist(fileInfo.absoluteDir().absolutePath())) {
        s_logMutex.unlock();
        return;
    }

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        s_logMutex.unlock();
        return;
    }
    QTextStream ts(&outFile);
    ts << message.toUtf8() << endl;
    std::cout << msg.toStdString() << std::endl;
    outFile.close();
    s_logMutex.unlock();
}
