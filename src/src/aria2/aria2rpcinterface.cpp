// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include "config.h"
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QCryptographicHash>
#include <QtMath>
#include <QUuid>

Aria2RPCInterface *Aria2RPCInterface::m_instance = new Aria2RPCInterface;

QStringList runPipeProcess(const QString &command, const QString &filter)
{
    QProcess process;
    process.start(command);
    process.waitForFinished();

    QString comStr = process.readAllStandardOutput();
    QStringList lines = comStr.split('\n');
    QStringList filteredLines;
    if(filter.isEmpty()) {
        return lines; //返回所有
    }
    // 过滤包含指定关键字的行
    for (const QString &line : lines) {
        if (line.contains(filter, Qt::CaseInsensitive)) {
            filteredLines.append(line);
        }
    }

    // 合并过滤后的行并返回
    return filteredLines;   // 返回以换行符分隔的字符串
}

Aria2RPCInterface *Aria2RPCInterface::instance()
{
    // qDebug() << "[Aria2RPC] instance function started";
    return m_instance;
}

Aria2RPCInterface::Aria2RPCInterface(QObject *parent)
    : QObject(parent)
    , m_aria2cCmd(ARIA2C_NAME)
    , m_basePath(ARIA2C_PATH)
{
    // qDebug() << "[Aria2RPC] Interface initialized";
}

Aria2RPCInterface::~Aria2RPCInterface()
{
    // qDebug() << "[Aria2RPC] Interface destroyed";
    delete m_proc;
}

