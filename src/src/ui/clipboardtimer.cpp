#include "clipboardtimer.h"


ClipboardTimer::ClipboardTimer(QObject *parent) :
    QObject(parent)
{
    m_clipboard = QApplication::clipboard();        //获取当前剪切板
    connect(m_clipboard,&QClipboard::dataChanged,this,&ClipboardTimer::getDataChanged);
}

ClipboardTimer::~ClipboardTimer()
{

}


void ClipboardTimer::getDataChanged()
{
    QString _url = m_clipboard->text();
    if(isMagnet(_url) || isHttp(_url))
    {
        emit sendClipboardText(_url);
    }

}

bool ClipboardTimer::isMagnet(QString url)
{
    QString _str = url;
    if(_str.mid(0,20) == "magnet:?xt=urn:btih:")
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
    QString _isHttp =url.mid(0,4);

    if(_isHttp != "http")
    {
        return false;
    }
    QStringList _list= url.split(".");
    QString _suffix = _list[_list.size()-1];
    QStringList _type;
     _type<< "asf"<<"avi"<<"iso"<<"mp3"<<"mpeg"<<"ra"<<"rar"<<"rm"<<"rmvb"<<"tar"<<"wma"<<"wmp"<<"wmv"<<"mov"<<"zip"<<"3gp"<<"chm"<<"mdf"<<"torrent"<<"jar"<<"msi"<<"arj"<<"bin"<<"dll"<<"psd"<<"hqx"<<"sit"<<"lzh"<<"gz"<<"tgz"<<"xlsx"<<"xls"<<"doc"<<"docx"<<"ppt"<<"pptx"<<"flv"<<"swf"<<"mkv"<<"tp"<<"ts"<<"flac"<<"ape"<<"wav"<<"aac"<<"txt"<<"dat"<<"7z"<<"ttf"<<"bat"<<"xv"<<"xvx"<<"pdf"<<"mp4"<<"apk"<<"ipa"<<"epub"<<"mobi"<<"deb"<<"sisx"<<"cab"<<"pxl"<<"xlb"<<"dmg"<<"msu"<<"bz2";
    if(_type.contains(_suffix))
    {
        return true;
    }
    else
    {
        return false;
    }
}
