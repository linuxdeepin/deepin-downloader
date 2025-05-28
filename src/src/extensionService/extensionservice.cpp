// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2021-2021 Uniontech Technology Co., Ltd.
 *
 * @file extensionservice.cpp
 *
 * @brief Websocket服务
 *
 * @date 2021-06-29 16:00
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
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
**/


#include "extensionservice.h"

#include <QWebSocketServer>
#include <QWebChannel>
#include <QWebSocket>
#include <QProcess>
#include <QTimer>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QCoreApplication>
#include <QDebug>

#include "websocketclientwrapper.h"
#include "websockettransport.h"
#include "websockethandle.h"

extensionService::extensionService()
{
    qDebug() << "[ExtensionService] Initializing WebSocket service";
    initWebsokcet();
}

extensionService::~extensionService()
{
    qDebug() << "[ExtensionService] Destroying WebSocket service";
    delete m_timer;
}

void extensionService::initWebsokcet()
{
    qDebug() << "[ExtensionService] Initializing WebSocket server";

    m_server = new QWebSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
    if (!m_server->listen(QHostAddress("127.0.0.1"), 12345)) {
        qWarning() << "[ExtensionService] Failed to start WebSocket server on port 12345";
        qFatal("Failed to open web socket server.");
    }
    qDebug() << "[ExtensionService] WebSocket server started successfully";
    WebSocketClientWrapper* clientWrapper = new WebSocketClientWrapper(m_server);
    QWebChannel* channel = new QWebChannel;
    QObject::connect(clientWrapper, &WebSocketClientWrapper::clientConnected,
                     channel, &QWebChannel::connectTo);
    Websockethandle* core = new Websockethandle;
    channel->registerObject(QStringLiteral("core"), core);
    connect(core, &Websockethandle::sendWebText, this, [&](QString text) {
         QTimer::singleShot(50, this, [=](){
             sendUrlToDownloader(text);
         });
    });
    m_timer = new QTimer;
    m_timer->start(60000);
    connect(m_timer, &QTimer::timeout, this, &extensionService::checkConnection);
}

void extensionService::sendUrlToDownloader(const QString &url)
{
    qDebug() << "[ExtensionService] Sending URL to downloader:" << url;

    QProcess proc;
    proc.startDetached("downloader " + url);
    QTimer *timer = new QTimer;
    timer->start(50);
    connect(timer, &QTimer::timeout, this, [=](){
       QDBusInterface iface("com.downloader.service",
                             "/downloader/path",
                             "local.downloader.MainFrame",
                             QDBusConnection::sessionBus());
       QDBusMessage m = iface.call("onReceiveExtentionUrl", url);
       QString msg = m.errorMessage();
       if(msg.isEmpty()) {
           qDebug() << "[ExtensionService] URL successfully sent to downloader";
           timer->stop();
       }
    });
}

void extensionService::checkConnection()
{
    qDebug() << "[ExtensionService] Checking WebSocket connections";

    QProcess p;
    QStringList options;
    options << "-c" << "netstat -apn | grep dlmextensions";
    p.start("/bin/bash", options);
    p.waitForFinished();
    QList<QByteArray> strList = p.readAllStandardOutput().split('\n');
    for(QString str : strList) {
        if(str.contains("ESTABLISHED")) {  //存在websocket链接
            qDebug() << "[ExtensionService] Active WebSocket connection found";
            return;
        }
    }
    qDebug() << "[ExtensionService] No active connections, exiting";
    qApp->exit(0);
}
