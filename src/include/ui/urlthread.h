/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file urlthread.h
 *
 * @brief 解析任务线程类
 *
 * @date 2020-08-28 14:42
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
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
    LinkInfo *m_linkInfo; //当前url信息5
    QThread *m_thread;
};

#endif // URLTHREAD_H
