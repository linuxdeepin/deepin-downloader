/**
* @file aria2rpcinterface.h
* @brief aria2 rpc 接口类
* @author denglinglong  <denglinglong@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 11:55
* @copyright 2020-2020Uniontech Technology Co., Ltd.
*/

#ifndef ARIA2RPCINTERFACE_H
#define ARIA2RPCINTERFACE_H

#include <QObject>
#include <QJsonObject>
#include "aria2cbtinfo.h"
#include "aria2const.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QCryptographicHash>


class Aria2RPCInterface : public QObject
{
    Q_OBJECT
public:
    explicit Aria2RPCInterface(QObject *parent = nullptr);

public:

    /**
     *@brief startUp 启动aria2c进程
     *@return 启动成功true  启动失败 false
     */
    bool startUp();
    /**
     *@brief init 初始化
     *@return
     */
    void init();//初始化RPC服务
    /**
     *@brief checkAria2cProc() 检测aria2c是否启动
     *@return 启动成功 true  否则 false
     */
    bool checkAria2cProc();

    /**
     *@brief killAria2cProc关闭aria2c进程
     *@return 启动成功 true  否则 false
     */
    bool killAria2cProc();

    /**
     *@brief 设置默认的下载路径
     *@param strDir
     *@return
     */
    void setDefaultDownLoadDir(QString strDir);

    /**
     *@brief 获得默认的下载路径
     *@return
     */
    QString getDefaultDownLoadDir() ;//{return this->defaultDownloadPath();}

    /**
     *@brief 设置配置文件路径
     *@param strPath
     *@return
     */
    void setConfigFilePath(QString strPath);

    QString getConfigFilePath();//获得配置文件路径

    bool checkAria2cFile();//检查aria2c文件
    /**
     * @brief addUri 添加下载 HTTP(S)/FTP/BitTorrent Magnet 链接 ，
     * @param uri 链接
     * @param opt QMap<QString, QVariant> 下载参数，配置仅应用于本次下载
     * 参数详见aria2c文档
     * 示例代码
     * QMap<QString, QVariant> opt;
     * //dir 指定文件存储的路径
     * opt.insert("dir", dir);
     * //max-download-limit 指定最大下载速度（字节/秒）速度值可以追加K或者M
     * opt.insert("max-download-limit", "100K");
     *
     * @param strId 可选，该参数用来唯一标示一次请求，异步返回的结果里会包含请求发出是指定的id，用以区分匹配多次请求。
     * 默认为method名，详见aria2const.h常量
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void addUri(QString strUri,QMap<QString,QVariant> opt,QString strId);//添加uri地址

    /**
     * @brief addNewUri 添加下载 HTTP(S)/FTP/BitTorrent Magnet 链接 ，
     * @param uri 链接
     * @param savepath  下载路径
     * @param strId     Gid
     */
    void addNewUri(QString uri,QString savepath,QString strId);


    /**
     * @brief addTorrent 添加下载Torrent
     * @param torrentFile torrent种子文件路径
     * @param opt QMap<QString, QVariant> 下载参数，配置仅应用于本次下载
     * 参数详见aria2c文档
     * 示例代码
     * QMap<QString, QVariant> opt;
     * //dir 指定文件存储的路径
     * opt.insert("dir", dir);
     * //seed-time bt下载完成后做种上传的时间。默认为0不做种。如果不为0，下载会一直处于active状态儿不会完成。
     * opt.insert("seed-time", 0);
     * //max-upload-limit 指定最大上传速度（字节/秒）速度值可以追加K或者M
     * opt.insert("max-upload-limit", "100K");
     * //max-download-limit 指定最大下载速度（字节/秒）速度值可以追加K或者M
     *
     * @param id 可选，该参数用来唯一标示一次请求，异步返回的结果里会包含请求发出是指定的id，用以区分匹配多次请求。
     * 默认为method名，详见aria2const.h常量
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void addTorrent(QString strTorrentFile,QMap<QString,QVariant> opt,QString strId);//添加bt文件

    /**
     * @brief addMetalink 添加Metalink
     * @param metalinkFile metalink种子文件路径
     * @param opt 同addTorrent
     * @param id 可选，该参数用来唯一标示一次请求，异步返回的结果里会包含请求发出是指定的id，用以区分匹配多次请求。
     * 默认为method名，详见aria2const.h常量
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void addMetalink(QString strMetalink,QMap<QString,QVariant> opt,QString strId);//

    /**
     * @brief tellStatus 获取下载状态
     * @param gId GID aria2c生成的下载唯一标示
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回指定下载的当前状态
     * result为一个JsonObject，包含类似map键值，详见aria2文档
     */
    void tellStatus(QString gId, QString id = "");

