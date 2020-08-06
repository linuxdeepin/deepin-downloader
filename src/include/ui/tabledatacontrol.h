/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tabledatacontrol.h
 *
 * @brief 表格数据管理类
 *
 * @date 2020-06-10 17:55
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

#ifndef TABLEDATACONTROL_H
#define TABLEDATACONTROL_H

#include <QObject>

class TableView;
class QSharedMemory;
struct Task;
namespace Global {
    struct DownloadDataItem;
    struct DeleteDataItem;
}


/**
 * @class tableDataControl
 * @brief 列表数据管理，包括一些新建、删除等
 */
class tableDataControl : public QObject
{
    Q_OBJECT
public:
    explicit tableDataControl(TableView* pTableView, QObject *parent = nullptr);

    void initTabledata();

    /**
     * @brief 设置RecycleTable
     */
    void setRecycleTable(TableView *pRecycleTable);

    /**
     * @brief 将正在下载列表里文件不存在的任务移到回收站
     */
    void removeDownloadListJob(Global::DownloadDataItem *pData, bool isAddToRecycle = true);

    /**
     * @brief aria2下载事件
     */
    void aria2MethodAdd(QJsonObject &json, QString &searchContent);

    /**
     * @brief aria2状态改变事件
     */
    void aria2MethodStatusChanged(QJsonObject &json, int iCurrentRow,  QString &searchContent);

    /**
     * @brief aria2关闭事件
     */
    void aria2MethodShutdown(QJsonObject &json);

    /**
     * @brief aria2获取文件事件
     */
    void aria2MethodGetFiles(QJsonObject &json, int iCurrentRow);

    /**
     * @brief aria2继续下载事件
     */
    void aria2MethodUnpause(QJsonObject &json, int iCurrentRow);

    /**
     * @brief aria2继续所有下载事件
     */
    void aria2MethodUnpauseAll(QJsonObject &json, int iCurrentRow);


    /**
     * @brief aria2强制删除事件
     */
    void aria2MethodForceRemove(QJsonObject &json);

    /**
     * @brief 退出之前保存
     */
    void saveDataBeforeClose();

    /**
     * @brief 查找的文本改变
    */
    void searchEditTextChanged(QString text);

    /**
     * @brief 删除ACtion槽函数
    */
    int onDelAction(int currentLab);

    /**
     * @brief 正在下载和已完成列表重新下载ACtion槽函数
    */
    int RedownloadDownloadAndFinishList(QList<Global::DownloadDataItem*> &reloadList);

    /**
     * @brief 下载错误重新下载ACtion槽函数
    */
    void RedownloadErrorItem(Global::DownloadDataItem* errorItem);

    /**
     * @brief 回收站重新下载ACtion槽函数
    */
    int RedownloadTrashList(QList<Global::DeleteDataItem*> &reloadList);

    /**
     * @brief 还原下载ACtion槽函数
    */
    void onReturnOriginAction();

    /**
     * @brief 打开文件ACtion槽函数
    */
    void onOpenFileAction();

    /**
     * @brief 打开文件目录ACtion槽函数
    */
    int onOpenFolderAction(int currentLab);

    /**
     * @brief 重命名文件ACtion槽函数
    */
    void onRenameAction();

    /**
     * @brief 清除回收站ACtion槽函数
    */
    void onClearRecyleAction();

    /**
     * @brief 复制URL ACtion槽函数
    */
    int onCopyUrlAction(int currentLab, QString &copyUrl);

    /**
     * @brief 永久删除ACtion槽函数
    */
    int onDeletePermanentAction(int currentLab);

    /**
     * @brief  获取删除正在下载和已完成列表窗口确定信号
     * @param ischecked 是否删除本地文件，true 删除本地文件；false 不删除
     * @param permanent 是否彻底删除，true彻底删除；false不彻底删除
     */
    void onDeleteDownloadListConfirm(bool ischecked,bool permanent, TableView* pRecycleTableView);

    /**
     * @brief  获取删除回收站列表窗口确定信号
     * @param ischecked 是否删除本地文件，true 删除本地文件；false 不删除
     * @param permanent 是否彻底删除，true彻底删除；false不彻底删除
     */
    void onDeleteRecycleListConfirm(bool ischecked,bool permanent);

    /**
     * @brief  正在下载列表和已完成列表重新下载
     */
    void downloadListRedownload(QString id);

    /**
     * @brief  已删除列表重新下载
     */
    void recycleListRedownload(QString id);


private:

    /**
     * @brief 获取url中的文件名称
     * @param url 下载地址
     */
    QString getFileName(const QString &url);

    /**
     * @brief 处理设置界面通知设置函数
     */
    void dealNotificaitonSettings(QString statusStr, QString fileName, QString errorCode);

    /**
     * @brief 格式化文件大小 （1B1KB1MB1GB）
     */
    QString formatFileSize(long size);

    /**
     * @brief 从配置文件中获取下载路径
     */
    QString   getDownloadSavepathFromConfig();

    /**
     * @brief 格式化下载速度（1B1KB1MB1GB  /S）
     */
    QString formatDownloadSpeed(long size);

    /**
     * @brief 检查文件是否存在
     */
    bool checkFileExist(QString &filePath);

    /**
     * @brief 清空共享内存
     */
    void clearShardMemary();
public slots:

    /**
     * @brief 异常处理确认
     */
    void onUnusualConfirm(int index, const QString &taskId);

    /**
    * @brief 通知aria2删除任务
    */
    void onAria2Remove(QString gId, QString id);

signals:
    /**
     * @brief 文件为bt，自动开始下载
     */
    void AutoDownloadBt(QString btFilePath);

    /**
     * @brief 重新下载信号
     */
    void RedownloadJob(QString taskId, int rd);

    /**
     * @brief 开始下载信号
     */
    void DownloadUnusuaJob(QStringList &urlList, QString savePath, QString filename = "");

    /**
     * @brief 删除完成
    */
    void removeFinished();

    void startDownload();
private:
    TableView *m_DownloadTableView;
    TableView *m_RececleTableView;
    QList<Global::DownloadDataItem*> m_DeleteList;
    QList<Global::DeleteDataItem*> m_RecycleDeleteList;
};

#endif // TABLEDATACONTROL_H
