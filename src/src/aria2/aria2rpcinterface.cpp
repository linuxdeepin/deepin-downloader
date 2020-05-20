#include "aria2rpcinterface.h"
#include <QProcess>

#include "log.h"


#define AIRA_CONFIG_PATH "/usr/share/uos-downloadmanager/config/aria2.conf"

//#ln /usr/bin/aria2c /usr/share/uos-downloadmanager/nfs-aria2c
const QString Aria2RPCInterface::basePath = "/usr/share/uos-downloadmanager/";// /usr/bin/
const QString Aria2RPCInterface::aria2cCmd = "nfs-aria2c";// aria2c

Aria2RPCInterface *Aria2RPCInterface::_pInstance = new Aria2RPCInterface;
Aria2RPCInterface *Aria2RPCInterface::Instance()
{
    return _pInstance;
}



Aria2RPCInterface::Aria2RPCInterface(QObject *parent) : QObject(parent)
{

}

//启动RPC服务
bool Aria2RPCInterface::startUp()
{
    //检测进程文件是否存在
    //检测aria2c进程是否启动 如果启动,杀它,杀死.
    //创建aria2c  session缓存文件
    //

    if(!this->checkAria2cFile()) {
        qDebug() << "未发现" << Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd ;
        return false;
    }

    bool bCheck = checkAria2cProc();
    if(checkAria2cProc()) {
        qDebug() << Aria2RPCInterface::aria2cCmd + "进程已存在,killAria2cProc()";
        killAria2cProc();
    }

    QString sessionCacheFile = QDir::homePath() + "/.cache/nfs-aria2c.session";
    QString inputFile = QDir::homePath() + "/.cache/nfs-aria2c.input";
    QString saveSessionInterval = "30";//秒
    //创建session缓存文件
    qDebug() << "创建session缓存文件: " << sessionCacheFile;
    QProcess::execute("/usr/bin/touch", QStringList() << sessionCacheFile);
    //
    QStringList opt;
    opt << "--enable-rpc=true";//启动RPC
    opt << "--rpc-listen-port=" + this->rpcPort;//RPC监听的端口
    opt << "--check-certificate=false";//停用rpc身份验证
    opt << "--rpc-allow-origin-all=true";// 允许所有来源
    opt << "--rpc-save-upload-metadata=true";//

    //opt << "--not-conf=true";//不使用配置文件
    if(this->configPath != "") {
        opt << "--conf-path=" + this->configPath;//加载指定的配置文件
    }
    if(this->defaultDownloadPath != "") {
      opt << "--dir=" + this->defaultDownloadPath;//配置默认下载路径。优先级高于配置文件，已移动到配置文件中
    }
    opt << "--continue=true";//http续传配置
    opt << "--disable-ipv6";//禁用ipv6
    //opt << "--seed-time=0";//bt完成不做种
    opt << "--bt-metadata-only=true";//仅下载bt metadata，不自动发起follow下载
    opt << "--bt-save-metadata=true";//保存magnet metadata到同目录下.torrent文件
    opt << "--follow-torrent=false";//当下载的文件是以.torrent结尾的，是否继续下载。true，是；false，否，只下载torrent文件；mem,不写文件保存在内存
    //opt << "--follow-metalink=false";//类似torrent
    opt << "--bt-remove-unselected-file=true";
    //opt << "--input-file=" + inputFile;
    opt << "--save-session=" + sessionCacheFile;
    opt << "--save-session-interval=" + saveSessionInterval;

    qDebug() << Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd << opt.join(' ');

    QProcess *proc = new QProcess;
    proc->start(Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd, opt);
    proc->waitForStarted();
    bCheck = checkAria2cProc();
    qDebug() << "启动aria2c完成！ " << proc->state() << bCheck;
    return bCheck;

}

//检查aria2c文件
bool Aria2RPCInterface::checkAria2cFile()
{

    QFile file(Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd);
    return file.exists();
}

//初始化RPC服务
void Aria2RPCInterface::Aria2RPCInterface::init()
{
    //定义配置文件路径
    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    //判断文件是否存在,如果不存在复制配置文件内容到目录下
    QFileInfo fileInfo(m_aria2configPath);
    if(!fileInfo.exists())
    {
        QFile::copy(AIRA_CONFIG_PATH,m_aria2configPath);
    }


    //设置配置文件路径
    setConfigFilePath(m_aria2configPath);
    bool rs = startUp();
    qDebug()<<"Startup aria2:"<<QString::number(rs);
}

;//检测aria2c是否启动
bool Aria2RPCInterface::checkAria2cProc()
{

    return true;
}


//关闭aria2c进程
bool Aria2RPCInterface::killAria2cProc()
{

    return true;
}

//设置默认的下载路径
void Aria2RPCInterface::setDefaultDownLoadDir(QString strDir)
{

}

//获得默认的下载路径
QString Aria2RPCInterface::getDefaultDownLoadDir()
{

    return "";
}

//设置配置文件路径
void Aria2RPCInterface::setConfigFilePath(QString strPath)
{

}

//获得配置文件路径
QString Aria2RPCInterface::getConfigFilePath()
{

    return "";
}


//添加uri地址
void Aria2RPCInterface::addUri(QString strUri,QMap<QString,QVariant> opt,QString strId)
{

}
