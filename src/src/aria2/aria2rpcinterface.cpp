/**
 * @copyright 2020-2020} Uniontech Technology Co., Ltd.
 *
 * @file aria2rpcinterface.h
 *
 * @brief aria2 RPC 后端接口实现
 *
 * @date 2020-05-26 11:55
 *
 * Author: denglinglong  <denglinglong@uniontech.com>
 *
 * Maintainer: denglinglong  <denglinglong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "aria2rpcinterface.h"

#include "log.h"

#include <QProcess>

#define AIRA_CONFIG_PATH "/usr/share/downloadmanager/config/aria2.conf"

const QString Aria2RPCInterface::basePath = "/usr/bin/";
const QString Aria2RPCInterface::aria2cCmd = "aria2c"; // aria2c

Aria2RPCInterface *Aria2RPCInterface::_pInstance = new Aria2RPCInterface;

Aria2RPCInterface *Aria2RPCInterface::Instance()
{
    return _pInstance;
}

Aria2RPCInterface::Aria2RPCInterface(QObject *parent)
    : QObject(parent)
{
}

bool Aria2RPCInterface::startUp()
{
    /*
     *检测aria2c执行文件是否存在
     */
    if (!this->checkAria2cFile()) {
        qDebug() << "未发现" << Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd;
        return false;
    }

    /*
     *检测aria2c进程是否启动 如果启动,杀它,杀死.
     */
    bool bCheck = checkAria2cProc();
    if (checkAria2cProc()) {
        qDebug() << Aria2RPCInterface::aria2cCmd + "进程已存在,killAria2cProc()";
        killAria2cProc();
    }

    /*
    *设置aria2c  session 路径  时间  input 路径
    */
    QString sessionCacheFile = QDir::homePath() + "/.cache/uos-aria2c.session";
    QString inputFile = QDir::homePath() + "/.cache/uos-aria2c.input";
    QString saveSessionInterval = "30"; //秒

    qDebug() << "创建session缓存文件: " << sessionCacheFile;
    QProcess::execute("/usr/bin/touch", QStringList() << sessionCacheFile); //创建session缓存文件

    QStringList opt;
    opt << "--enable-rpc=true"; //启动RPC
    opt << "--rpc-listen-port=" + this->rpcPort; //RPC监听的端口
    opt << "--check-certificate=false"; //停用rpc身份验证
    opt << "--rpc-allow-origin-all=true"; // 允许所有来源
    opt << "--rpc-save-upload-metadata=true"; //

    //opt << "--not-conf=true";//不使用配置文件
    if (this->configPath != "") {
        opt << "--conf-path=" + this->configPath; //加载指定的配置文件
    }
    if (this->defaultDownloadPath != "") {
        opt << "--dir=" + this->defaultDownloadPath; //配置默认下载路径。优先级高于配置文件，已移动到配置文件中
    }
    opt << "--continue=true"; //http续传配置
    opt << "--disable-ipv6"; //禁用ipv6
    //opt << "--seed-time=0";//bt完成不做种
    opt << "--bt-metadata-only=true"; //仅下载bt metadata，不自动发起follow下载
    opt << "--bt-save-metadata=true"; //保存magnet metadata到同目录下.torrent文件
    opt << "--follow-torrent=false"; //当下载的文件是以.torrent结尾的，是否继续下载。true，是；false，否，只下载torrent文件；mem,不写文件保存在内存
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

bool Aria2RPCInterface::checkAria2cFile()
{
    QFile file(Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd);
    return file.exists();
}

void Aria2RPCInterface::Aria2RPCInterface::init()
{
    //定义配置文件路径
    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName());

    //判断文件是否存在,如果不存在复制配置文件内容到目录下
    QFileInfo fileInfo(m_aria2configPath);
    if (!fileInfo.exists()) {
        QFile::copy(AIRA_CONFIG_PATH, m_aria2configPath);
    }

    //设置配置文件路径
    setConfigFilePath(m_aria2configPath);
    bool rs = startUp();
    qDebug() << "Startup aria2:" << QString::number(rs);
}

