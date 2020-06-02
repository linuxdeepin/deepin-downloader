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

struct S_Task_Status {
    QString m_task_id; //任务id，主键唯一标识
    int m_download_status; //任务状态
    QDateTime m_modify_time; //修改时间
    QString m_compeletedLength; //下载大小
    QString m_download_speed; //下载速度
    QString m_totalLength; //下载文件大小
    int m_percent;
    int m_totalFromSource; //获取的下载文件大小
    QDateTime m_finish_time; //完成时间
    S_Task_Status() {};
    S_Task_Status(QString task_id, int download_status, QDateTime modify_time, QString compeletedLength,
                  QString download_speed, QString totalLength, int percent, int totalFromSource, QDateTime finish_time)
    {
        m_task_id = task_id;
        m_download_status = download_status;
        m_modify_time.setDate(modify_time.date());
        m_modify_time.setTime(modify_time.time());
        m_compeletedLength = compeletedLength;
        m_download_speed = download_speed;
        m_totalLength = totalLength;
        if (percent < 0 || percent > 100) {
            m_percent = 0;
        } else {
            m_percent = percent;
        }
        m_totalFromSource = totalFromSource;
        m_finish_time = finish_time;
    };
};

struct S_Task {
    QString m_task_id; //任务id，主键唯一标识
    QString m_gid; //下载gid
    int m_gid_index; //位置index
    QString m_url; //下载url地址
    QString m_download_path; //下载全路径包括文件名
    QString m_download_filename; //下载文件名
    QDateTime m_create_time; //任务创建时间
    S_Task() {};
    S_Task(QString task_id, QString gid, int gid_index, QString url, QString download_path,
           QString download_filename, QDateTime create_time)
    {
        m_task_id = task_id;
        m_gid = gid;
        m_gid_index = gid_index;
        m_url = url;
        m_download_path = download_path;
        m_download_filename = download_filename;
        m_create_time.setDate(create_time.date());
        m_create_time.setTime(create_time.time());
    };
};

struct S_Url_Info {
    QString m_task_id; //任务id唯一标识
    QString m_url; //下载url
    QString m_download_type; //下载类型
    QString m_seedFile; //种子文件
    QString m_selectedNum; //选择的种子文件序号
    QString m_infoHash; //种子文件hash值
    S_Url_Info() {};
    S_Url_Info(QString task_id, QString url, QString download_type,
               QString seedFile, QString selectedNum, QString infoHash)
    {
        m_task_id = task_id;
        m_url = url;
        m_download_type = download_type;
        m_seedFile = seedFile;
        m_selectedNum = selectedNum;
        m_infoHash = infoHash;
    }
};

#endif // DBDEFINE_H
