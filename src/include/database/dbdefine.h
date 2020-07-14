#ifndef DBDEFINE_H
#define DBDEFINE_H
#include <QString>
#include <QDateTime>

/**
* @file %{CurrentDocument:dbdefine.h}
* @brief
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date %{CurrentDate:2020-05-26} %{CurrentTime:17:59}
* @copyright 2020-%{CurrentDate:2020} Uniontech Technology Co., Ltd.
*/

struct TaskStatus
{
    QString m_taskId; //任务id，主键唯一标识
    int m_downloadStatus; //任务状态
    QDateTime m_modifyTime; //修改时间
    QString m_compeletedLength ; //已下载大小
    QString  m_downloadSpeed ; //下载速度
    QString  m_totalLength; //下载总文件大小
    int m_percent;
    int m_totalFromSource; //获取的下载文件大小
    QDateTime m_finishTime; //完成时间
    TaskStatus(){
        m_taskId = "";
        m_downloadStatus = -1;
        m_compeletedLength = "";
        m_downloadSpeed = "";
        m_totalLength = "";
        m_percent  = -1;
        m_totalFromSource = 0;
    };
    TaskStatus(QString task_id,int download_status,QDateTime modify_time,QString compeletedLength,
                  QString download_speed,QString totalLength,int percent,int totalFromSource, QDateTime finish_time)
    {
        m_taskId = task_id;
        m_downloadStatus = download_status;
        m_modifyTime.setDate(modify_time.date());
        m_modifyTime.setTime(modify_time.time());
        m_compeletedLength = compeletedLength ;
        m_downloadSpeed = download_speed;
        m_totalLength = totalLength;
        if (percent < 0 || percent > 100) {
            m_percent = 0;
        } else {
            m_percent = percent;
        }
        m_totalFromSource = totalFromSource;
        m_finishTime = finish_time;
    };
};


struct Task
{
    QString m_taskId; //任务id，主键唯一标识
    QString m_gid; //下载gid
    int m_gidIndex; //位置index
    QString m_url; //下载url地址
    QString m_downloadPath; //下载全路径包括文件名
    QString m_downloadFilename; //下载文件名
    QDateTime m_createTime; //任务创建时间
    Task(){
        m_taskId = "";
        m_gid = "";
        m_gidIndex = -1;
        m_url = "";
        m_downloadPath = "";
        m_downloadFilename = "";
    };
    Task(QString task_id,QString gid,int gid_index,QString url,QString download_path,
                QString download_filename,QDateTime create_time)
    {
        m_taskId = task_id;
        m_gid = gid;
        m_gidIndex = gid_index;
        m_url = url;
        m_downloadPath = download_path;
        m_downloadFilename = download_filename;
        m_createTime.setDate(create_time.date());
        m_createTime.setTime(create_time.time());
    };
};

struct UrlInfo
{
    QString m_taskId ;//任务id唯一标识
    QString m_url; //下载url
    QString m_downloadType; //下载类型
    QString m_seedFile; //种子文件
    QString m_selectedNum; //选择的种子文件序号
    QString m_infoHash; //种子文件hash值
    UrlInfo(){
        m_taskId = "" ;
        m_url = "";
        m_downloadType = "";
        m_seedFile = "";
        m_selectedNum = "";
        m_infoHash = "";
    };
    UrlInfo(QString task_id ,QString url ,QString download_type,
               QString seedFile,QString selectedNum,QString infoHash)
    {
        m_taskId = task_id ;
        m_url = url;
        m_downloadType = download_type;
        m_seedFile = seedFile;
        m_selectedNum = selectedNum;
        m_infoHash = infoHash;
    }
};

#endif // DBDEFINE_H
