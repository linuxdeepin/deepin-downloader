/**
 * @file global.h
 * @brief  全局定义
 * @author zhaoyue  <zhaoyue@uniontech.com>
 * @version 1.0.0
 * @date 2020-05-26 09:45
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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QPoint>

#include <QStyleOptionViewItem>
#include <QApplication>
namespace Global {

const QString ACTIVE = QObject::tr("Downloading");
const QString WAITING = QObject::tr("Waiting");
const QString PAUSED = QObject::tr("Paused");
const QString ERROR = QObject::tr("Error");
const QString COMPLETE = QObject::tr("Complete");
const QString REMOVED = QObject::tr("Removed");
const QString UNKNOWN = QObject::tr("Unknown");

/**
     * @struct DataItem
     * @brief 下载列结构体
     */
struct DownloadDataItem {
    int status = 0;
    int percent = 0;
    int total = 0;

    bool Ischecked;
    bool IsHide = false;
    QString taskId;
    //        QString fileName = Global::UNKNOWN;
    QString fileName = QObject::tr("Unknown");
    QString completedLength;
    QString totalLength;
    QString savePath;
    QString speed;
    QString gid;
    QString url;
    QString time;
    QString createTime;
};
/**
     * @struct DataItem
     * @brief 删除列结构体
     */
struct DeleteDataItem {
    int status = 5;
    bool Ischecked;
    bool IsHide = false;
    QString taskId;
    QString fileName;
    QString completedLength;
    QString savePath;
    QString gid;
    QString url;
    QString totalLength;
    QString deleteTime;
    QString finishTime;
};
/**
     * @enum Status
     * @brief 列表项当前状态
     */
enum DownloadJobStatus {
    Active = 0,
    Waiting,
    Paused,
    Complete,
    Removed,
    Lastincomplete,
    Error
};
/**
     * @enum MsgboxType
     * @brief 消息弹框类型
     */
enum MsgboxType {
    NewDownloding = 0,
    Warnings = 1,
    Delete = 2,
    ReDownload = 3,
    ReName = 4,
    Clear = 5,
    RepeatDownload = 6
};

}; // namespace Global

#endif
