#include "func.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

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

bool Func::setMimeappsValue(QString key, QString value)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list";
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error";
        return false;
    }
    QTextStream data(&readFile);
    bool isDefault = false;
    bool isAdded = false;
    QStringList DefaultList;
    QStringList AddedList;
    //找到 [Default Applications] 和[Added Associations] 下面中的归类，放入list中
    while (!data.atEnd()) {
        QString sLine = data.readLine();
        if (sLine == "[Default Applications]") {
            isDefault = true;
            isAdded = false;
        } else if (sLine == "[Added Associations]") {
            isDefault = false;
            isAdded = true;
        }
        if (isDefault) {
            DefaultList.append(sLine);
        }
        if (isAdded) {
            AddedList.append(sLine);
        }
    }
    //设置key和value,如果key已经存在，将value设置。  如果key不存在，在后追加一行
    if (!DefaultList.isEmpty()) {
        for (int i = 0; i < DefaultList.size(); i++) {
            if (DefaultList[i].contains(key)) {
                DefaultList[i] = key+ "=" + value;
                break;
            }
            if (i == DefaultList.size() - 1 && !(DefaultList[i].contains(key))) {
                DefaultList.append(key+ "=" + value);
                break;
            }
        }
    }
    //设置key和value,如果key已经存在，将value设置。  如果key不存在，在后追加一行
    if (!AddedList.isEmpty()) {
        for (int i = 0; i < AddedList.size(); i++) {
            if (AddedList[i].contains(key)) {
                AddedList[i] = key+ "=" + value;
                break;
            }
            if (i == AddedList.size() - 1 && !(AddedList[i].contains("application/x-bittorrent"))) {
                AddedList.append(key+ "=" + value);
                break;
            }
        }
    }
    readFile.close();

    //将替换以后的字符串，重新写入到文件中去
    QFile writerFile(path);
    DefaultList << AddedList;
    if (writerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    }
    QTextStream writeData(&writerFile);

    for (int i = 0; i < DefaultList.size(); i++) {
        writeData << DefaultList[i] << endl;
    }
    writeData.flush();
    writerFile.close();
    return true;
}
