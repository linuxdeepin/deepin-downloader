// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "analysisurl.h"
#include "urlthread.h"
#include <QThread>
#include <QDebug>
#include <QMetaType>
#include <QMutex>

AnalysisUrl::AnalysisUrl(QObject *parent)
    : QObject(parent)
{
    // qDebug() << "AnalysisUrl constructor";
    //qRegisterMetaType<LinkInfo>("LinkInfo*");
}

AnalysisUrl::~AnalysisUrl()
{
    // qDebug() << "AnalysisUrl destructor, stopping" << m_workThread.size() << "threads";
    for (int i = 0; i < m_workThread.size(); i++) {
        stopWork(i);
    }
}

void AnalysisUrl::setUrlList(QMap<QString, LinkInfo> list)
{
    qDebug() << "setUrlList called with" << list.size() << "urls";
    if (list.isEmpty()) {
        qWarning() << "Empty url list provided";
        return;
    }
    QMap<QString, LinkInfo>::iterator it = m_curAllUrl.begin();
    QMap<QString, LinkInfo>::iterator sameLink;
    for (; it != m_curAllUrl.end();) {
        if (list.contains(it.key())) {
            // qDebug() << "[AnalysisUrl] Found existing URL, updating index:" << it.key();
            it.value().index = list.value(it.key()).index;
            *(m_curAllUrl.find(it.key())) = it.value();
            emit sendFinishedUrl(&it.value());
            it++;
        } else {
            // qDebug() << "[AnalysisUrl] URL not in new list, stopping work and removing:" << it.key();
            stopWork(it.value().index);
            it = m_curAllUrl.erase(it);
        }
    }

    it = list.begin();

    for (; it != list.end(); it++) {
        if (m_curAllUrl.contains(it.key())) {
            // qDebug() << "[AnalysisUrl] URL already exists in current list, skipping insert:" << it.key();
        } else {
            // qDebug() << "[AnalysisUrl] Adding new URL to current list:" << it.key();
            m_curAllUrl.insert(it.key(), it.value());
        }
    }
    it = m_curAllUrl.begin();

    static QMutex mutex;
    bool isLock = mutex.tryLock();
    if (isLock) {
        // qDebug() << "[AnalysisUrl] Mutex locked, processing URLs";
        for (; it != m_curAllUrl.end(); it++) {
            if (it->state != LinkInfo::UrlState::Finished) {
                // qDebug() << "[AnalysisUrl] Creating new thread for unfinished URL:" << it.key();
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
    qDebug() << "[AnalysisUrl] setUrlList function ended";
}

void AnalysisUrl::getLinkInfo(LinkInfo linkInfo)
{
    qDebug() << "getLinkInfo for url:" << linkInfo.url << "state:" << linkInfo.state;

    static QMutex mutex;
    bool isLock = mutex.tryLock();
    if (isLock) {
        qDebug() << "[AnalysisUrl] Mutex locked, processing link info";
        QMap<QString, LinkInfo>::iterator it = m_curAllUrl.find(linkInfo.url);
        QMap<QString, LinkInfo>::iterator iter = m_curAllUrl.begin();
         for (; iter != m_curAllUrl.end(); iter++) {
             if(iter.value().urlTrueLink == linkInfo.urlTrueLink && !linkInfo.urlTrueLink.isEmpty()){
                // qDebug() << "[AnalysisUrl] Found matching true link, updating info";
                iter.value().type = linkInfo.type;
                iter.value().state = linkInfo.state;
                iter.value().length = linkInfo.length;
                iter.value().urlName = linkInfo.urlName;
                iter.value().urlSize = linkInfo.urlSize;
                emit sendFinishedUrl(&linkInfo);
                stopWork(iter.value().index);
                mutex.unlock();
                qDebug() << "[AnalysisUrl] getLinkInfo function ended early (true link match)";
                return;
             }

         }
        if(it==m_curAllUrl.end())
        {
            qDebug() << "[AnalysisUrl] URL not found in current list";
            mutex.unlock();
            return;
        }
        it.value().type = linkInfo.type;
        it.value().state = linkInfo.state;
       it.value().length = linkInfo.length;
        it.value().urlName = linkInfo.urlName;
        it.value().urlSize = linkInfo.urlSize;
        emit sendFinishedUrl(&linkInfo);
        stopWork(it.value().index);
    }
    mutex.unlock();
    qDebug() << "[AnalysisUrl] getLinkInfo function ended";
}

void AnalysisUrl::stopWork(int index)
{
    qDebug() << "stopWork for thread index:" << index;

    static QMutex mutex;
    bool isLock = mutex.tryLock();
    if (isLock) {
        qDebug() << "[AnalysisUrl] Mutex locked, stopping work";
        QMap<int,QThread*>::iterator it=m_workThread.find(index);
        if(it == m_workThread.end()){
            qDebug() << "[AnalysisUrl] Thread not found for index:" << index;
            mutex.unlock();
            return;
        }
        QMap<int,UrlThread*>::iterator urlIt=m_urlThread.find(index);
        if(urlIt == m_urlThread.end()){
            qDebug() << "[AnalysisUrl] URL thread not found for index:" << index;
            mutex.unlock();
            return;
        }
        if(urlIt.value() == nullptr){
            qDebug() << "[AnalysisUrl] URL thread is null for index:" << index;
            mutex.unlock();
            return;
        }
        if(it==m_workThread.end()){
            qDebug() << "[AnalysisUrl] Work thread not found (second check) for index:" << index;
            mutex.unlock();
            return ;
        }

        QThread *thread = m_workThread.value(index);
        if(thread == nullptr){
            qDebug() << "[AnalysisUrl] Thread is null for index:" << index;
            mutex.unlock();
            return;
        }

        thread->requestInterruption();
        thread->quit();
        thread->wait(100);
        delete thread;
        delete urlIt.value();
        urlIt.value() = nullptr;
        mutex.unlock();
    }
    qDebug() << "[AnalysisUrl] stopWork function ended";
}

void AnalysisUrl::getTrueLinkInfo(LinkInfo linkInfo)
{
    qDebug() << "getTrueLinkInfo original:" << linkInfo.url << "true:" << linkInfo.urlTrueLink;

    static QMutex mutex;
    mutex.lock();
    QMap<QString, LinkInfo>::iterator it = m_curAllUrl.find(linkInfo.url);
    if(it==m_curAllUrl.end()){
        qDebug() << "[AnalysisUrl] Original URL not found in current list:" << linkInfo.url;
        mutex.unlock();
        return;
    }
    m_curAllUrl.erase(it);
    m_curAllUrl.insert(linkInfo.urlTrueLink, linkInfo);
    mutex.unlock();
    qDebug() << "[AnalysisUrl] getTrueLinkInfo function ended";
}


