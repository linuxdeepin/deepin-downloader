#include "func.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <dpinyin.h>
#include <sys/resource.h>

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

QString Func::pathToMD5(QString path)
{
    QFile theFile(path);
    theFile.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
    theFile.close();
    return  ba.toHex().constData();
}

QString Func::getIniConfigValue(QString path, QString group, QString key)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "error";
        return "";
    }
     QTextStream data(&f);
     group = QString("[%1]").arg(group);
     bool isGroup = false;
     while (!data.atEnd()) {
         QString sLine = data.readLine();
         if(sLine.isNull()){
             continue;
         }
         if(sLine == group){
            isGroup = true;
            continue;
         }
         if(isGroup){
             if(sLine.startsWith('[') && sLine.endsWith(']')){
                 isGroup = false;
                 continue;
             }
             if(sLine.split('=')[0] == key){
                 return sLine.split('=')[1];
             }
         }
     }
}

bool Func::setIniConfigValue(QString path, QString group, QString key, QString value)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadWrite)) {
        qDebug() << "error";
        return false;
    }
    QTextStream data(&f);
    QStringList dataList = data.readAll().split("\n");
    group = QString("[%1]").arg(group);
    bool isGroup = false;
    for (int i = 0; i < dataList.size(); i++) {
        if(dataList[i].isNull()){
            continue;
        }
        if(dataList[i] == group){
            isGroup = true;
            continue;
        }
        if(isGroup){
            if(dataList[i].startsWith('[') && dataList[i].endsWith(']')){
                isGroup = false;
                continue;
            }
            if(dataList[i].split('=')[0] == key){
                dataList[i] = dataList[i].split('=')[0] + "=" + value;

                //将替换以后的字符串，重新写入到文件中去
                QFile writerFile(path);
                if (writerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                }
                QTextStream writeData(&writerFile);
                for (int i = 0; i < dataList.size(); i++) {
                    writeData << dataList[i] <<endl;
                }
                writeData.flush();
                writerFile.close();
                return true;
            }
        }
    }
    return false;
}

double Func::formatSpeed(QString str)
{
    QString number = str;
    if (str.contains("GB/s")) {
        str.remove("GB/s");
    }if (str.contains("KB/s")) {
        str.remove("KB/s");
    } else if (str.contains("MB/s")) {
        str.remove("MB/s");
    } else if (str.contains("B/s")) {
        str.remove("B/s");
    }
    double num = str.toDouble();
    if (number.contains("KB")) {
        num = num * 1024;
    } else if (number.contains("MB")) {
        num = num * 1024 * 1024;
    } else if (number.contains("GB")) {
        num = num * 1024 * 1024 * 1024;
    }
    return num;
}

double Func::formatFileSize(QString str)
{
    double num = -1;
    QString number = str.left(str.length() - 2);
    num = number.toDouble();
    if (str.contains("KB")) {
        num = num * 1024;
    } else if (str.contains("MB")) {
        num = num * 1024 * 1024;
    } else if (str.contains("GB")) {
        num = num * 1024 * 1024 * 1024;
    }
    return num;
}

QString Func::chineseToPinyin(QString input)
{
    if ("" == input) {
        return "";
    }
    QString value = input;
    for(int i = input.size() - 1; i >= 0; i--) {
        QString ch = input.at(i);
        if(ch.contains(QRegExp("[\\x4e00-\\x9fa5]+"))){
            QString pinyin = removeDigital(DTK_NAMESPACE::Core::Chinese2Pinyin(ch));
            value.replace(ch, pinyin);
        }
    }
    return value;
}

QString Func::removeDigital(QString input)
{
    if ("" == input) {
        return "";
    }
    QString value = "";
    QByteArray ba = input.toLocal8Bit();
    char *data = nullptr;
    data = ba.data();
    while (*data) {
        if (!(*data >= '0' && *data <= '9')) {
            value += *data;
        }
        data++;
    }
    return value;
}

bool Func::setfdLimit(unsigned long maxLen)
{
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = maxLen + 3;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        return false;
    } else {
        return true;
    }
}