bool Aria2RPCInterface::checkAria2cProc()
{
    QProcess *proc = new QProcess;
    QStringList opt;
    opt << "-c";
    //opt << "ps aux | grep aria2c";
    opt << "ps aux|grep " + Aria2RPCInterface::aria2cCmd;
    proc->start("/bin/bash", opt);
    proc->waitForFinished();
    QString output = QString::fromLocal8Bit(proc->readAllStandardOutput());
    QStringList lineList = output.split("\n");
    int cnt = 0;
    foreach(QString t, lineList) {
        if(t == "") {
            continue;
        }
        if(t.indexOf("grep " + Aria2RPCInterface::aria2cCmd) >= 0) {
            continue;
        }
        if(t.indexOf(Aria2RPCInterface::aria2cCmd) >= 0) {
            cnt++;
            //break;
        }
    }
    if(cnt>0){
        return true;
    }
    else {
        return false;
    }
}

int Aria2RPCInterface::killAria2cProc()
{
    QStringList opt;
    opt << "-c";
    opt << "ps -ef|grep " + Aria2RPCInterface::aria2cCmd + "|grep -v grep|awk '{print $2}'|xargs kill -9";
    return QProcess::execute("/bin/bash", opt);
}

void Aria2RPCInterface::setDefaultDownLoadDir(QString strDir)
{
    this->defaultDownloadPath = strDir;
}

QString Aria2RPCInterface::getDefaultDownLoadDir()
{
    return defaultDownloadPath;
}

void Aria2RPCInterface::setConfigFilePath(const QString strPath)
{
    configPath = strPath;
}

QString Aria2RPCInterface::getConfigFilePath() const
{
    return configPath;
}

void Aria2RPCInterface::addUri(QString strUri, QMap<QString, QVariant> opt, QString strId)
{
    strUri = processThunderUri(strUri); //处理迅雷链接
    QJsonArray ja, inner;
    inner.append(strUri); //可支持多个URI
    ja.append(inner);

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    callRPC(ARIA2C_METHOD_ADD_URI, ja, strId);
}

void Aria2RPCInterface::addNewUri(QString uri, QString savepath, QString filename, QString strId)
{
    QMap<QString, QVariant> opt;
    opt.insert("dir", savepath);
    opt.insert("out", filename);

    addUri(uri, opt, strId);

    qDebug() << "Add new uri" << uri;
}

void Aria2RPCInterface::addTorrent(QString strTorrentFile, QMap<QString, QVariant> opt, QString strId)
{
    QString torrentB64Str = fileToBase64(strTorrentFile); //把bt文件转成base64编码
    QJsonArray ja;
    ja.append(torrentB64Str);
    ja.append(QJsonArray());

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    callRPC(ARIA2C_METHOD_ADD_TORRENT, ja, strId);
}

void Aria2RPCInterface::addMetalink(QString strMetalink, QMap<QString, QVariant> opt, QString strId)
{
    QString metalinkB64Str = fileToBase64(strMetalink);
    QJsonArray ja;
    ja.append(metalinkB64Str);
    ja.append(QJsonArray());

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    callRPC(ARIA2C_METHOD_ADD_METALINK, ja, strId);
}

QString Aria2RPCInterface::fileToBase64(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    QString b64Str = ba.toBase64();
    return b64Str;
}

