#include "aria2rpcinterface.h"
#include <QProcess>

#include "log.h"


#define AIRA_CONFIG_PATH "/usr/share/uos-downloadmanager/config/aria2.conf"

//#ln /usr/bin/aria2c /usr/share/uos-downloadmanager/uos-aria2c
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
    /*
     *检测aria2c执行文件是否存在
     */
    if(!this->checkAria2cFile()) {
        qDebug() << "未发现" << Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd ;
        return false;
    }

    /*
     *检测aria2c进程是否启动 如果启动,杀它,杀死.
     */
    bool bCheck = checkAria2cProc();
    if(checkAria2cProc()) {
        qDebug() << Aria2RPCInterface::aria2cCmd + "进程已存在,killAria2cProc()";
        killAria2cProc();
    }

    /*
    *设置aria2c  session 路径  时间  input 路径
    */
    QString sessionCacheFile = QDir::homePath() + "/.cache/uos-aria2c.session";
    QString inputFile = QDir::homePath() + "/.cache/uos-aria2c.input";
    QString saveSessionInterval = "30";//秒

    qDebug() << "创建session缓存文件: " << sessionCacheFile;
    QProcess::execute("/usr/bin/touch", QStringList() << sessionCacheFile);//创建session缓存文件

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

//添加bt文件
void Aria2RPCInterface::addTorrent(QString strTorrentFile,QMap<QString,QVariant> opt,QString strId)
{

}

//获取bt文件信息
Aria2cBtInfo Aria2RPCInterface::getBtInfo(QString strTorrentPath)
{
    QProcess *pProc = new QProcess;//进程调用指针
    QStringList opt;
    opt << "--show-files=true";
    opt << strTorrentPath;
    pProc->start(Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd,opt);//启动aria2c进程
    pProc->waitForFinished();//等待执行完成

    QByteArray array = pProc->readAllStandardOutput();//获取进程执行返回值
    pProc->close();//关闭进程

    QStringList strList = QString(array).split("\n");//将array内容分割开了

    Aria2cBtInfo btInfo;
    bool bFlag = false;
    QString temp = "";

    foreach(QString line,strList){
        if(line.startsWith("Mode: ")) {
            btInfo.mode = line.mid(6);
        }
        else if(line.startsWith("Announce:")) {
            continue;
        }
        else if(!bFlag && line.startsWith(" ")) {
            btInfo.announceList.append(line.mid(1));
        }
        else if(line.startsWith("Info Hash: ")) {
            btInfo.infoHash = line.mid(11);
        }
        else if(line.startsWith("Piece Length: ")) {
            btInfo.pieceLength = line.mid(14).remove('i');
        }
        else if(line.startsWith("The Number of Pieces: ")) {
            btInfo.pieceNumber = line.mid(22).toInt();
        }
        else if(line.startsWith("Total Length: ")) {
            QString tpl = line.mid(14);
            QStringList sp = tpl.split(" ");
            btInfo.totalLength = sp[0].remove('i');//MiB==>MB

            QString len = sp[1].remove(',');
            QString len2 = len.mid(1, len.length() - 2);
            btInfo.totalLengthByets = len2.toLong();
        }
        else if(line.startsWith("Name: ")) {
            btInfo.name = line.mid(6);
        }
        else if(line.startsWith("Magnet URI: ")) {
            btInfo.magnet = line.mid(12);
        }
        else if(line.startsWith("Files:")) {
            continue;
        }
        else if(line.startsWith("idx")) {
            continue;
        }
        else if(line.startsWith("===+===")) {
            bFlag = true;
            temp = "";
            continue;
        }
        else if(bFlag && line.startsWith("---+---")) {//
            QStringList stl = temp.split("|");
            Aria2cBtFileInfo f;
            f.index = stl[0].toInt();
            f.path = stl[1];
            QStringList spl = stl[2].split(" ");
            f.length = spl[0].remove('i');

            QString len = spl[1].remove(',');
            QString len2 = len.mid(1, len.length() - 2);
            f.lengthBytes = len2.toLong();
            btInfo.files.append(f);
            temp = "";
        }
        else if(bFlag) {//idx|fileName
            temp += line.trimmed();
        }
    }
    return  btInfo;

}

