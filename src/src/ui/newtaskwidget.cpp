/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file newtaskwidget.cpp
 *
 * @brief 新建任务类
 *
 * @date 2020-06-09 10:52
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

#include "newtaskwidget.h"
#include "btinfodialog.h"
#include "messagebox.h"
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QStandardPaths>

newTaskWidget::newTaskWidget(DDialog *parent):
    DDialog(parent)
{
    initUi();

}

newTaskWidget::~newTaskWidget()
{

}

void newTaskWidget::initUi()
{
    setCloseButtonVisible(true);
    setAcceptDrops(true);

    QIcon tryIcon=QIcon(QIcon::fromTheme(":/icons/icon/downloader2.svg"))  ;
    this->setIcon(tryIcon);
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
    this->setTitle(tr("New Task"));

    QString _msg = tr("When adding download links, please enter one URL in each line");
    this->addSpacing(15);
    DLabel * _msgLab= new DLabel(this);
    _msgLab->setText(_msg);
    this->addContent(_msgLab,Qt::AlignHCenter);
    this->addSpacing(15);
    m_texturl= new DTextEdit(this);

    m_texturl->setReadOnly(false);
    m_texturl->setAcceptDrops(false);
    m_texturl->setPlaceholderText(tr("Enter download links or drag torrent file here"));
    m_texturl->setFixedSize(QSize(454,154));
    connect(m_texturl,&DTextEdit::textChanged,this,&newTaskWidget::onTextChanged);
    QPalette _pal;
    _pal.setColor(QPalette::Base, QColor(0,0,0,20));
    m_texturl->setPalette(_pal);
    this->addContent(m_texturl);
    this->addSpacing(15);


    QWidget *_boxBtn= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(_boxBtn);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,10,0);
    DIconButton *_iconBtn= new DIconButton(_boxBtn);
    QIcon _tryIcon=QIcon(QIcon::fromTheme("dcc_bt"));
    _iconBtn->setIcon(_tryIcon);
    _iconBtn->setIconSize(QSize(18,15));
    _iconBtn->setFixedSize(QSize(40,35));
    connect(_iconBtn,&DIconButton::clicked,this,&newTaskWidget::openfileDialog);
    _iconBtn->setToolTip("Select file");
    layout->addWidget(_iconBtn);

    QWidget *_rightBox= new QWidget(_boxBtn);
    QHBoxLayout *layout_right=new QHBoxLayout(_rightBox);
    layout->setSpacing(10);
    layout_right->setContentsMargins(0,0,0,0);

    DPushButton *cancel_button = new DPushButton(_boxBtn);
    QSizePolicy _policy;
    _policy = cancel_button->sizePolicy();
    _policy.setHorizontalPolicy(QSizePolicy::Expanding);
    cancel_button->setSizePolicy(_policy);
    cancel_button->setText(tr("Cancel"));
    connect(cancel_button,&DPushButton::clicked,this,&newTaskWidget::onCancelBtnClicked);
    layout_right->addWidget(cancel_button);



    m_sure_button = new DSuggestButton(_boxBtn);
    m_sure_button->setText(tr("Confirm"));
    _policy = m_sure_button->sizePolicy();
    _policy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_sure_button->setSizePolicy(_policy);
    connect(m_sure_button,&DPushButton::clicked,this,&newTaskWidget::onSureBtnClicked);
    layout_right->addSpacing(20);
    layout_right->addWidget(m_sure_button);
    layout->addWidget(_rightBox);
    this->addContent(_boxBtn);
}


void newTaskWidget::openfileDialog()
{
    QString _btFile = DFileDialog::getOpenFileName(this, tr("Choose Torrent File"), QDir::homePath(), "*.torrent");
    if(_btFile != "") {
        QString _savePath =  Settings::getInstance()->getDownloadSavePath();
        BtInfoDialog *_dialog = new BtInfoDialog(_btFile,_savePath);//torrent文件路径
        if(_dialog->exec() == QDialog::Accepted) {
            QMap<QString,QVariant> opt;
            QString _infoName;
            QString _infoHash;
            _dialog->getBtInfo(opt, _infoName, _infoHash);
            emit newDownLoadTorrent(_btFile,opt,_infoName,_infoHash);
            this->close();
        }
        delete _dialog;
    }
}

void newTaskWidget::onCancelBtnClicked()
{
    this->close();
}