bool Aria2RPCInterface::startUp()
{
    qDebug() << "[Aria2RPC] Starting aria2 service";

    /*
     *检测aria2c执行文件是否存在
     */
    if (!checkAria2cFile()) {
        qDebug() << "未发现" << m_basePath + m_aria2cCmd;
        return false;
    }

    /*
     *检测aria2c进程是否启动 如果启动,杀它,杀死.
     */
    bool bCheck = checkAria2cProc();
    if (bCheck) {
        qDebug() << m_aria2cCmd + "进程已存在,killAria2cProc()";
        killAria2cProc();
    }

    /*
    *设置aria2c  session 路径  时间  input 路径
    */
    QString sessionCacheFile = QDir::homePath() + "/.cache/uos-aria2c.session"; //session 文件路径
    QString saveSessionInterval = "30"; //秒

    qDebug() << "创建session缓存文件: " << sessionCacheFile;
    QProcess::execute("touch", QStringList() << sessionCacheFile); //创建session缓存文件

#if QT_VERSION_MAJOR > 5
    qDebug() << "[Aria2RPC] Using Qt version > 5";
    QStringList opt;
    opt += "--enable-rpc=true"; //启动RPC
    opt += "--rpc-secret=" + getToken();
    opt += "--rpc-listen-port=" + this->m_rpcPort; //RPC监听的端口
    opt += "--check-certificate=false"; //停用rpc身份验证
    opt += "--rpc-allow-origin-all=true"; // 允许所有来源
    opt += "--rpc-max-request-size=99999999"; //设置rpc最大接收数
    opt += "--rpc-save-upload-metadata=true"; //

    //opt += "--not-conf=true";//不使用配置文件
    if (!this->m_configPath.isEmpty()) {
        qDebug() << "[Aria2RPC] Config path is not empty, using config file:" << this->m_configPath;
        opt += "--conf-path=" + this->m_configPath; //加载指定的配置文件
    }
    if (!this->m_defaultDownloadPath.isEmpty()) {
        qDebug() << "[Aria2RPC] Default download path is not empty:" << this->m_defaultDownloadPath;
        opt += "--dir=" + this->m_defaultDownloadPath; //配置默认下载路径。优先级高于配置文件，已移动到配置文件中
    }
    opt += "--continue=true"; //http续传配置
    opt += "--disable-ipv6"; //禁用ipv6
    //opt += "--seed-time=0";//bt完成不做种
    opt += "--bt-metadata-only=true"; //仅下载bt metadata，不自动发起follow下载
    opt += "--bt-save-metadata=true"; //保存magnet metadata到同目录下.torrent文件
    opt += "--follow-torrent=false"; //当下载的文件是以.torrent结尾的，是否继续下载。true，是；false，否，只下载torrent文件；mem,不写文件保存在内存
    //opt += "--follow-metalink=false";//类似torrent
    opt += "--bt-remove-unselected-file=true";
    //opt += "--input-file=" + inputFile;
    opt += "--save-session=" + sessionCacheFile;
    opt += "--save-session-interval=" + saveSessionInterval;
    opt += "--enable-dht=true"; //启动dht文件
    opt += "--enable-dht6=false"; //禁用dht6文件
    opt += "--follow-metalink=false";
    if(QSysInfo::currentCpuArchitecture() == "loongarch64"){
        qDebug() << "[Aria2RPC] Using loongarch64 architecture, disabling async-dns";
        opt += "--async-dns=false";
    }

    QProcess proc; // = new QProcess;
    proc.setStandardOutputFile("/dev/null");
    proc.setStandardErrorFile("/dev/null");
    proc.setProgram(m_basePath + m_aria2cCmd);
    proc.setArguments(opt);
    proc.startDetached();
    proc.waitForStarted();
#else
    qDebug() << "[Aria2RPC] Using Qt version <= 5";
    QString opt;
    opt += " --enable-rpc=true"; //启动RPC
    opt += " --rpc-secret=" + getToken();
    opt += " --rpc-listen-port=" + this->m_rpcPort; //RPC监听的端口
    opt += " --check-certificate=false"; //停用rpc身份验证
    opt += " --rpc-allow-origin-all=true"; // 允许所有来源
    opt += " --rpc-max-request-size=99999999"; //设置rpc最大接收数
    opt += " --rpc-save-upload-metadata=true"; //

    //opt += " --not-conf=true";//不使用配置文件
    if (!this->m_configPath.isEmpty()) {
        qDebug() << "[Aria2RPC] Config path is not empty, using config file:" << this->m_configPath;
        opt += " --conf-path=" + this->m_configPath; //加载指定的配置文件
    }
    if (!this->m_defaultDownloadPath.isEmpty()) {
        qDebug() << "[Aria2RPC] Default download path is not empty:" << this->m_defaultDownloadPath;
        opt += " --dir=" + this->m_defaultDownloadPath; //配置默认下载路径。优先级高于配置文件，已移动到配置文件中
    }
    opt += " --continue=true"; //http续传配置
    opt += " --disable-ipv6"; //禁用ipv6
    //opt += " --seed-time=0";//bt完成不做种
    opt += " --bt-metadata-only=true"; //仅下载bt metadata，不自动发起follow下载
    opt += " --bt-save-metadata=true"; //保存magnet metadata到同目录下.torrent文件
    opt += " --follow-torrent=false"; //当下载的文件是以.torrent结尾的，是否继续下载。true，是；false，否，只下载torrent文件；mem,不写文件保存在内存
    //opt += " --follow-metalink=false";//类似torrent
    opt += " --bt-remove-unselected-file=true";
    //opt += " --input-file=" + inputFile;
    opt += " --save-session=" + sessionCacheFile;
    opt += " --save-session-interval=" + saveSessionInterval;
    opt += " --enable-dht=true"; //启动dht文件
    opt += " --enable-dht6=false"; //禁用dht6文件
    opt += " --follow-metalink=false";
    if(QSysInfo::currentCpuArchitecture() == "loongarch64"){
        qDebug() << "[Aria2RPC] Using loongarch64 architecture, disabling async-dns";
        opt += " --async-dns=false";
    }

    // qDebug() << m_basePath + m_aria2cCmd << opt.join(' ');

    QProcess proc; // = new QProcess;
    proc.setStandardOutputFile("/dev/null");
    proc.setStandardErrorFile("/dev/null");
    proc.startDetached(m_basePath + m_aria2cCmd + " " + opt);
    proc.waitForStarted();
#endif

//    proc.start("ulimit -n 40");
//    bool b = proc.waitForFinished();
//    QByteArray sssss = proc.readAllStandardError();
//    QByteArray wwwww = proc.readAllStandardOutput();

//    proc.start("ulimit -n");
//    int r = proc.waitForFinished(-1);
//    int r1 = proc.waitForReadyRead();
//    sssss = proc.readAllStandardError();
//    wwwww = proc.readAllStandardOutput();

    bCheck = checkAria2cProc();
    qDebug() << "启动aria2c完成！ " << bCheck;
    return bCheck;
}

bool Aria2RPCInterface::checkAria2cFile()
{
    qDebug() << "[Aria2RPC] checkAria2cFile function started";
    QFile file(m_basePath + m_aria2cCmd);
    return file.exists();
}

bool Aria2RPCInterface::init()
{
    qDebug() << "[Aria2RPC] Initializing aria2 service";

    setupConfig();
    bool rs = startUp();
    qDebug() << "Startup aria2:" << QString::number(rs);
    return rs;
}

