#ifndef ARIA2RPCINTERFACE_H
#define ARIA2RPCINTERFACE_H

#include <QObject>
#include <QJsonObject>

class Aria2RPCInterface : public QObject
{
    Q_OBJECT
public:
    explicit Aria2RPCInterface(QObject *parent = nullptr);

public:
    bool startUp();//启动RPC服务
    void init();//初始化RPC服务
    bool checkAria2cProc();//检测aria2c是否启动
    bool killAria2cProc();//关闭aria2c进程
    void setDefaultDownLoadDir(QString strDir);//设置默认的下载路径
    QString getDefaultDownLoadDir();//获得默认的下载路径
    void setConfigFilePath(QString strPath);//设置配置文件路径
    QString getConfigFilePath();//获得配置文件路径
    bool checkAria2cFile();//检查aria2c文件
    void addUri(QString strUri,QMap<QString,QVariant> opt,QString strId);//添加uri地址
    void addTorrent();//添加bt文件
    void addMetalink();//添加磁力链地址
    void pause();//暂停
    void pauseAll();//
    void remove();//移除

    void getBtInfo();//得到bt文件信息


private:

private:
    //QString cmd = "/usr/bin/aria2c";//aria2c程序路径 -> 已改成public static变量
    QString rpcPort = "88888";//rpc端口
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


};

#endif // ARIA2RPCINTERFACE_H
