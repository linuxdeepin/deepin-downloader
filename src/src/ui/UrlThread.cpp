#include "UrlThread.h"
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

UrlThread::UrlThread(QString url,QObject *parent)
    : QObject(parent)
    , m_url(url)
{
    getTruetUrl();
}



void UrlThread::getTruetUrl()
{
    if(isMagnet(m_url))
    {
        emit sendUrl(m_url, "");
        return;
    }
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest *requset = new QNetworkRequest;                       // 定义请求对象
    QString _trueUrl;
    requset->setUrl(QUrl(m_url)); // 设置服务器的uri
    requset->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //manager->get(*requset);
    manager->head(*requset);
    // post信bool ret = true;息到服务器
    QObject::connect(manager,
                     &QNetworkAccessManager::finished,
                     this,
                     [ = ](QNetworkReply *reply){
                        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        //QString length = reply->attribute(QNetworkRequest::OriginalContentLengthAttribute).toString();
                        qDebug() << "FINISHED" << statusCode;
                        switch(statusCode) {
                            case 200: // redirect (Location: [URL])   真实链接
                            {
                                QProcess *p = new QProcess;
                                QStringList _list;
                                _list<<"-i"<< m_url;
                                p->start("curl", _list);
                                p->waitForReadyRead();
                                QString _str = p->readAllStandardOutput();
                                if(!_str.contains("Content-Disposition: attachment;filename="))  // 为200的真实链接
                                {
                                     //emit NewDownload_sig(QStringList(redirecUrl),m_defaultDownloadDir,"");
                                    emit sendUrl(m_url, "");
                                    return ;
                                }
                                QStringList _urlInfoList = _str.split("\r\n");
                                for (int i = 0; i < _urlInfoList.size(); i++)
                                {
                                    if(_urlInfoList[i].startsWith("Content-Disposition:"))  //为405链接
                                    {
                                        int _start= _urlInfoList[i].lastIndexOf("'");
                                        QString _urlName = _urlInfoList[i].mid(_start);
                                        QString _urlNameForZH = QUrl::fromPercentEncoding(_urlName.toUtf8());
                                      //  emit NewDownload_sig(QStringList(redirecUrl),m_defaultDownloadDir,_urlNameForZH);
                                        emit sendUrl(m_url, _urlNameForZH);
                                        return ;
                                    }
                                }
                                break;
                            }
                            case 302: // redirect (Location: [URL])  重定向链接
                            {
                                QString strUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
                               // emit NewDownload_sig(QStringList(strUrl),m_defaultDownloadDir, "");
                                emit sendUrl(strUrl, "");
                               break;
                            }
                            case 405:   //405链接
                            {
                                    QProcess *p = new QProcess;
                                    QStringList _list;
                                    _list<<"-i"<< m_url;
                                    p->start("curl", _list);
                                    p->waitForReadyRead();
                                    QString _str = p->readAllStandardOutput();
                                    QStringList _urlInfoList = _str.split("\r\n");
                                    for (int i = 0; i < _urlInfoList.size(); i++)
                                    {
                                        if(_urlInfoList[i].startsWith("Content-Disposition:"))  //为405链接
                                        {
                                            int _start= _urlInfoList[i].lastIndexOf("'");
                                            QString _urlName = _urlInfoList[i].mid(_start);
                                            QString _urlNameForZH = QUrl::fromPercentEncoding(_urlName.toUtf8());
                                           // emit NewDownload_sig(QStringList(redirecUrl),m_defaultDownloadDir,_urlNameForZH);
                                            emit sendUrl(m_url, _urlNameForZH);
                                            return ;
                                        }
                                    }
                                    break;
                            }
                            default:
                            {
                                //QString warning_msg = QString(tr("%1\nThe address you entered cannot be resolved correctly")).arg(m_url);
//                                MessageBox *msg = new MessageBox();
//                                msg->setWarings(warning_msg, tr("sure"), "");
//                                msg->exec();
                                emit sendUrl("", "");
                                return;
                            }

                        }

    });
}

bool UrlThread::isMagnet(QString url)
{
    QString _str = url;
    if(_str.mid(0,20) == "magnet:?xt=urn:btih:")
    {
        return  true;
    }
    else
    {
        return  false;
    }
}
