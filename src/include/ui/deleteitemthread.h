/**
* @file deleteitemthread.h
* @brief 删除任务类，是一个独立线程
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-06-03 16:20
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
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
    DeleteItemThread(QList<DelDataItem>,TableView* m_pRecycleTableview,Aria2RPCInterface *m_pAria2c,bool m_bIfDeleteLocal,QString m_StrDeleteType);
    DeleteItemThread( QList<DataItem> m_DeleteList,TableView *m_pDownloadingTableview,Aria2RPCInterface *m_pAria2c ,bool m_bIfDeleteLocal,QString m_StrDeleteType);
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
     void signalAria2Remove(QString gId, QString id);

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