bool Aria2RPCInterface::checkAria2cProc()
{
    qDebug() << "[Aria2RPC] checkAria2cProc function started";
    QStringList lineList = runPipeProcess("ps aux", m_aria2cCmd);
    int cnt = 0;
    for (QString t : lineList) {
        if (t.isEmpty()) {
            // qDebug() << "[Aria2RPC] Empty line found, continuing";
            continue;
        }
        if (t.indexOf("grep " + m_aria2cCmd) >= 0) {
            // qDebug() << "[Aria2RPC] Grep process found, continuing";
            continue;
        }
        if (t.indexOf(m_aria2cCmd) >= 0) {
            // qDebug() << "[Aria2RPC] Aria2c process found";
            cnt++;
            //break;
        }
    }
    if (cnt > 0) {
        qDebug() << "[Aria2RPC] Aria2c process found, returning true";
        return true;
    } else {
        qDebug() << "[Aria2RPC] Aria2c process not found, returning false";
        return false;
    }
}

int Aria2RPCInterface::killAria2cProc()
{
    qDebug() << "[Aria2RPC] killAria2cProc function started";
    QStringList lines = runPipeProcess("ps -eo pid,lstart,cmd", m_aria2cCmd);
    QStringList processPids;

    for (const QString &line : lines) {
        QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (parts.size() < 3) continue;
        if (!parts[6].contains(m_aria2cCmd)) continue;
        processPids.append(parts[0]);
    }

    // 杀死以前启动的进程
    for (const QString &pid : processPids) {
        QProcess killProcess;
        killProcess.start("kill", QStringList() << "-9" << pid);
        killProcess.waitForFinished();
        qInfo() << "Killed process with PID:" << pid;
    }
    return 0;
}

void Aria2RPCInterface::setDefaultDownLoadDir(QString dir)
{
    // qDebug() << "[Aria2RPC] setDefaultDownLoadDir function started";
    this->m_defaultDownloadPath = dir;
    // qDebug() << "[Aria2RPC] setDefaultDownLoadDir function ended";
}

QString Aria2RPCInterface::getDefaultDownLoadDir()
{
    // qDebug() << "[Aria2RPC] getDefaultDownLoadDir function started with path:" << m_defaultDownloadPath;
    return m_defaultDownloadPath;
}

void Aria2RPCInterface::setConfigFilePath(const QString path)
{
    // qDebug() << "[Aria2RPC] setConfigFilePath function started";
    m_configPath = path;
    // qDebug() << "[Aria2RPC] setConfigFilePath function ended";
}

QString Aria2RPCInterface::getConfigFilePath() const
{
    // qDebug() << "[Aria2RPC] getConfigFilePath function with path:" << m_configPath;
    return m_configPath;
}

bool Aria2RPCInterface::addUri(QString uri, QMap<QString, QVariant> opt, QString id)
{
    qDebug() << "[Aria2RPC] addUri function started";
    if (uri.isEmpty() || opt.isEmpty() || id.isEmpty()) {
        qWarning() << "[Aria2RPC] Invalid parameters for addUri";
        return false;
    }
    uri = processThunderUri(uri); //处理迅雷链接
    QJsonArray ja, inner;
    inner.append(uri); //可支持多个URI
    ja.append(inner);

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    bool result = callRPC(ARIA2C_METHOD_ADD_URI, ja, id);
    qDebug() << "[Aria2RPC] addUri function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::addNewUri(QString uri, QString savepath, QString filename, QString id)
{
    qDebug() << "[Aria2RPC] addNewUri function started";
    if (uri.isEmpty() || savepath.isEmpty() || filename.isEmpty() || id.isEmpty()) {
        qDebug() << "[Aria2RPC] addNewUri function ended with result: false";
        return false;
    }
    QMap<QString, QVariant> opt;
    opt.insert("dir", savepath);
    opt.insert("out", filename);

    bool result = addUri(uri, opt, id);
    qDebug() << "[Aria2RPC] addNewUri function ended with result:" << result;
    return result;

    //qDebug() << "Add new uri" << uri;
}

bool Aria2RPCInterface::addTorrent(QString torrentFile, QMap<QString, QVariant> opt, QString id)
{
    qDebug() << "[Aria2RPC] addTorrent function started";
    if (torrentFile.isEmpty() || opt.isEmpty() || id.isEmpty()) {
        qWarning() << "[Aria2RPC] Invalid parameters for addTorrent";
        return false;
    }
    QString torrentB64Str = fileToBase64(torrentFile); //把bt文件转成base64编码
    QJsonArray ja;
    ja.append(torrentB64Str);
    ja.append(QJsonArray());

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    bool result = callRPC(ARIA2C_METHOD_ADD_TORRENT, ja, id);
    qDebug() << "[Aria2RPC] addTorrent function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::addMetalink(QString metalink, QMap<QString, QVariant> opt, QString id)
{
    qDebug() << "[Aria2RPC] addMetalink function started";
    if (metalink.isEmpty() || opt.isEmpty() || id.isEmpty()) {
        qWarning() << "[Aria2RPC] Invalid parameters for addMetalink";
        return false;
    }
    QString metalinkB64Str = fileToBase64(metalink);
    QJsonArray ja;
    ja.append(metalinkB64Str);
    //ja.append(QJsonArray());

    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(opt));
    QJsonObject optJson = doc.object();
    ja.append(optJson);

    bool result = callRPC(ARIA2C_METHOD_ADD_METALINK, ja, id);
    qDebug() << "[Aria2RPC] addMetalink function ended with result:" << result;
    return result;
}

QString Aria2RPCInterface::fileToBase64(QString filePath)
{
    // qDebug() << "[Aria2RPC] fileToBase64 function started";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        // qDebug() << "[Aria2RPC] fileToBase64 function ended with empty result";
        return QString();
    }
    QByteArray ba = file.readAll();
    QString b64Str = ba.toBase64();
    // qDebug() << "[Aria2RPC] fileToBase64 function ended successfully";
    return b64Str;
}

