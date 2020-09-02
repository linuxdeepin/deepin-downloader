#include "urlthread.h"
#include "analysisurl.h"
#include "aria2rpcinterface.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QMutex>
#include <QThread>
//#incldue <Settings>

UrlThread::UrlThread(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<LinkInfo>("LinkInfo");
}

void UrlThread::start(LinkInfo &urlInfo)
{
    m_linkInfo = new LinkInfo;
    *m_linkInfo = urlInfo;
}

UrlThread::~UrlThread()
{
}

void UrlThread::begin()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest requset; // 定义请求对象
    requset.setUrl(QUrl(m_linkInfo->url)); // 设置服务器的uri
    requset.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager->head(requset);
    // post信息到服务器
    QObject::connect(manager, &QNetworkAccessManager::finished, this, &UrlThread::onHttpRequest, Qt::UniqueConnection);
    QThread::usleep(100);
}

void UrlThread::onHttpRequest(QNetworkReply *reply)
{
    m_linkInfo->state = LinkInfo::UrlState::Finished;
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch (statusCode) {
    case 200: // redirect (Location: [URL])   真实链接
    {
        QProcess *process = new QProcess;
        QStringList list;
        list << "-i" << reply->url().toString();
        process->start("curl", list);
        connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
            static QMutex mutex;
            bool isLock = mutex.tryLock();
            if (isLock) {
                QString str = process->readAllStandardOutput();
                process->kill();
                process->close();
                QStringList urlList;
                urlList.append(process->arguments().at(1));
                process->deleteLater();
                m_linkInfo->urlSize = getUrlSize(str);
                m_linkInfo->type = getUrlType(str);
                if (!str.contains("Content-Disposition: attachment;filename=")) // 为200的真实链接
                {
                    emit sendFinishedUrl(*m_linkInfo);
                    mutex.unlock();
                    return;
                }
                QStringList urlInfoList = str.split("\r\n");
                for (int i = 0; i < urlInfoList.size(); i++) {
                    if (urlInfoList[i].startsWith("Content-Disposition:")) //为405链接
                    {
                        int start = urlInfoList[i].lastIndexOf("'");
                        QString urlName = urlInfoList[i].mid(start);
                        QString encodingUrlName = QUrl::fromPercentEncoding(urlName.toUtf8());
                        m_linkInfo->urlName = encodingUrlName;
                        emit sendFinishedUrl(*m_linkInfo);
                    }
                }
                mutex.unlock();
            }
        });
        break;
    }
    case 301:
    case 302: // redirect (Location: [URL])  重定向链接
    {
        QProcess *process = new QProcess;
        QStringList list;
        list << "-i" << reply->url().toString();
        process->start("curl", list);
        connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
            static QMutex mutex;
            bool isLock = mutex.tryLock();
            if (isLock) {
                QProcess *proc = dynamic_cast<QProcess *>(sender());
                QString str = proc->readAllStandardOutput();
                proc->deleteLater();
                QString strUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
                QStringList strList = strUrl.split("/");
                QStringList strUrlName = strList[strList.size() - 1].split(".");
                //需要转两次
                QString encodingUrlName = strUrlName[0]; //QUrl::fromPercentEncoding(strUrlName[0].toUtf8());
                m_linkInfo->urlName = encodingUrlName;
                QStringList urlStrList = QStringList(strUrl);
                m_linkInfo->type = getUrlType(str);
                emit sendFinishedUrl(*m_linkInfo);
                //onDownloadNewUrl(strUrl, Settings::getInstance()->getCustomFilePath(), encodingUrlName, type);
                proc->kill();
                proc->close();
                mutex.unlock();
            }
        });
        break;
    }
    case 404: {
        emit sendFinishedUrl(*m_linkInfo);
        break;
    }
    case 405: //405链接
    {
        QProcess *process = new QProcess;
        QStringList list;
        list << "-i" << reply->url().toString();
        process->start("curl", list);
        connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
            static QMutex mutex;
            bool isLock = mutex.tryLock();
            if (isLock) {
                QProcess *proc = dynamic_cast<QProcess *>(sender());
                proc->kill();
                proc->close();
                QString str = proc->readAllStandardOutput();
                proc->deleteLater();
                QStringList urlInfoList = str.split("\r\n");
                for (int i = 0; i < urlInfoList.size(); i++) {
                    if (urlInfoList[i].startsWith("Content-Disposition:")) //为405链接
                    {
                        int start = urlInfoList[i].lastIndexOf("'");
                        QString urlName = urlInfoList[i].mid(start);
                        QString urlNameForZH = QUrl::fromPercentEncoding(urlName.toUtf8());
                        // emit NewDownload_sig(QStringList(redirecUrl),m_defaultDownloadDir,_urlNameForZH);
                        QStringList strList = QStringList(urlInfoList[i]);
                        // onDownloadNewUrl(urlInfoList[i], Settings::getInstance()->getCustomFilePath(), urlNameForZH);
                        mutex.unlock();
                        return;
                    }
                }
                mutex.unlock();
            }
        });

        break;
    }
    default: {
        emit sendFinishedUrl(*m_linkInfo);
        break;
    }
    }
}

QString UrlThread::getUrlType(QString url)
{
    QStringList urlInfoList = url.split("\r\n");
    for (int i = 0; i < urlInfoList.size(); i++) {
        if (urlInfoList[i].startsWith("content-type:", Qt::CaseInsensitive)) {
            QString type;
            QByteArray ba = urlInfoList[i].split("/")[1].toLatin1();
            const char *s = ba.data();
            while (*s) {
                if ((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z')) {
                    type.append(*s);
                } else {
                    break;
                }
                s++;
            }
            return type;
        }
    }
    return "";
}

QString UrlThread::getUrlSize(QString url)
{
    QStringList urlInfoList = url.split("\r\n");
    for (int i = 0; i < urlInfoList.size(); i++) {
        if (urlInfoList[i].startsWith("Content-Length:", Qt::CaseInsensitive)) {
            int size = urlInfoList[i].split(" ")[1].toLatin1().toInt();
            m_linkInfo->length = size;
            return Aria2RPCInterface::instance()->bytesFormat(size);
        }
    }
    return "";
}
