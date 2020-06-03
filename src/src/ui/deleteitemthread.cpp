/**
* @file deleteitemthread.cpp
* @brief 删除任务类，是一个独立线程
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-06-03 16:21
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "deleteitemthread.h"
#include <QDebug>
#include <QFile>
#include <QDir>

DeleteItemThread::DeleteItemThread()
{

}

DeleteItemThread::DeleteItemThread(QList<DelDataItem>recycle_delete_list,TableView *recycle_tableview,Aria2RPCInterface *aria2c,bool ifDeleteLocal,QString deleteType)
{
    this->m_pRecycleDeleteList = recycle_delete_list;
    this->m_pRecycleTableview = recycle_tableview;
    this->m_pAria2c = aria2c;
    this->m_bIfDeleteLocal = ifDeleteLocal;
    this->m_StrDeleteType = deleteType;
}
DeleteItemThread::DeleteItemThread( QList<DataItem> delete_list,TableView *downloading_tableview,Aria2RPCInterface *aria2c ,bool ifDeleteLocal,QString deleteType)
{
    this->m_DeleteList = delete_list;
    this->m_pDownloadingTableview = downloading_tableview;
    this->m_pAria2c = aria2c;
    this->m_bIfDeleteLocal = ifDeleteLocal;
    this->m_StrDeleteType = deleteType;
}

//删除文件夹
bool DeleteItemThread::DelDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

void DeleteItemThread::deleteRecycleData()
{
    for(int i=0;i<m_pRecycleDeleteList.size();i++)
    {

         QString save_path= m_pRecycleDeleteList.at(i).savePath;
         QString gid = m_pRecycleDeleteList.at(i).gid;
         QString taskId = m_pRecycleDeleteList.at(i).taskId;
         QString filename = m_pRecycleDeleteList.at(i).fileName;


         if(m_bIfDeleteLocal)
         {
              if(save_path!="")
              {
                  QFileInfo fileinfo(save_path);
                  if(fileinfo.isDir() && save_path.contains(filename) && ! filename.isEmpty())
                  {
                      QDir tar(m_pRecycleDeleteList.at(i).savePath);
                      tar.removeRecursively();
                  }
                  else
                  {
                      QString aria_temp_file=save_path+".aria2";
                      if(!save_path.isEmpty())
                      {
                          QFile::remove(save_path);
                          if(QFile::exists(aria_temp_file))
                          {
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
     for(int i = 0; i < m_DeleteList.size(); ++i)
     {
          QString gid = m_DeleteList.at(i).gid;
          QString task_id = m_DeleteList.at(i).taskId;
          QString save_path = m_DeleteList.at(i).savePath;
          QString filename = m_DeleteList.at(i).fileName;
          emit signalAria2Remove(m_DeleteList.at(i).gid, "");

         if(m_bIfDeleteLocal)
         {
             m_pAria2c->pause(gid,task_id);

             if(save_path != "")
             {
                 QFileInfo fileinfo(save_path);
                 if(fileinfo.isDir() && save_path.contains(filename) && ! filename.isEmpty())
                 {
                     QDir tar(save_path);
                     tar.removeRecursively();
                 }
                 else
                 {
                     QString aria_temp_file=save_path+".aria2";
                      if(!save_path.isEmpty())
                       {

                            QFile::remove(save_path);
                            if(QFile::exists(aria_temp_file))
                            {
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
    if(m_StrDeleteType == "recycle_delete")
    {
        deleteRecycleData();
    }
    if(m_StrDeleteType == "download_delete")
    {
        deleteDownloadData();
    }

}