Aria2cBtInfo Aria2RPCInterface::getBtInfo(QString strTorrentPath)
{
    QProcess *pProc = new QProcess; //进程调用指针
    QStringList opt;
    opt << "--show-files=true";
    opt << strTorrentPath;
    pProc->start(Aria2RPCInterface::basePath + Aria2RPCInterface::aria2cCmd, opt); //启动aria2c进程
    pProc->waitForFinished(); //等待执行完成

    QByteArray array = pProc->readAllStandardOutput(); //获取进程执行返回值
    pProc->close(); //关闭进程

    QStringList strList = QString(array).split("\n"); //将array内容分割开了

    Aria2cBtInfo btInfo;
    bool bFlag = false;
    QString temp = "";

    foreach (QString line, strList) {
        if (line.startsWith("Mode: ")) {
            btInfo.mode = line.mid(6);
        } else if (line.startsWith("Announce:")) {
            continue;
        } else if (!bFlag && line.startsWith(" ")) {
            btInfo.announceList.append(line.mid(1));
        } else if (line.startsWith("Info Hash: ")) {
            btInfo.infoHash = line.mid(11);
        } else if (line.startsWith("Piece Length: ")) {
            btInfo.pieceLength = line.mid(14).remove('i');
        } else if (line.startsWith("The Number of Pieces: ")) {
            btInfo.pieceNumber = line.mid(22).toInt();
        } else if (line.startsWith("Total Length: ")) {
            QString tpl = line.mid(14);
            QStringList sp = tpl.split(" ");
            btInfo.totalLength = sp[0].remove('i'); //MiB==>MB

            QString len = sp[1].remove(',');
            QString len2 = len.mid(1, len.length() - 2);
            btInfo.totalLengthByets = len2.toLong();
        } else if (line.startsWith("Name: ")) {
            btInfo.name = line.mid(6);
        } else if (line.startsWith("Magnet URI: ")) {
            btInfo.magnet = line.mid(12);
        } else if (line.startsWith("Files:")) {
            continue;
        } else if (line.startsWith("idx")) {
            continue;
        } else if (line.startsWith("===+===")) {
            bFlag = true;
            temp = "";
            continue;
        } else if (bFlag && line.startsWith("---+---")) { //
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
        } else if (bFlag) { //idx|fileName
            temp += line.trimmed();
        }
    }
    return btInfo;
}

void Aria2RPCInterface::callRPC(QString method, QJsonArray params, QString id)
{
    QJsonObject json;
    json.insert("jsonrpc", "2.0");
    if (id == "") {
        json.insert("id", method);
    } else {
        json.insert("id", id);
    }
    json.insert("method", method);
    if (!params.isEmpty()) {
        json.insert("params", params);
    }
    this->sendMessage(json, method);
}

void Aria2RPCInterface::callRPC(QString method, QString id)
{
    callRPC(method, QJsonArray(), id);
}

void Aria2RPCInterface::sendMessage(QJsonObject jsonObj, const QString &method)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager; //定义网络对象

    if (!jsonObj.isEmpty()) { //json如果不为空
        QNetworkRequest *requset = new QNetworkRequest; //定义请求对象
        requset->setUrl(QUrl(this->rpcServer)); //设置服务器的uri
        requset->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        manager->post(*requset, QJsonDocument(jsonObj).toJson()); //post信息到服务器

        //调用返回的信息
        QObject::connect(manager,
                         &QNetworkAccessManager::finished,
                         this,
                         [=](QNetworkReply *reply) {
                             this->rpcRequestReply(reply, method, jsonObj.value("id").toString()); //调用出来函数
                             manager->deleteLater(); //删除
                             manager->destroyed();
                         });
    }
}

void Aria2RPCInterface::rpcRequestReply(QNetworkReply *reply, const QString &method, const QString id)
{
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(); //请求返回的属性值
    if (code == 200) { //返回正常
        QByteArray buf = reply->readAll(); //获取信息
        QJsonDocument doc = QJsonDocument::fromJson(buf); //转换为json格式
        QJsonObject obj = doc.object();
        emit signalRPCSuccess(method, obj);
    } else { //错误
        emit signalRPCError(method, id, code);
    }

    reply->deleteLater();
    reply->destroyed();
}

void Aria2RPCInterface::tellStatus(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_TELL_STATUS, ja, id);
}

