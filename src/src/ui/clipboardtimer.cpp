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


ClipboardTimer::ClipboardTimer(QObject *parent)
    : QObject(parent)
{
    m_clipboard = QApplication::clipboard();        //获取当前剪切板
    connect(m_clipboard,&QClipboard::dataChanged,this,&ClipboardTimer::getDataChanged);

}

ClipboardTimer::~ClipboardTimer()
{

}


void ClipboardTimer::getDataChanged()
{
    QStringList urlList = m_clipboard->text().split("\n");
    for (int i = 0; i < urlList.size(); i++) {
        urlList[i] = urlList[i].simplified();
    }
    QString url;
    Settings *setting =  Settings::getInstance();
    bool bIsHttp =  setting->getHttpDownloadState();
    bool bIsMagnet = setting->getMagneticDownloadState();
    bool bIsBt = setting->getBtDownloadState();
    //将不符合规则链接剔除
    for (int i = 0; i < urlList.size(); i++)
    {
        if((isMagnet(urlList[i]) && bIsMagnet) ||
                (isHttp(urlList[i]) && bIsHttp))
        {
            url.append(urlList[i]).append("\n");
        }
    }
    //将符合规则链接发送至主页面
    if(!url.isEmpty())
    {
        emit sendClipboardTextChange(url);
    }

    //是否是BT文件托管，若是BT文件托管，打开BT文件
    if(m_clipboard->text().endsWith(".torrent") && bIsBt)
    {
        emit sentBtTextChange(m_clipboard->text());
        m_clipboard->clear();        
    }
    //是否调用下载器，不打开任何任务
    if(!urlList.isEmpty())
    {
        if(isStartManager(urlList[0]))
        {
            emit mainWindowsShow();
        }
    }

}

bool ClipboardTimer::isMagnet(QString url)
{
    QString str = url;
    if(str.mid(0,20) == "magnet:?xt=urn:btih:")
    {
        return  true;
    }
    else
    {
        return  false;
    }
}

bool ClipboardTimer::isHttp(QString url)
{
    if( (-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://")))
    {
        return false;
    }
    QStringList list= url.split(".");
    QString suffix = list[list.size()-1];
    QStringList type;
     type<< "asf"<<"avi"<<"iso"<<"mp3"<<"mpeg"<<"ra"<<"rar"<<"rm"<<"rmvb"<<"tar"<<"wma"<<"wmp"<<"wmv"<<"mov"<<"zip"<<"3gp"<<"chm"<<"mdf"<<"torrent"<<"jar"<<"msi"<<"arj"<<"bin"<<"dll"<<"psd"<<"hqx"<<"sit"<<"lzh"<<"gz"<<"tgz"<<"xlsx"<<"xls"<<"doc"<<"docx"<<"ppt"<<"pptx"<<"flv"<<"swf"<<"mkv"<<"tp"<<"ts"<<"flac"<<"ape"<<"wav"<<"aac"<<"txt"<<"dat"<<"7z"<<"ttf"<<"bat"<<"xv"<<"xvx"<<"pdf"<<"mp4"<<"apk"<<"ipa"<<"epub"<<"mobi"<<"deb"<<"sisx"<<"cab"<<"pxl"<<"xlb"<<"dmg"<<"msu"<<"bz2"<<"exe";
    if(type.contains(suffix))
    {
        return true;
    }
    for (int i = 0; i < type.size(); i++) {
        if(type[i].toUpper() == suffix)
        {
            return true;
        }
    }
    return false;
}


bool ClipboardTimer::isStartManager(QString str)
{
    if(str.contains("start_manager_for_clipboard"))
    {
        str.remove("start_manager_for_clipboard");
        return true;
    }
    return false;
}
