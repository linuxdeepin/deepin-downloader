#include "websockethandle.h"

#include <QSettings>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSharedMemory>
#include <QDBusConnection>

Websockethandle::Websockethandle(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().unregisterService("com.dlmExtensionService.service");
    QDBusConnection::sessionBus().registerService("com.dlmExtensionService.service");
    QDBusConnection::sessionBus().registerObject("/dlmExtensionService/path", this, QDBusConnection ::ExportAllSlots | QDBusConnection ::ExportAllSignals);
}

Websockethandle::~Websockethandle()
{
}

bool Websockethandle::isControlBrowser()
{
    QString iniConfigPath = QString("%1/%2/%3/config.conf")
                                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                .arg("uos")
                                .arg("downloader");
    QSettings set(iniConfigPath, QSettings::IniFormat);

    bool b = set.value("Monitoring.MonitoringObject.Browser/value").toBool();
    return b;
}

void Websockethandle::sendTextToClient(bool b)
{
    sendText(QString().number(b));
}

void Websockethandle::freeSharedMem()
{
    QSharedMemory sharedMemory("dlmExtensionService");
    if (sharedMemory.attach()) {
        if (sharedMemory.isAttached()) {
            sharedMemory.detach();
        }
    }
}

void Websockethandle::receiveText(const QString &text)
{
    if(text.contains("init")) { //初始化
        bool b = isControlBrowser();
        sendText(QString().number(b));
        return;
    } /*else if(text.contains("close")) { //初始化
        freeSharedMem();
        qApp->exit(0);
    }*/
    sendWebText(text);  //发送链接到下载器
}