void Aria2RPCInterface::tellStatus(QString gId, QStringList keys, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    QJsonArray ka;
    foreach (QString k, keys) {
        ka.append(k);
    }
    ja.append(ka);
    callRPC(ARIA2C_METHOD_TELL_STATUS, ja, id);
}

void Aria2RPCInterface::pause(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_PAUSE, ja, id);
}

void Aria2RPCInterface::forcePause(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_FORCE_PAUSE, ja, id);
}

void Aria2RPCInterface::pauseAll(QString id)
{
    callRPC(ARIA2C_METHOD_PAUSE_ALL, id);
}

void Aria2RPCInterface::forcePauseAll(QString id)
{
    callRPC(ARIA2C_METHOD_FORCE_PAUSE_ALL, id);
}

void Aria2RPCInterface::unpause(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_UNPAUSE, ja, id);
}

void Aria2RPCInterface::unpauseAll(QString id)
{
    callRPC(ARIA2C_METHOD_UNPAUSE_ALL, id);
}

void Aria2RPCInterface::remove(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_REMOVE, ja, id);
}

void Aria2RPCInterface::forceRemove(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_FORCE_REMOVE, ja, id);
}

void Aria2RPCInterface::removeDownloadResult(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_REMOVE_DOWNLOAD_RESULT, ja, id);
}

void Aria2RPCInterface::getFiles(QString gId, QString id)
{
    QJsonArray ja;
    ja.append(gId);
    callRPC(ARIA2C_METHOD_GET_FILES, ja, id);
}

void Aria2RPCInterface::changeGlobalOption(QMap<QString, QVariant> options, QString id)
{
    QJsonArray ja;
    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(options));
    QJsonObject optJson = doc.object();
    ja.append(optJson);
    callRPC(ARIA2C_METHOD_CHANGE_GLOBAL_OPTION, ja, id);
}

void Aria2RPCInterface::modify_config_file(QString config_item, QString value)
{
    QString strAll;
    QStringList strList;

    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName());

    QFile readFile(m_aria2configPath);
    if (readFile.open((QIODevice::ReadOnly | QIODevice::Text))) {
        QTextStream stream(&readFile);
        strAll = stream.readAll();
    }
    readFile.close();
    QFile writeFile(m_aria2configPath);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&writeFile);
        strList = strAll.split("\n");
        for (int i = 0; i < strList.count(); i++) {
            if (strList.at(i).contains(config_item)) {
                QString tempStr = strList.at(i);
                tempStr.replace(0, tempStr.length(), value);
                stream << tempStr << '\n';
            } else {
                if (i == strList.count() - 1) {
                    //最后一行不需要换行
                    stream << strList.at(i);
                } else {
                    stream << strList.at(i) << '\n';
                }
            }
        }
    }
    writeFile.close();
}

void Aria2RPCInterface::setMaxDownloadNum(QString maxDownload)
{
    QMap<QString, QVariant> opt;
    QString value = "max-concurrent-downloads=" + maxDownload;
    modify_config_file("max-concurrent-downloads=", value);
    opt.insert("max-concurrent-downloads", maxDownload);
    changeGlobalOption(opt);
    qDebug() << "set max download num:" << maxDownload;
}

void Aria2RPCInterface::setDownloadUploadSpeed(QString downloadSpeed, QString uploadSpeed)
{
    QMap<QString, QVariant> opt;
    QString down_speed = downloadSpeed + "K";
    opt.insert("max-overall-download-limit", down_speed);

    QString upload_speed = uploadSpeed + "K";
    opt.insert("max-overall-upload-limit", upload_speed);
    changeGlobalOption(opt);
}

void Aria2RPCInterface::SetDisckCacheNum(QString disckCacheNum)
{
    QMap<QString, QVariant> opt;
    QString cacheNum = disckCacheNum + "M";
    opt.insert("disk-cache", cacheNum);
    changeGlobalOption(opt);
    QString value = "disk-cache=" + cacheNum;
    modify_config_file("disk-cache=", value);

    qDebug() << "set disk cache num:" << disckCacheNum;
}

