#ifndef URLTHREAD_H
#define URLTHREAD_H

#include "analysisurl.h"
#include <QObject>
class LinkInfo;
class QThread;
class QNetworkReply;

class UrlThread : public QObject
{
    Q_OBJECT

public:
    explicit UrlThread(QObject *parent = nullptr);
    ~UrlThread();

    void start(LinkInfo &urlInfo);


public slots:
    void begin();

    void onHttpRequest(QNetworkReply *reply);

private:
    QString getUrlType(QString url);
    QString getUrlSize(QString url);

signals:
    void sendFinishedUrl(LinkInfo);
    void send();

private:
    LinkInfo *m_linkInfo;  //当前url信息5
    QThread *m_thread;

};

#endif // URLTHREAD_H
