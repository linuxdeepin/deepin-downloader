/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file clipboardtimer.h
 *
 * @brief 剪切板托管类
 *
 * @date 2020-06-09 10:51
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

#include "clipboardtimer.h"
#include <QClipboard>
#include <QtDebug>
#include <QFile>
#include <QMimeData>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include "func.h"

ClipboardTimer::ClipboardTimer(QObject *parent)
    : QObject(parent)
{
    m_clipboard = QApplication::clipboard(); //获取当前剪切板
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardTimer::getDataChanged);
}

ClipboardTimer::~ClipboardTimer()
{
}

void ClipboardTimer::checkClipboardHasUrl()
{
    getDataChanged();
}

void ClipboardTimer::getDataChanged()
{
    //qDebug() << "ClipboardTimer::getDataChanged()";
    const QMimeData *mimeData = m_clipboard->mimeData();
    QByteArray isDeepinCilpboard = mimeData->data("FROM_DEEPIN_CLIPBOARD_MANAGER");
    if (mimeData->data("TIMESTAMP") == m_timeStamp) {
        return;
    }
    if (isDeepinCilpboard == "1") {
        return;
    }
    if (m_clipboard->ownsClipboard()) {
        return;
    }
    m_timeStamp = mimeData->data("TIMESTAMP");
    const QMimeData *data = m_clipboard->mimeData();
    if (data->hasText()) {
        QString text = data->text();
    }
    for (int i = 0; i < data->formats().size(); i++) {
        QString format = data->formats()[i];
        QString formatData = data->data(data->formats()[i]);
        if (format == "FROM_DEEPIN_CLIPBOARD_MANAGER") {
            //qDebug() << "ClipboardTimer <getDataChanged> : FROM_DEEPIN_CLIPBOARD_MANAGER";
            return;
        }
        //qDebug() << "ClipboardTimer <getDataChanged> format:: " << format;
        //qDebug() << "ClipboardTimer <getDataChanged> formatData:: " << formatData;
    }

    QStringList urlList = m_clipboard->text().split("\n");
    for (int i = 0; i < urlList.size(); i++) {
        urlList[i] = urlList[i].simplified();
    }
    QString url;
    ////qDebug()<< "class::ClipboardTimer getDataChanged() url <<  "<< urlList;
    Settings *setting = Settings::getInstance();
    bool bIsHttp = setting->getHttpDownloadState();
    bool bIsMagnet = setting->getMagneticDownloadState();
    bool bIsBt = setting->getBtDownloadState();
    bool bIsMl = setting->getMLDownloadState();
    //将不符合规则链接剔除
    for (int i = 0; i < urlList.size(); i++) {
        if ((isMagnetFormat(urlList[i]) && bIsMagnet) || (isHttpFormat(urlList[i]) && bIsHttp)
                || (isBtFormat(urlList[i]) && bIsBt) || (isMlFormat(urlList[i]) && bIsMl)) {
            url.append(urlList[i]).append("\n");
        }
    }
    //将符合规则链接发送至主页面
    if (!url.isEmpty()) {
        emit sendClipboardTextChange(url);
    }
}

bool ClipboardTimer::isMagnetFormat(QString url)
{
    url = url.toLower();
    return url.mid(0, 20) == "magnet:?xt=urn:btih:";
}

bool ClipboardTimer::isHttpFormat(QString url)
{
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        return false;
    }

    if(!isWebFormat(url)){
        return false;
    }
    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];
    QStringList typeList = getTypeList();
    if (typeList.contains(suffix)) {
        return true;
    }
    for (int i = 0; i < typeList.size(); i++) {
        if (typeList[i].toUpper() == suffix.toUpper()) {
            return true;
        }
    }

    return false;
}

bool ClipboardTimer::isBtFormat(QString url)
{
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        return false;
    }
    if(!isWebFormat(url)){
        return false;
    }
    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];
    QStringList type;
    type << "torrent";
    if (type.contains(suffix)) {
        return true;
    }
    for (int i = 0; i < type.size(); i++) {
        if (type[i].toUpper() == suffix.toUpper()) {
            return true;
        }
    }
    return false;
}

bool ClipboardTimer::isMlFormat(QString url)
{
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        return false;
    }
    if(!isWebFormat(url)){
        return false;
    }
    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];
    QStringList type;
    type << "metalink";
    if (type.contains(suffix)) {
        return true;
    }
    for (int i = 0; i < type.size(); i++) {
        if (type[i].toUpper() == suffix.toUpper()) {
            return true;
        }
    }
    return false;
}

QStringList ClipboardTimer::getTypeList()
{

    QString configPath = QString("%1/%2/%3/httpAdvanced.json")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());
    QFile file(configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString defaultSuffix = obj.value("CurSuffix").toString();
    //defaultSuffix.remove('.');
    QStringList defaulList = defaultSuffix.split(".");
    QStringList::iterator it = defaulList.begin();
    for (;it != defaulList.end() ; it++) {
        QString str = *it;
        if(!it->endsWith(';')){
            it = defaulList.erase(it);
            it--;
            continue;
        } 
        it->remove(";");
    }
   defaulList.removeAll("metalink");
   defaulList.removeAll("torrent");
   return  defaulList;
}

QStringList ClipboardTimer::getWebList()
{
    QString configPath = QString("%1/%2/%3/httpAdvanced.json")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());
    QFile file(configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString curWeb = obj.value("CurWeb").toString();


    return  curWeb.split("\n");
}

bool ClipboardTimer::isWebFormat(QString url)
{
    QStringList webList =getWebList();
    for (int i = 0; i < webList.size(); i++) {
        if(webList[i].simplified().isEmpty()){
            continue;
        }
        if(url.contains(webList[i])){
            return false;
        }
    }
    return true;
}