bool Aria2RPCInterface::purgeDownloadResult(QString id)
{
    qDebug() << "[Aria2RPC] purgeDownloadResult function started";
    bool result = callRPC(ARIA2C_METHOD_PURGE_DOWNLOAD_RESULT, id);
    qDebug() << "[Aria2RPC] purgeDownloadResult function ended with result:" << result;
    return result;
}

Aria2cBtInfo Aria2RPCInterface::getBtInfo(QString torrentPath)
{
    qDebug() << "[Aria2RPC] getBtInfo function started";
    QProcess Proc; //进程调用指针
    QStringList opt;
    opt << "--show-files=true";
    opt << torrentPath;
    Proc.start(m_basePath + m_aria2cCmd, opt); //启动aria2c进程
    Proc.waitForFinished(); //等待执行完成

    QByteArray array = Proc.readAllStandardOutput(); //获取进程执行返回值
    Proc.close(); //关闭进程

    QStringList strList = QString(array).split("\n"); //将array内容分割开了

    Aria2cBtInfo btInfo;
    bool bFlag = false;
    QString temp = "";

    for (QString line : strList) {
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
    if (btInfo.totalLengthByets == 0) {
        qDebug() << "[Aria2RPC] getBtInfo function, totalLengthByets is 0, calculating totalLengthByets";
        for (int i = 0; i < btInfo.files.size(); i++) {
            btInfo.totalLengthByets += btInfo.files[i].lengthBytes;
        }
    }
    qDebug() << "[Aria2RPC] getBtInfo function ended";
    return btInfo;
}

bool Aria2RPCInterface::callRPC(QString method, QJsonArray params, QString id)
{
    // qDebug() << "[Aria2RPC] callRPC function started with method:" << method;
    QJsonObject json;
    params.prepend("token:" + getToken());
    json.insert("jsonrpc", "2.0");
    if (id.isEmpty()) {
        json.insert("id", method);
    } else {
        json.insert("id", id);
    }
    json.insert("method", method);
    if (!params.isEmpty()) {

        json.insert("params", params);
    }
    json.insert("", 0);
    bool result = sendMessage(json, method);
    // qDebug() << "[Aria2RPC] callRPC function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::callRPC(QString method, QString id)
{
    // qDebug() << "[Aria2RPC] callRPC function started with method:" << method;
    bool result = callRPC(method, QJsonArray(), id);
    // qDebug() << "[Aria2RPC] callRPC function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::sendMessage(QJsonObject jsonObj, const QString &method)
{
    qDebug() << "[Aria2RPC] sendMessage function started with method:" << method;
    QNetworkAccessManager *manager = new QNetworkAccessManager; //定义网络对象

    if (!jsonObj.isEmpty()) { //json如果不为空
        qDebug() << "[Aria2RPC] sendMessage function, jsonObj is not empty";
        QNetworkRequest requset; //定义请求对象
        requset.setUrl(QUrl(this->m_rpcServer)); //设置服务器的uri
        requset.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply *networkReply = manager->post(requset, QJsonDocument(jsonObj).toJson()); //post信息到服务器

        //调用返回的信息
        connect(manager,
                &QNetworkAccessManager::finished,
                this,
                [=](QNetworkReply *reply) {
                    this->rpcRequestReply(reply, method, jsonObj.value("id").toString()); //调用出来函数
                    manager->deleteLater(); //删除
                    //manager->destroyed();
                });

        //        connect(networkReply,
        //                QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
        //                [=](QNetworkReply::NetworkError error){
        //                    QByteArray buf = networkReply->readAll();
        //                    qDebug() << error << ":  " << buf;
        //                });
        //        connect(networkReply,
        //                &QNetworkReply::finished,
        //                [=](){
        //                    QByteArray buf = networkReply->readAll();
        //                    qDebug() << "finished" << ":  " << buf;
        //                });
        bool result = (networkReply->error() ? false : true);
        qDebug() << "[Aria2RPC] sendMessage function ended with result:" << result;
        return result;
    }

    if (manager != nullptr) {
        qDebug() << "[Aria2RPC] sendMessage function, manager is not null";
        manager->deleteLater();
        manager = nullptr;
    }

    qDebug() << "[Aria2RPC] sendMessage function ended with false";
    return false;
}

void Aria2RPCInterface::rpcRequestReply(QNetworkReply *reply, const QString &method, const QString id)
{
    qDebug() << "[Aria2RPC] rpcRequestReply function started with method:" << method;
    if (method == ARIA2C_METHOD_FORCE_PAUSE) {
        qDebug() << "[Aria2RPC] sendMessage function, method is FORCE_PAUSE";
        method.size();
    }
    if (method == ARIA2C_METHOD_PAUSE) {
        qDebug() << "[Aria2RPC] sendMessage function, method is PAUSE";
        method.size();
    }
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(); //请求返回的属性值
    QByteArray buf = reply->readAll(); //获取信息
    
    // Safety check: validate JSON parsing
    if (buf.isEmpty()) {
        qDebug() << "[Aria2RPC] Error: Empty response buffer for method:" << method;
        return;
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(buf, &parseError); //转换为json格式
    
    // Safety check: ensure JSON parsing succeeded
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "[Aria2RPC] Error: JSON parse failed for method:" << method 
                 << "Error:" << parseError.errorString() 
                 << "Offset:" << parseError.offset;
        return;
    }
    
    // Safety check: ensure document contains an object
    if (!doc.isObject()) {
        qDebug() << "[Aria2RPC] Error: JSON document is not an object for method:" << method;
        return;
    }
    
    QJsonObject obj = doc.object();
    
    // Safety check: ensure object is not empty
    if (obj.isEmpty()) {
        qDebug() << "[Aria2RPC] Error: JSON object is empty for method:" << method;
        return;
    }
    
    if (code == 200) { //返回正常
        qDebug() << "[Aria2RPC] sendMessage function, method is " << method << ", code is 200";
        emit RPCSuccess(method, obj);
    } else { //错误
        qDebug() << "[Aria2RPC] sendMessage function, method is " << method << ", code is not 200";
        emit RPCError(method, id, code, obj);
    }

    reply->deleteLater();
    //reply->destroyed();
    qDebug() << "[Aria2RPC] rpcRequestReply function ended";
}

bool Aria2RPCInterface::tellStatus(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] tellStatus function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_TELL_STATUS, ja, id);
    qDebug() << "[Aria2RPC] tellStatus function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::tellStatus(QString gId, QStringList keys, QString id)
{
    qDebug() << "[Aria2RPC] tellStatus function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    QJsonArray ka;
    for (QString k : keys) {
        ka.append(k);
    }
    ja.append(ka);
    bool result = callRPC(ARIA2C_METHOD_TELL_STATUS, ja, id);
    qDebug() << "[Aria2RPC] tellStatus function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::pause(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] pause function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_PAUSE, ja, id);
    qDebug() << "[Aria2RPC] pause function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::forcePause(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] forcePause function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_FORCE_PAUSE, ja, id);
    qDebug() << "[Aria2RPC] forcePause function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::pauseAll(QString id)
{
    qDebug() << "[Aria2RPC] pauseAll function started";
    bool result = callRPC(ARIA2C_METHOD_PAUSE_ALL, id);
    qDebug() << "[Aria2RPC] pauseAll function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::forcePauseAll(QString id)
{
    qDebug() << "[Aria2RPC] forcePauseAll function started";
    bool result = callRPC(ARIA2C_METHOD_FORCE_PAUSE_ALL, id);
    qDebug() << "[Aria2RPC] forcePauseAll function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::unpause(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] unpause function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_UNPAUSE, ja, id);
    qDebug() << "[Aria2RPC] unpause function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::unpauseAll(QString id)
{
    qDebug() << "[Aria2RPC] unpauseAll function started";
    bool result = callRPC(ARIA2C_METHOD_UNPAUSE_ALL, id);
    qDebug() << "[Aria2RPC] unpauseAll function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::remove(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] remove function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_REMOVE, ja, id);
    qDebug() << "[Aria2RPC] remove function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::forceRemove(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] forceRemove function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_FORCE_REMOVE, ja, id);
    qDebug() << "[Aria2RPC] forceRemove function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::removeDownloadResult(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] removeDownloadResult function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_REMOVE_DOWNLOAD_RESULT, ja, id);
    qDebug() << "[Aria2RPC] removeDownloadResult function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::getFiles(QString gId, QString id)
{
    qDebug() << "[Aria2RPC] getFiles function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    bool result = callRPC(ARIA2C_METHOD_GET_FILES, ja, id);
    qDebug() << "[Aria2RPC] getFiles function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::getGlobalSatat()
{
    qDebug() << "[Aria2RPC] getGlobalSatat function started";
    QJsonArray ja;
    bool result = callRPC(ARIA2C_METHOD_GET_GLOBAL_STAT, ja, "");
    qDebug() << "[Aria2RPC] getGlobalSatat function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::getGlobalOption()
{
    qDebug() << "[Aria2RPC] getGlobalOption function started";
    QJsonArray ja;
    bool result = callRPC(ARIA2C_METHOD_GET_GLOBAL_OPTION, ja, "");
    qDebug() << "[Aria2RPC] getGlobalOption function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::changeGlobalOption(QMap<QString, QVariant> options, QString id)
{
    qDebug() << "[Aria2RPC] changeGlobalOption function started";
    QJsonArray ja;
    QJsonDocument doc = QJsonDocument::fromVariant(QVariant(options));
    QByteArray jba = doc.toJson();
    QString jsonString = QString(jba);
    QByteArray njba = jsonString.toUtf8();
    QJsonObject nobj = QJsonObject(QJsonDocument::fromJson(njba).object());
    QJsonObject optJson = doc.object();
    ja.append(nobj);
    bool result = callRPC(ARIA2C_METHOD_CHANGE_GLOBAL_OPTION, ja, id);
    qDebug() << "[Aria2RPC] changeGlobalOption function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::changePosition(QString gId, int pos)
{
    qDebug() << "[Aria2RPC] changePosition function started with gId:" << gId;
    QJsonArray ja;
    ja.append(gId);
    ja.append(pos);
    ja.append("POS_SET");
    bool result = callRPC(ARIA2C_METHOD_CHANGE_POSITION, ja, gId);
    qDebug() << "[Aria2RPC] changePosition function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::modifyConfigFile(QString configItem, QString value)
{
    qDebug() << "[Aria2RPC] modifyConfigFile function started";

    QString strAll;
    QStringList strList;

    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName());

    QFile readFile(m_aria2configPath);
    if (readFile.open((QIODevice::ReadOnly | QIODevice::Text))) {
        qDebug() << "[Aria2RPC] Successfully opened config file for reading";
        QTextStream stream(&readFile);
        strAll = stream.readAll();
    } else {
        qDebug() << "[Aria2RPC] modifyConfigFile function ended with false";
        return false;
    }
    readFile.close();
    QFile writeFile(m_aria2configPath);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "[Aria2RPC] Successfully opened config file for writing";
        QTextStream stream(&writeFile);
        strList = strAll.split("\n");
        for (int i = 0; i < strList.count(); i++) {
            if (strList.at(i).contains(configItem)) {
                // qDebug() << "[Aria2RPC] Found config item to replace at line:" << i;
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

    qDebug() << "[Aria2RPC] modifyConfigFile function ended with true";
    return true;
}

bool Aria2RPCInterface::setMaxDownloadNum(QString maxDownload)
{
    qDebug() << "[Aria2RPC] setMaxDownloadNum function started: maxDownload:" << maxDownload;

    QMap<QString, QVariant> opt;
    QString value = "max-concurrent-downloads=" + maxDownload;
    modifyConfigFile("max-concurrent-downloads=", value);
    opt.insert("max-concurrent-downloads", maxDownload);

    bool result = changeGlobalOption(opt);
    qDebug() << "[Aria2RPC] setMaxDownloadNum function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::setDownloadUploadSpeed(QString downloadSpeed, QString uploadSpeed)
{
    qDebug() << "[Aria2RPC] setDownloadUploadSpeed function started: downloadSpeed:" << downloadSpeed << "uploadSpeed:" << uploadSpeed;
    
    QMap<QString, QVariant> opt;
    QString down_speed = downloadSpeed + "K";
    opt.insert("max-overall-download-limit", down_speed);

    QString upload_speed = uploadSpeed + "K";
    opt.insert("max-overall-upload-limit", upload_speed);
    changeGlobalOption(opt);
    QString value = "max-overall-download-limit=" + down_speed;
    modifyConfigFile("max-overall-download-limit=", value);

    value = "max-overall-upload-limit=" + upload_speed;
    bool result = modifyConfigFile("max-overall-upload-limit=", value);
    qDebug() << "[Aria2RPC] setDownloadUploadSpeed function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::SetDisckCacheNum(QString disckCacheNum)
{
    qDebug() << "[Aria2RPC] SetDisckCacheNum function started: disckCacheNum:" << disckCacheNum;

    QMap<QString, QVariant> opt;
    QString cacheNum = disckCacheNum + "M";
    opt.insert("disk-cache", cacheNum);
    changeGlobalOption(opt);
    QString value = "disk-cache=" + cacheNum;

    bool result = modifyConfigFile("disk-cache=", value);
    qDebug() << "[Aria2RPC] SetDisckCacheNum function ended with result:" << result;
    return result;
}

void Aria2RPCInterface::setDownloadLimitSpeed(QString downloadLimitSpeed)
{
    qDebug() << "[Aria2RPC] setDownloadLimitSpeed function started: downloadLimitSpeed:" << downloadLimitSpeed;
    QMap<QString, QVariant> opt;

    QString speed = downloadLimitSpeed + "K";

    opt.insert("max-overall-download-limit", speed);
    changeGlobalOption(opt);

    QString value = "max-overall-download-limit=" + speed;
    modifyConfigFile("max-overall-download-limit=", value);

    qDebug() << "set download limit speed:" << downloadLimitSpeed;
}

bool Aria2RPCInterface::setUploadLimitSpeed(QString UploadLimitSpeed)
{
    qDebug() << "[Aria2RPC] setUploadLimitSpeed function started: UploadLimitSpeed:" << UploadLimitSpeed;

    QMap<QString, QVariant> opt;
    QString speed = UploadLimitSpeed + "K";
    opt.insert("max-overall-upload-limit", speed);
    changeGlobalOption(opt);

    QString value = "max-overall-upload-limit=" + speed;
    bool result = modifyConfigFile("max-overall-upload-limit=", value);
    qDebug() << "[Aria2RPC] setUploadLimitSpeed function ended with result:" << result;
    return result;
}

QString Aria2RPCInterface::processThunderUri(QString thunder)
{
    qDebug() << "[Aria2RPC] processThunderUri function started: thunder:" << thunder;
    
    QString uri = thunder;
    if (thunder.startsWith("thunder://")) {
        qDebug() << "[Aria2RPC] URI is thunder protocol, decoding";
        QString oUir = thunder.mid(thunder.indexOf("thunder://") + 9 + 1);
        uri = QString(QByteArray::fromBase64(oUir.toLatin1()));
        uri = uri.mid(2, uri.length() - 4); //AA[URI]ZZ
    }

    qDebug() << "[Aria2RPC] processThunderUri function ended with uri:" << uri;
    return uri;
}

//QString Aria2RPCInterface::getCapacityFree(QString path)
//{
//    QProcess *proc = new QProcess;
//    QStringList opt;
//    opt << "-c";
//    opt << "df -h " + path;
//   // opt << "df";
//    proc->start("/bin/bash", opt);
//  //  proc->start("df", QStringList());
//    proc->waitForFinished();
//    QByteArray rt = proc->readAllStandardOutput();
//    proc->close();
//    delete proc;

//    QString free = "0B";
//    QStringList lt = QString(rt).split("\n");
//    if (lt.length() >= 2) {
//        QString line = lt.at(1);
//        QString temp;
//        QStringList tpl;
//        for (int i = 0; i < line.length(); i++) {
//            if (line[i] != ' ') {
//                temp.append(line[i]);
//            } else {
//                if (temp != "") {
//                    tpl.append(temp);
//                    temp = "";
//                }
//            }
//        }
//        free = tpl[3];
//    }
//    return free + "B";
//}

QString Aria2RPCInterface::getCapacityFree(QString path)
{
    qDebug() << "[Aria2RPC] getCapacityFree function started: path:" << path;

    QProcess *proc = new QProcess;
    QStringList opt;
    opt << "-h" << path;
    // opt << "df";
    proc->start("df", opt);
    //  proc->start("df", QStringList());
    proc->waitForFinished();
    QByteArray rt = proc->readAllStandardOutput();
    proc->close();
    delete proc;

    QString free = "0B";
    QStringList lt = QString(rt).split("\n");
    if (lt.length() >= 2) {
        qDebug() << "[Aria2RPC] Found at least 2 lines in df output";
        QString line = lt.at(1);
        QString temp;
        QStringList tpl;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                temp.append(line[i]);
            } else {
                if (!temp.isEmpty()) {
                    tpl.append(temp);
                    temp = "";
                }
            }
        }
        free = tpl[3];
    }

    qDebug() << "[Aria2RPC] getCapacityFree function ended with result:" << (free + "B");
    return free + "B";
}

long Aria2RPCInterface::getCapacityFreeByte(QString path)
{
    qDebug() << "[Aria2RPC] getCapacityFreeByte function started: path:" << path;

    QProcess *proc = new QProcess;
    QStringList opt;
    opt << path;
    proc->start("df", opt);
    proc->waitForFinished();
    QByteArray rt = proc->readAllStandardOutput();
    proc->close();
    delete proc;

    QString free = "0";
    QStringList lt = QString(rt).split("\n");
    if (lt.length() >= 2) {
        qDebug() << "[Aria2RPC] Found at least 2 lines in df output";
        QString line = lt.at(1);
        QString temp;
        QStringList tpl;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                temp.append(line[i]);
            } else {
                if (!temp.isEmpty()) {
                    tpl.append(temp);
                    temp = "";
                }
            }
        }
        free = tpl[3];
    }

    qDebug() << "[Aria2RPC] getCapacityFreeByte function ended with result:" << free.toLong();
    return free.toLong();
}

QString Aria2RPCInterface::getBtToMetalink(QString filePath)
{
    qDebug() << "[Aria2RPC] getBtToMetalink function started: filePath:" << filePath;
    
    QString strMetaLink = ""; //磁力链

    QFile file(filePath); //strFilePath文件的绝对路径
    if (file.open(QIODevice::ReadOnly)) //只读方式打开
    {
        qDebug() << "[Aria2RPC] Successfully opened file for reading";
        QCryptographicHash hash(QCryptographicHash::Sha1);
        if (!file.atEnd()) {
            qDebug() << "[Aria2RPC] File not empty, calculating hash";
            hash.addData(file.readAll());
            QString stHashValue;
            stHashValue.append(hash.result().toHex());
            return stHashValue;
        }
    }

    qDebug() << "[Aria2RPC] getBtToMetalink function ended with empty result";
    return strMetaLink;
}

QString Aria2RPCInterface::bytesFormat(qint64 size)
{
    qDebug() << "[Aria2RPC] bytesFormat function started: size:" << size;
    
    if (!size) {
        qDebug() << "[Aria2RPC] Size is 0, returning 0B";
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
    QString result = QString::number(size * 1.0 / qPow(1024, qFloor(i)), 'f', (i > 1) ? 2 : 0) + sl.at(i);
    qDebug() << "[Aria2RPC] bytesFormat function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::shutdown(QString id)
{
    qDebug() << "[Aria2RPC] shutdown function started: id:" << id;
    bool result = callRPC(ARIA2C_METHOD_SHUTDOWN, id);
    qDebug() << "[Aria2RPC] shutdown function ended with result:" << result;
    return result;
}

bool Aria2RPCInterface::forceShutdown(QString id)
{
    qDebug() << "[Aria2RPC] forceShutdown function started: id:" << id;
    bool result = callRPC(ARIA2C_METHOD_FORCE_SHUTDOWN, id);
    qDebug() << "[Aria2RPC] forceShutdown function ended with result:" << result;
    return result;
}

QString Aria2RPCInterface::getToken()
{
    // qDebug() << "[Aria2RPC] getToken function started";
    static QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    // qDebug() << "[Aria2RPC] getToken function ended with token:" << uuid;
    return uuid;
}

bool Aria2RPCInterface::setupConfig()
{
    qDebug() << "[Aria2RPC] setupConfig function started";

    //定义配置文件路径
    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName());

    //判断文件是否存在,如果不存在复制配置文件内容到目录下
    QFileInfo fileInfo(m_aria2configPath);
    if (!fileInfo.exists()) {
        qDebug() << "[Aria2RPC] Config file does not exist, copying from template";
        QFile::copy(ARIA_CONFIG_PATH, m_aria2configPath);
    }

    //设置配置文件路径
    setConfigFilePath(m_aria2configPath);
    qDebug() << "[Aria2RPC] setupConfig function ended with true";
    return true;
}
