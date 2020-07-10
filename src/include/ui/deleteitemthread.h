/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file deleteitemthread.h
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

#ifndef DELETEITEMTHREAD_H
#define DELETEITEMTHREAD_H

#include "global.h"
#include <QThread>
#include "tableView.h"
#include "aria2rpcinterface.h"

using namespace Global;


/**
 * @class DeleteItemThread
 * @brief 删除任务类，是一个独立线程
*/
class DeleteItemThread:public QThread
{
    Q_OBJECT
public:
    DeleteItemThread();
    DeleteItemThread(QList<DelDataItem>,TableView *pRecycleTableview,Aria2RPCInterface *m_pAria2c,bool m_bIfDeleteLocal,QString m_StrDeleteType);
    DeleteItemThread( QList<DataItem> deleteList,TableView *pDownloadingTableview,Aria2RPCInterface *m_pAria2c ,bool m_bIfDeleteLocal,QString m_StrDeleteType);
    void run();
    bool DelDir(const QString &path);


private:
    /**
     * @brief 删除回收站数据
    */
     void deleteRecycleData();
     /**
      * @brief 删除下载列表数据
     */
     void deleteDownloadData();

signals:
     /**
      * @brief 通知主界面调用aria2删除任务
     */
     void signal_Aria2Remove(QString gId, QString id);

private:
     QList<DelDataItem> m_pRecycleDeleteList;
     TableView *m_pRecycleTableview;
     Aria2RPCInterface *m_pAria2c;
     bool m_bIfDeleteLocal;
     QString m_StrDeleteType;
     QList<DataItem> m_DeleteList;
     TableView *m_pDownloadingTableview;
};

#endif // DELETEITEMTHREAD_H