void newTaskWidget::onSureBtnClicked()
{
    QString _strUrl = m_texturl->toPlainText();
    if(_strUrl.isEmpty())
    {
        qDebug()<<"url is NUll";
        return;
    }
    //获取当前错误地址
    QStringList _urlList = _strUrl.split("\n");
    _urlList = _urlList.toSet().toList();
    QStringList _errorList;
    for (int i = 0; i < _urlList.size(); i++) {
        if(!(isHttp(_urlList[i]) || isMagnet(_urlList[i])))
        {
            _errorList.append(_urlList[i]);
        }
    }
    if(!_errorList.isEmpty())
    {
        QString warning_msg = tr("The address you entered cannot be resolved correctly. Please try again");
        MessageBox *msg = new MessageBox();
        msg->setWarings(warning_msg, tr("sure"), "");
        msg->exec();
        return;
    }
//    //将错误地址弹出提示框   使用列表方式提示
//    if(!_errorList.isEmpty())
//    {
//        QString warning_msg = tr("has ") + QString::number(_errorList.size()) + tr(" the error download");
//        MessageBox *msg = new MessageBox();
//        msg->setWarings(warning_msg, tr("sure"), "", _errorList.size(), _errorList);
//        msg->exec();
//    }
    //删除错误地址
    for (int i = 0;i < _errorList.size() ; i++)
    {
        _urlList.removeOne(_errorList[i]);
    }
    //发送至主窗口
    QString _savePath =  Settings::getInstance()->getDownloadSavePath();
    emit NewDownload_sig(_urlList,_savePath);
    this->close();
}

void newTaskWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) //数据中是否包含URL
    {
        event->acceptProposedAction(); //如果是则接受动作
    }
    else
        event->ignore();
}


void newTaskWidget::dropEvent(QDropEvent *event)
{

    const QMimeData *mineData=event->mimeData();//获取MIME数据
    if(mineData->hasUrls())                     //如数据中包含URL
    {
        QList<QUrl>urlList=mineData->urls();    //获取URL列表
        if(urlList.isEmpty())return ;
        QString fileName;
        for(int i=0;i<urlList.size();++i)
        {
            fileName=urlList.at(i).toString();


        if(fileName.isEmpty())return;

        if(!fileName.isEmpty())                 //若文件路径不为空
        {
            if(fileName.startsWith("file:")&&fileName.endsWith(".torrent"))
            {
                fileName=fileName.right(fileName.length()-6);
                QString _savePath =  Settings::getInstance()->getDownloadSavePath();
                BtInfoDialog *_dialog = new BtInfoDialog(fileName,_savePath);//torrent文件路径
                int ret = _dialog->exec();
                if(ret == QDialog::Accepted) {
                    QMap<QString,QVariant> opt;
                    QString _infoName;
                    QString _infoHash;
                    _dialog->getBtInfo(opt, _infoName, _infoHash);
                    emit newDownLoadTorrent(fileName,opt,_infoName,_infoHash);
                    this->close();
                }
                delete _dialog;
             }
            else
            {
                 //textBrowser->append(fileName);
            }
        }
       }
    }
}

void newTaskWidget::setUrl(QString url)
{
    m_texturl->clear();
    m_texturl->setText(url);
}

bool newTaskWidget::isMagnet(QString url)
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

bool newTaskWidget::isHttp(QString url)
{

    if( (-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://")))
    {
        return false;
    }
    QStringList _list= url.split(".");
    QString _suffix = _list[_list.size()-1];
    QStringList _type;
     _type<< "asf"<<"avi"<<"iso"<<"mp3"<<"mpeg"<<"ra"<<"rar"<<"rm"<<"rmvb"<<"tar"<<"wma"<<"wmp"<<"wmv"<<"mov"<<"zip"<<"3gp"<<"chm"<<"mdf"<<"torrent"<<"jar"<<"msi"<<"arj"<<"bin"<<"dll"<<"psd"<<"hqx"<<"sit"<<"lzh"<<"gz"<<"tgz"<<"xlsx"<<"xls"<<"doc"<<"docx"<<"ppt"<<"pptx"<<"flv"<<"swf"<<"mkv"<<"tp"<<"ts"<<"flac"<<"ape"<<"wav"<<"aac"<<"txt"<<"dat"<<"7z"<<"ttf"<<"bat"<<"xv"<<"xvx"<<"pdf"<<"mp4"<<"apk"<<"ipa"<<"epub"<<"mobi"<<"deb"<<"sisx"<<"cab"<<"pxl"<<"xlb"<<"dmg"<<"msu"<<"bz2"<<"exe";
    if(_type.contains(_suffix))
    {
        return true;
    }
    for (int i = 0; i < _type.size(); i++) {
        if(_type[i].toUpper() == _suffix)
        {
            return true;
        }
    }
    return false;
}

void newTaskWidget::onTextChanged()
{
    if(m_texturl->toPlainText().isEmpty())
    {
        m_sure_button->setEnabled(false);
    }
    else
    {
        m_sure_button->setEnabled(true);
    }
}
