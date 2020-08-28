#include "func.h"
#include <QProcess>
#include <QDir>

//Func::Func()
//{
//}

//Func &Func::Instance()
//{
//    static Func *f = nullptr;
//    if (f == nullptr) {
//        f = new Func();
//    }
//    return *f;
//}

bool Func::isNetConnect()
{
    QProcess *process = new QProcess();
    QStringList list;
    list << "-i"
         << "www.baidu.com";
    process->start("curl", list);
    process->waitForFinished();
    int ret = process->exitCode();
    return ret ? false : true;
}

bool Func::isIpv6Connect()
{
    QProcess *process = new QProcess();
    QStringList list;
    list << "-i"
         << "http://ipv6.uenu.cn/";
    process->start("curl", list);
    process->waitForFinished();
    int ret = process->exitCode();
    return ret ? true : false;
}

bool Func::isDHT()
{
    QString dht6 = QString(QDir::homePath() + "/.config/uos/downloadmanager/dht6.dat");
    QString dht = QString(QDir::homePath() + "/.config/uos/downloadmanager/dht.dat");
    QFileInfo dht6File(dht6);
    QFileInfo dhtFile(dht);
    if (dht6File.exists() && dhtFile.exists()) {
        return true;
    }
    return false;

}

bool Func::isBt()
{
    QString aria2 = QString(QDir::homePath() + "/.config/uos/downloadmanager/aria2.conf");
    QFileInfo aria2File(aria2);
    if (aria2File.exists()) {
        return true;
    }
    return false;
}
