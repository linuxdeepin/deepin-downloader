#ifndef DBDEFINE_H
#define DBDEFINE_H
#include <QString>
#include <QDateTime>

struct S_Task_Status
{
    QString m_task_id; //任务id，主键唯一标识
    int m_download_status; //任务状态
    QDateTime m_modify_time; //修改时间
    QString m_compeletedLength ; //下载大小
    QString  m_download_speed ; //下载速度
    QString  m_totalLength; //下载文件大小
    int m_percent;
    int m_totalFromSource; //获取的下载文件大小
    QDateTime m_finish_time; //完成时间
} ;

struct S_Task
{
    QString m_task_id; //任务id，主键唯一标识
    QString m_gid; //下载gid
    int m_gid_index; //位置index
    QString m_url; //下载url地址
    QString m_download_path; //下载全路径包括文件名
    QString m_download_filename; //下载文件名
    QDateTime m_create_time; //任务创建时间
};

struct S_Url_Info
{
    QString m_task_id ;//任务id唯一标识
    QString m_url; //下载url
    QString m_download_type; //下载类型
    QString m_seedFile; //种子文件
    QString m_selectedNum ;//选择的种子文件序号
    QString m_infoHash; //种子文件hash值
};



#endif // DBDEFINE_H
