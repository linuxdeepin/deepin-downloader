#ifndef ARIA2CBTINFO_H
#define ARIA2CBTINFO_H
#include <QObject>


class Aria2cBtFileInfo{
public:
    int index;//文件索引,从1开始
    QString path;//文件路径,包含文件夹层次
    QString length;//文件大小
    long lengthBytes;//文件大小(字节)
};

class Aria2cBtInfo
{
public:
    Aria2cBtInfo();

public:
    QString mode;//模式,multi表示有多个文件,signal表示单个文件
    QStringList announceList;//种子服务器列表
    QString infoHash;//hash特征值
    QString pieceLength;//分片大小
    int pieceNumber;//分片数量
    QString totalLength;//总大小(带单位)
    long totalLengthByets;//总大小(字节)
    QString name;//种子名称(多文件时为文件夹名,单文件为文件名)
    QString magnet;//对应的磁力链接
    QList<Aria2cBtFileInfo> files;//文件列表
};

#endif // ARIA2CBTINFO_H
