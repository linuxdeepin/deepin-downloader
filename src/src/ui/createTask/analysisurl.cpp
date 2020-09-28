#include "analysisurl.h"
#include <QThread>
#include <QDebug>
#include <QMetaType>
#include <QMutex>

AnalysisUrl::AnalysisUrl(QObject *parent)
    : QObject(parent)
{
    //qRegisterMetaType<LinkInfo>("LinkInfo*");
}

AnalysisUrl::~AnalysisUrl()
{
    QMap<int, QThread *>::iterator workIt = m_workThread.begin();
    for (;workIt != m_workThread.end();) {
        QThread *value = workIt.value();
        if(value == nullptr){
            value->requestInterruption();
            value->exit();
            value->quit();
            value->wait();

            delete value;
            value = nullptr;
        }
        m_workThread.erase(workIt++);
    }
    QMap<int, UrlThread *>::iterator urlIt = m_urlThread.begin();
    for (; urlIt != m_urlThread.end();) {
        UrlThread *value = urlIt.value();
        if(urlIt.value() == nullptr){
            delete value;
            value = nullptr;
        }
        m_urlThread.erase(urlIt++);
    }
}

void AnalysisUrl::setUrlList(QMap<QString, LinkInfo> list)
{
    if (list.isEmpty()) {
        return;
    }
    QMap<QString, LinkInfo>::iterator it = m_curAllUrl.begin();
    QMap<QString, LinkInfo>::iterator sameLink;
    for (; it != m_curAllUrl.end();) {
        if (list.contains(it.key())) {
            it.value().index = list.value(it.key()).index;
            *(m_curAllUrl.find(it.key())) = it.value();
            emit sendFinishedUrl(&it.value());
            it++;
        } else {
            stopWork(it.value().index);
            it = m_curAllUrl.erase(it);
        }
    }

    it = list.begin();

    for (; it != list.end(); it++) {
        if (m_curAllUrl.contains(it.key())) {
        } else {
            m_curAllUrl.insert(it.key(), it.value());
        }
    }
    it = m_curAllUrl.begin();

    static QMutex mutex;
    bool isLock = mutex.tryLock();
    if (isLock) {
        for (; it != m_curAllUrl.end(); it++) {
            if (it->state != LinkInfo::UrlState::Finished) {
                LinkInfo info;
                info.url = it.key();
                info.index = it.value().index;
                UrlThread *url= new UrlThread;
                QThread *thread = new QThread;
                m_workThread.insert(info.index, thread);
                m_urlThread.insert(info.index,url);
                url->moveToThread(thread);
                url->start(info);
                connect(thread, SIGNAL(started()), url, SLOT(begin()));
                connect(url, SIGNAL(sendFinishedUrl(LinkInfo)), this, SLOT(getLinkInfo(LinkInfo)),Qt::QueuedConnection);
                connect(url, SIGNAL(sendTrueUrl(LinkInfo)), this, SLOT(getTrueLinkInfo(LinkInfo)),Qt::QueuedConnection);
                thread->start();
            }
        }
    }
    mutex.unlock();
}

void AnalysisUrl::getLinkInfo(LinkInfo linkInfo)
{
    static QMutex mutex;
    bool isLock = mutex.tryLock();
    if (isLock) {
        QMap<QString, LinkInfo>::iterator it = m_curAllUrl.find(linkInfo.url);
        QMap<QString, LinkInfo>::iterator iter = m_curAllUrl.begin();
         for (; iter != m_curAllUrl.end(); iter++) {
             if(iter.value().urlTrueLink == linkInfo.urlTrueLink && !linkInfo.urlTrueLink.isEmpty()){
                iter.value().type = linkInfo.type;
                iter.value().state = linkInfo.state;
                iter.value().length = linkInfo.length;
                iter.value().urlName = linkInfo.urlName;
                iter.value().urlSize = linkInfo.urlSize;

                emit sendFinishedUrl(&linkInfo);
                mutex.unlock();
                return;
             }

         }
        if(it==m_curAllUrl.end())
        {
            mutex.unlock();
            return;
        }
        it.value().type = linkInfo.type;
        it.value().state = linkInfo.state;
        it.value().length = linkInfo.length;
        it.value().urlName = linkInfo.urlName;
        it.value().urlSize = linkInfo.urlSize;

        emit sendFinishedUrl(&linkInfo);
    }
    mutex.unlock();
}

void AnalysisUrl::stopWork(int index)
{
    static QMutex mutex;
    bool isLock = mutex.tryLock();

    if (isLock) {

        QMap<int,QThread*>::iterator it=m_workThread.find(index);
        QMap<int,UrlThread*>::iterator urlIt=m_urlThread.find(index);
        if(urlIt.value() == nullptr){
            return;
        }
        if(it==m_workThread.end())
            return ;

        QThread *thread = m_workThread.value(index);
        if(thread == nullptr){
            return;
        }

        thread->requestInterruption();
        thread->quit();
        thread->wait(10);
        delete thread;
        delete urlIt.value();
        urlIt.value() = nullptr;

        mutex.unlock();
    }

}

void AnalysisUrl::getTrueLinkInfo(LinkInfo linkInfo)
{
    QMap<QString, LinkInfo>::iterator it = m_curAllUrl.find(linkInfo.url);
    if(it==m_curAllUrl.end()){
        return;
    }
    it->urlTrueLink = linkInfo.urlTrueLink;
}


