#include <QCoreApplication>
#include <QSharedMemory>
#include <QProcess>
#include <signal.h>
#include "extensionservice.h"


bool checkProcessExist();
void signalHandlerFun(int signum);

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);
    extensionService e;
    return a.exec();
}

bool checkProcessExist()
{
    QProcess process;
    QStringList list;
    process.start("pgrep dlmExtensionService");
    process.start();
    process.waitForStarted(1000);
    process.waitForFinished(1000);
    QString str = process.readAll();
    QStringList strList = str.split('\n');
    if (strList.size() > 1 && strList.at(1).isEmpty()) {
        return false;
    }
    return true;
}

void signalHandlerFun(int signum) {
    printf("catch signal %d\n", signum);
}