void Aria2RPCInterface::setDownloadLimitSpeed(QString downloadlimitSpeed)
{
    QMap<QString, QVariant> opt;

    QString speed = downloadlimitSpeed + "K";

    opt.insert("max-overall-download-limit", speed);
    changeGlobalOption(opt);

    QString value = "max-overall-download-limit=" + speed;
    modify_config_file("max-overall-download-limit=", value);

    qDebug() << "set download limit speed:" << downloadlimitSpeed;
}

void Aria2RPCInterface::setUploadLimitSpeed(QString UploadlimitSpeed)
{
    QMap<QString, QVariant> opt;
    QString speed = UploadlimitSpeed + "K";
    opt.insert("max-overall-upload-limit", speed);
    changeGlobalOption(opt);

    QString value = "max-overall-upload-limit=" + speed;
    modify_config_file("max-overall-upload-limit=", value);

    qDebug() << "set upload limit speed:" << UploadlimitSpeed;
}

QString Aria2RPCInterface::processThunderUri(QString thunder)
{
    QString uri = thunder;
    if (thunder.startsWith("thunder://")) {
        QString oUir = thunder.mid(thunder.indexOf("thunder://") + 9 + 1);
        uri = QString(QByteArray::fromBase64(oUir.toLatin1()));
        uri = uri.mid(2, uri.length() - 4); //AA[URI]ZZ
    }
    return uri;
}

QString Aria2RPCInterface::getCapacityFree(QString path)
{
    QProcess *proc = new QProcess;
    QStringList opt;
    opt << "-c";
    opt << "df -h " + path;
    proc->start("/bin/bash", opt);
    proc->waitForFinished();
    QByteArray rt = proc->readAllStandardOutput();
    proc->close();
    delete proc;

    QString free = "0B";
    QStringList lt = QString(rt).split("\n");
    if (lt.length() >= 2) {
        QString line = lt.at(1);
        QString temp;
        QStringList tpl;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                temp.append(line[i]);
            } else {
                if (temp != "") {
                    tpl.append(temp);
                    temp = "";
                }
            }
        }
        free = tpl[3];
    }
    return free + "B";
}

long Aria2RPCInterface::getCapacityFreeByte(QString path)
{
    QProcess *proc = new QProcess;
    QStringList opt;
    opt << "-c";
    opt << "df " + path;
    proc->start("/bin/bash", opt);
    proc->waitForFinished();
    QByteArray rt = proc->readAllStandardOutput();
    proc->close();
    delete proc;

    QString free = "0";
    QStringList lt = QString(rt).split("\n");
    if (lt.length() >= 2) {
        QString line = lt.at(1);
        QString temp;
        QStringList tpl;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                temp.append(line[i]);
            } else {
                if (temp != "") {
                    tpl.append(temp);
                    temp = "";
                }
            }
        }
        free = tpl[3];
    }
    return free.toLong();
}

QString Aria2RPCInterface::getBtToMetalink(QString strFilePath)
{
    QString strMetaLink = ""; //磁力链

    QFile file(strFilePath); //strFilePath文件的绝对路径
    if (file.open(QIODevice::ReadOnly)) //只读方式打开
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (!file.atEnd()) {
            hash.addData(file.readAll());
            QString stHashValue;
            stHashValue.append(hash.result().toHex());
            return stHashValue;
        }
    }

    return strMetaLink;
}

QString Aria2RPCInterface::bytesFormat(qint64 size)
{
    if (!size) {
        return "0B";
    }
    QStringList sl;
    if (sl.empty()) {
        sl << "B"
           << "KB"
           << "MB"
           << "GB"
           << "TB"
           << "PB";
    }
    int i = qFloor(qLn(size) / qLn(1024));
    return QString::number(size * 1.0 / qPow(1024, qFloor(i)), 'f', (i > 1) ? 2 : 0) + sl.at(i);
}
