// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <QProcess>
#include "func.h"
#include "ddlog.h"

ClipboardTimer::ClipboardTimer(QObject *parent)
    : QObject(parent)
{
    qCDebug(downloaderLog) << "ClipboardTimer object created";

    auto e = QProcessEnvironment::systemEnvironment();
    m_sessionType = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    qCDebug(downloaderLog) << "Session type:" << m_sessionType;

    m_clipboard = QApplication::clipboard(); //获取当前剪切板
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardTimer::getDataChanged);
    qCDebug(downloaderLog) << "Clipboard monitoring initialized";
}

ClipboardTimer::~ClipboardTimer()
{
    qCDebug(downloaderLog) << "ClipboardTimer object destroyed";
}

void ClipboardTimer::checkClipboardHasUrl()
{
    qCDebug(downloaderLog) << "[ClipboardTimer] checkClipboardHasUrl function started";
    getDataChanged();
}

void ClipboardTimer::getDataChanged()
{
    qCDebug(downloaderLog) << "Clipboard content changed";
    static bool isInitWayland = true;

    const QMimeData *mimeData = m_clipboard->mimeData();
    qCDebug(downloaderLog) << "Clipboard formats:" << mimeData->formats();
    QByteArray isDeepinCilpboard = mimeData->data("FROM_DEEPIN_CLIPBOARD_MANAGER");
    if(m_sessionType == QLatin1String("wayland")){
        if(m_clipboard->text() == m_lastUrl || m_clipboard->text().isEmpty()){
            return;
        }
        if(isInitWayland){
            if(!Settings::getInstance()->getIsClipboradStart(m_clipboard->text())){
                return;
            }
        }
        m_lastUrl = m_clipboard->text();
    }else {
        if (mimeData->data("TIMESTAMP") == m_timeStamp || isDeepinCilpboard == "1") {
            return;
        }
        m_timeStamp = mimeData->data("TIMESTAMP");
    }
    if (m_clipboard->ownsClipboard()) {
        return;
    }

    for (int i = 0; i < mimeData->formats().size(); i++) {
        QString format = mimeData->formats()[i];
        QString formatData = mimeData->data(mimeData->formats()[i]);
        if (format == "FROM_DEEPIN_CLIPBOARD_MANAGER") {
            return;
        }
    }

    QStringList urlList = m_clipboard->text().split("\n");
    for (int i = 0; i < urlList.size(); i++) {
        urlList[i] = urlList[i].simplified();
    }
    QString url;
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
        qCDebug(downloaderLog) << "Valid download URL detected in clipboard";
        emit sendClipboardTextChange(url);
    } else {
        qCDebug(downloaderLog) << "No valid download URL found in clipboard";
    }
    qCDebug(downloaderLog) << "[ClipboardTimer] getDataChanged function ended";
}

bool ClipboardTimer::isMagnetFormat(QString url)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] isMagnetFormat function started for URL:" << url;
    url = url.toLower();
    bool isMagnet = url.mid(0, 20) == "magnet:?xt=urn:btih:";
    qCDebug(downloaderLog) << "Magnet link check:" << isMagnet;
    return isMagnet;
}

bool ClipboardTimer::isHttpFormat(QString url)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] isHttpFormat function started for URL:" << url;
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        qCDebug(downloaderLog) << "[ClipboardTimer] URL does not contain ftp, http, or https protocols, returning false";
        return false;
    }

    if(!isWebFormat(url)){
        qCDebug(downloaderLog) << "[ClipboardTimer] URL is not in valid web format, returning false";
        return false;
    }
    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];
    QStringList typeList = getTypeList();
    if (typeList.contains(suffix)) {
        qCDebug(downloaderLog) << "[ClipboardTimer] Suffix found in type list, returning true";
        return true;
    }
    for (int i = 0; i < typeList.size(); i++) {
        if (typeList[i].toUpper() == suffix.toUpper()) {
            qCDebug(downloaderLog) << "[ClipboardTimer] Suffix found in type list (case insensitive), returning true";
            return true;
        }
    }
    qCDebug(downloaderLog) << "[ClipboardTimer] Suffix not found in type list, returning false";
    return false;
}

bool ClipboardTimer::isBtFormat(QString url)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] isBtFormat function started for URL:" << url;
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        qCDebug(downloaderLog) << "[ClipboardTimer] URL does not contain ftp, http, or https protocols, returning false";
        return false;
    }
    if(!isWebFormat(url)){
        qCDebug(downloaderLog) << "[ClipboardTimer] URL is not in valid web format, returning false";
        return false;
    }
    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];
    QStringList type;
    type << "torrent";
    if (type.contains(suffix)) {
        qCDebug(downloaderLog) << "[ClipboardTimer] Suffix matches torrent format, returning true";
        return true;
    }
    for (int i = 0; i < type.size(); i++) {
        if (type[i].toUpper() == suffix.toUpper()) {
            qCDebug(downloaderLog) << "[ClipboardTimer] Suffix matches torrent format (case insensitive), returning true";
            return true;
        }
    }
    qCDebug(downloaderLog) << "[ClipboardTimer] Suffix does not match torrent format, returning false";
    return false;
}

