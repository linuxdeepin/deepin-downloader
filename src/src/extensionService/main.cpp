#include <QCoreApplication>
#include <QSharedMemory>
#include <QProcess>
#include "extensionservice.h"


bool checkProcessExist();

int main(int argc, char** argv) {

    QSharedMemory sharedMemory;
    sharedMemory.setKey("dlmExtensionService");
    if (sharedMemory.attach()) //设置成单例程序
    {
        if (!checkProcessExist()) { //下载器任务不存在，退出
            sharedMemory.detach();
        } else{
            return 0;
        }
    }
    if (sharedMemory.create(99)) {
        char *to = static_cast<char *>(sharedMemory.data());
        memset(to, 0, 99);
    }

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
