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
    QString taskId; //任务id，主键唯一标识
    int downloadStatus; //任务状态
    QDateTime modifyTime; //修改时间
    QString compeletedLength ; //已下载大小
    QString  downloadSpeed ; //下载速度
    QString  totalLength; //下载总文件大小
    int percent;
    int totalFromSource; //获取的下载文件大小
    QDateTime finishTime; //完成时间
    TaskStatus(){
        taskId = "";
        downloadStatus = -1;
        compeletedLength = "";
        downloadSpeed = "";
        totalLength = "";
        percent  = -1;
        totalFromSource = 0;
    };
    TaskStatus(QString task_id,int download_status,QDateTime modify_time,QString compeletedLength,
                  QString download_speed,QString totalLength,int percent,int totalFromSource, QDateTime finish_time)
    {
        taskId = task_id;
        downloadStatus = download_status;
        modifyTime.setDate(modify_time.date());
        modifyTime.setTime(modify_time.time());
        compeletedLength = compeletedLength ;
        downloadSpeed = download_speed;
        totalLength = totalLength;
        if (percent < 0 || percent > 100) {
            percent = 0;
        } else {
            percent = percent;
        }
        totalFromSource = totalFromSource;
        finishTime = finish_time;
    };
};


struct Task
{
    QString taskId; //任务id，主键唯一标识
    QString gid; //下载gid
    int gidIndex; //位置index
    QString url; //下载url地址
    QString downloadPath; //下载全路径包括文件名
    QString downloadFilename; //下载文件名
    QDateTime createTime; //任务创建时间
    Task(){
        taskId = "";
        gid = "";
        gidIndex = -1;
        url = "";
        downloadPath = "";
        downloadFilename = "";
    };
    Task(QString task_id,QString gid,int gid_index,QString url,QString download_path,
                QString download_filename,QDateTime create_time)
    {
        taskId = task_id;
        gid = gid;
        gidIndex = gid_index;
        url = url;
        downloadPath = download_path;
        downloadFilename = download_filename;
        createTime.setDate(create_time.date());
        createTime.setTime(create_time.time());
    };
};

struct UrlInfo
{
    QString taskId ;//任务id唯一标识
    QString url; //下载url
    QString downloadType; //下载类型
    QString seedFile; //种子文件
    QString selectedNum; //选择的种子文件序号
    QString infoHash; //种子文件hash值
    UrlInfo(){
        taskId = "" ;
        url = "";
        downloadType = "";
        seedFile = "";
        selectedNum = "";
        infoHash = "";
    };
    UrlInfo(QString task_id ,QString url ,QString download_type,
               QString seedFile,QString selectedNum,QString infoHash)
    {
        taskId = task_id ;
        url = url;
        downloadType = download_type;
        seedFile = seedFile;
        selectedNum = selectedNum;
        infoHash = infoHash;
    }
};

#endif // DBDEFINE_H