bool ClipboardTimer::isMlFormat(QString url)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] isMlFormat function started for URL:" << url;
    if ((-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))) {
        qCDebug(downloaderLog) << "[ClipboardTimer] URL does not contain ftp, http, or https protocols, returning false";
        return false;
    }
    if(!isWebFormat(url)){
        qCDebug(downloaderLog) << "[ClipboardTimer] URL is not in valid web format, returning false";
        return false;
    }

    QStringList list = url.split(".");
    QString suffix = list[list.size() - 1];

    QStringList type;
    type << "metalink";
    if (type.contains(suffix)) {
        qCDebug(downloaderLog) << "[ClipboardTimer] Suffix matches metalink format, returning true";
        return true;
    }
    for (int i = 0; i < type.size(); i++) {
        if (type[i].toUpper() == suffix.toUpper()) {
            qCDebug(downloaderLog) << "[ClipboardTimer] Suffix matches metalink format (case insensitive), returning true";
            return true;
        }
    }
    qCDebug(downloaderLog) << "[ClipboardTimer] Suffix does not match metalink format, returning false";
    return false;
}

QStringList ClipboardTimer::getTypeList()
{
    qCDebug(downloaderLog) << "[ClipboardTimer] getTypeList function started";

    QString configPath = QString("%1/%2/%3/httpAdvanced.json")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());
    QFile file(configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qCDebug(downloaderLog) << "[ClipboardTimer] File open failed!, returning empty list";
        return QStringList();
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString defaultSuffix = obj.value("CurSuffix").toString();
    defaultSuffix.remove('.');
    QStringList defaulList = defaultSuffix.split(";");
    QStringList::iterator it = defaulList.begin();
//    for (;it != defaulList.end() ; it++) {
//        QString str = *it;
//        if(!it->endsWith(';')){
//            it = defaulList.erase(it);
//            it--;
//            continue;
//        }
//        it->remove(";");
//    }
   defaulList.removeAll("metalink");
   defaulList.removeAll("torrent");
   qCDebug(downloaderLog) << "[ClipboardTimer] Filtered type list:" << defaulList;
   return  defaulList;
}

QStringList ClipboardTimer::getWebList()
{
    qCDebug(downloaderLog) << "[ClipboardTimer] getWebList function started";
    QString configPath = QString("%1/%2/%3/httpAdvanced.json")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());
    QFile file(configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qCDebug(downloaderLog) << "File open failed!";
        return QStringList();
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString curWeb = obj.value("CurWeb").toString();


    return  midWebList(curWeb.split("\n"));
}

bool ClipboardTimer::isWebFormat(QString url)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] isWebFormat function started for URL:" << url;

    url = url.mid(url.indexOf("//")+2);
    url = url.mid(0,url.indexOf("/"));
    QStringList webList =getWebList();
    QString webUrl;
    for (int i = 0; i < webList.size(); i++) {
        if(webList[i].simplified().isEmpty()){
            // qDebug() << "[ClipboardTimer] Skipping empty web list entry at index:" << i;
            continue;
        }
        if(url == webList[i]){
            // qDebug() << "[ClipboardTimer] URL found in web list, blocking, returning false";
            return false;
        }
    }
    qCDebug(downloaderLog) << "[ClipboardTimer] URL not found in web list, allowing, returning true";
    return true;
}

QStringList ClipboardTimer::midWebList(QStringList webList)
{
    qCDebug(downloaderLog) << "[ClipboardTimer] midWebList function started with" << webList.size() << "entries";
    for (int i = 0; i < webList.size() ; ++i) {
        if (!((-1 == webList[i].indexOf("ftp:")) && (-1 == webList[i].indexOf("http://")) && (-1 == webList[i].indexOf("https://")))) {
            // qDebug() << "[ClipboardTimer] Entry contains protocol, extracting domain";
            webList[i] = webList[i].mid(webList[i].indexOf("//")+2);
        }
        if(webList[i].contains("/")){
            // qDebug() << "[ClipboardTimer] Entry contains path, extracting domain only";
            webList[i] = webList[i].mid(0,webList[i].indexOf("/"));
        }
    }

    qCDebug(downloaderLog) << "[ClipboardTimer] midWebList function ended with processed list:" << webList;
    return webList;
}








