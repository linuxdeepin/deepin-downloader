#include "extensionservice.h"

#include <QWebSocketServer>
#include <QWebChannel>
#include <QWebSocket>
#include <QSharedMemory>
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
    initWebsokcet();
}

extensionService::~extensionService()
{
    delete m_timer;
}

void extensionService::initWebsokcet()
{
    m_server = new QWebSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
    if (!m_server->listen(QHostAddress("127.0.0.1"), 12345)) {
      //  qFatal("Failed to open web socket server.");
    }
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
//    m_timer = new QTimer;
//    m_timer->start(5000);
//    connect(m_timer, &QTimer::timeout, this, &extensionService::checkConnection);
}

void extensionService::sendUrlToDownloader(const QString &url)
{
    QProcess p;
    p.startDetached("downloader " + url);
    QTimer *t = new QTimer;
    t->start(50);
    connect(t, &QTimer::timeout, this, [=](){
       QDBusInterface iface("com.downloader.service",
                             "/downloader/path",
                             "local.downloader.MainFrame",
                             QDBusConnection::sessionBus());
       QDBusMessage m = iface.call("onReceiveExtentionUrl", url);
       QString msg = m.errorMessage();
       if(msg.isEmpty()) {
           t->stop();
       }
    });
}

void extensionService::checkConnection()
{
    QProcess p;
    QStringList options;
    options << "-c" << "netstat -apn | grep dlmextensions";
    p.start("/bin/bash", options);
    p.waitForFinished();
    QList<QByteArray> strList = p.readAllStandardOutput().split('\n');
    for(QString str : strList) {
        if(str.contains("ESTABLISHED")) {
            return;
        }
    }
    qApp->exit(0);
}

void extensionService::freeSharedMem()
{
    QSharedMemory sharedMemory("dlmExtensionService");
    if (sharedMemory.attach()) {
        if (sharedMemory.isAttached()) {
            sharedMemory.detach();
        }
    }
}
