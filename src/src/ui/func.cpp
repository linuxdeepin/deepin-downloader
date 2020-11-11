#include "func.h"
#include <QProcess>
#include <QDir>

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

bool Func::isHTTPConnect()
{
    //    if (!isBt()) {
    //        return false;
    //    }
    return isNetConnect();
}