    /**
     * @brief tellStatus 获取下载状态 可指定需要的返回字段
     * @param gId GID aria2c生成的下载唯一标示
     * @param keys 指定需要的返回字段列表，参数详见aria2文档
     * [status|totalLength|completedLength|uploadLength|bitfield|downloadSpeed|uploadSpeed|...]
     * 示例代码：
     * QStringList keys;
     * keys<<"gid"<<"status"<<"totalLength"<<"completedLength"<<"downloadSpeed";
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回指定下载的当前状态
     * result为一个JsonObject，包含类似map键值，详见aria2文档
     */
    void tellStatus(QString gId, QStringList keys, QString id = "");
    /**
     * @brief pause 暂停指定下载 active/waiting状态将变为paused
     * @param gId GID
     * @param id 可选，该参数用来唯一标示一次请求，异步返回的结果里会包含请求发出是指定的id，用以区分匹配多次请求。
     * 默认为method名，详见aria2cconst.h常量
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void pause(QString gId, QString id = "");

    /**
     * @brief forcePause 强制暂停 active/waiting状态将变为paused
     * @param gId GID aria2c生成的下载唯一标示
     * @param id 可选，该参数用来唯一标示一次请求，异步返回的结果里会包含请求发出是指定的id，用以区分匹配多次请求。
     * 默认为method名，详见aria2cconst.h常量
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void forcePause(QString gId, QString id = "");
    /**
     * @brief pauseAll 全部暂停 active/waiting状态将变为paused
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回 "OK"
     * {id:"", jsonrpc:"2.0", result:"ok"}
     * QString ok = json.value("result").toString();
     */
    void pauseAll(QString id = "");//

    /**
     * @brief forcePauseAll 强制全部暂停
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回 "OK"
     * {id:"", jsonrpc:"2.0", result:"ok"}
     * QString ok = json.value("result").toString();
     */
    void forcePauseAll(QString id = "");
    /**
     * @brief unpause 恢复指定GID下载 paused状态变为waiting
     * @param gId GID aria2c生成的下载唯一标示
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void unpause(QString gId, QString id = "");

    /**
     * @brief unpauseAll 全部恢复下载
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回 "OK"
     * {id:"", jsonrpc:"2.0", result:"ok"}
     * QString ok = json.value("result").toString();
     */
    void unpauseAll(QString id = "");

    /**
     * @brief remove 移除指定下载。下载状态会变为removed。
     * 注意，实验表明并不会删除下载的文件。
     * @param gId GID aria2c生成的下载唯一标示
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void remove(QString gId, QString id = "");

    /**
     * @brief forceRemove 强制移除,不会做后处理
     * @param gId GID aria2c生成的下载唯一标示
     * @param id 同其它方法参数
     *
     * signal_success信号中异步返回本次下载的GID
     * {id:"", jsonrpc:"2.0", result:"gid"}
     * QString gId = json.value("result").toString();
     */
    void forceRemove(QString gId, QString id = "");

    /**
     * @brief getFiles 获取文件列表
     * @param gId
     * @param id
     *
     * signal_success信号中异步返回,参数详见aria2文档
     */
    void getFiles(QString gId, QString id = "");

    /**
     * @brief modify_config_file 写入配置文件
     * @param config_item 配置项
     * @param value 配置值
     *
     * signal_success信号中异步返回，参数结构见aria2文档
     */
    void modify_config_file(QString config_item, QString value);

    /**
     * @brief changeGlobalOption 变更全局配置
     * @param options 配置项键值map
     * @param id
     *
     * signal_success信号中异步返回，参数结构见aria2文档
     */
    void changeGlobalOption(QMap<QString, QVariant> options, QString id = "");

    /**
     * @brief setMaxDownloadNum 设置最大任务数
     * @param maxDownload
     *
     * 调用changeglobaloption设置,参数详见aria2文档
     */
    void setMaxDownloadNum(QString maxDownload);
    /**
     * @brief setDownloadUploadSpeed 下载和上传速度的设置
     * @param downloadSpeed 下载速度
     * @param uploadSpeed   上传速度
     *
     */
    void setDownloadUploadSpeed(QString downloadSpeed, QString uploadSpeed);
    /**
     * @brief SetDisckCacheNum 设置磁盘缓存大小
     * @param diskCacheNum 缓存大小
     *
     */
    void SetDisckCacheNum(QString disckCacheNum);

    /**
     * @brief setDownloadLimitSpeed 设置最大下载速度
     * @param downloadlimitSpeed
     *
     */
    void setDownloadLimitSpeed(QString downloadlimitSpeed);

    /**
     * @brief setUploadLimitSpeed 设置最大上传速度
     * @param UploadlimitSpeed
     *
     */
    void setUploadLimitSpeed(QString UploadlimitSpeed);

    /**
     * @brief getBtToMetalink bt文件转磁力链
     * @param strFileName bt文件名
     *
     */
    QString getBtToMetalink(QString strFilePath);

private:
    /**
     *@brief 调用RPC
     *@param method 调用的方法 params  该方法对应的参数   id
     *
     *@return
     */
    void callRPC(QString method, QJsonArray params, QString id = "");
    void callRPC(QString method, QString id = "");//

    /**
     *@brief 发送请求
     *@param jsonObj json包 method请求的方法
     *
     *@return
     */
    void sendMessage(QJsonObject jsonObj,const QString &method);
    /**
     *@brief 请求的返回
     *@param reply网络对象 method 方法 id
     *
     *@return
     */
    void rpcRequestReply(QNetworkReply *reply,const QString &method,const QString id);

    QString fileToBase64(QString filePath);//文件转base64

    QString processThunderUri(QString thunder);//如果是迅雷链接会解密处理，否则原样返回
private:
    //QString cmd = "/usr/bin/aria2c";//aria2c程序路径 -> 已改成public static变量
    QString rpcPort = "16800";//rpc端口
    QString rpcServer = "http://localhost:" + rpcPort + "/jsonrpc";//rpc服务器地址
    QString defaultDownloadPath;//默认下载路径
    QString configPath = "";

signals:
    void signalRPCSuccess(QString method, QJsonObject json);
    void signalRPCError(QString method, QString id, int errCode);

public slots:

public:
    static Aria2RPCInterface *Instance();

    static const QString aria2cCmd;//aria2c程序路径
    static const QString basePath;//下载器安装目录
private:
    static Aria2RPCInterface *_pInstance;


public:
    /**
     * @brief getBtInfo  获取torrent信息
     * @param strTorrentPath  bt文件路径
     * @return Aria2cBtInfo
     */
    Aria2cBtInfo getBtInfo(QString strTorrentPath);//得到bt文件信息

    /**
     * @brief getCapacityFree 获取指定路径所在分区的磁盘剩余容量
     * @param path
     * @return
     */
    QString getCapacityFree(QString path);

    /**
     * @brief getCapacityFreeByte 获取指定路径所在分区的磁盘剩余容量(字节)
     * @param path
     * @return
     */
    long getCapacityFreeByte(QString path);

};

#endif // ARIA2RPCINTERFACE_H
