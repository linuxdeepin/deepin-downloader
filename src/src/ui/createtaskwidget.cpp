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

#include "createtaskwidget.h"
#include "btinfodialog.h"
#include "messagebox.h"
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>

CreateTaskWidget::CreateTaskWidget(DDialog *parent):
    DDialog(parent)
{
    initUi();

}

CreateTaskWidget::~CreateTaskWidget()
{

}

void CreateTaskWidget::initUi()
{
    setCloseButtonVisible(true);
    setAcceptDrops(true);


    QIcon tryIcon=QIcon(QIcon::fromTheme(":/icons/icon/downloader2.svg"))  ;
    this->setIcon(tryIcon);
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
    this->setTitle(tr("New Task"));

    QString msg = tr("When adding download links, please enter one URL in each line");
    this->addSpacing(15);
    DLabel * msgLab= new DLabel(this);
    msgLab->setText(msg);
    this->addContent(msgLab,Qt::AlignHCenter);
    this->addSpacing(15);
    m_texturl= new DTextEdit(this);

    m_texturl->setReadOnly(false);
    m_texturl->setAcceptDrops(false);
    m_texturl->setPlaceholderText(tr("Enter download links or drag torrent file here"));
    m_texturl->setFixedSize(QSize(454,154));
    connect(m_texturl,&DTextEdit::textChanged,this,&CreateTaskWidget::onTextChanged);
    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0,0,0,20));
    m_texturl->setPalette(pal);
    this->addContent(m_texturl);
    this->addSpacing(15);

    this->m_editDir = new DFileChooserEdit(this);
    this->m_editDir->lineEdit()->setReadOnly(true);
    this->m_editDir->lineEdit()->setClearButtonEnabled(false);
    this->m_editDir->setFileMode(QFileDialog::FileMode::DirectoryOnly);
    connect(this->m_editDir, &DFileChooserEdit::fileChoosed, this, &CreateTaskWidget::onFilechoosed);
    QString savePath =  Settings::getInstance()->getDownloadSavePath();
    this->m_editDir->setText(savePath);
    this->addContent(m_editDir);
    this->addSpacing(15);
    m_defaultDownloadDir = savePath;


    QWidget *boxBtn= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(boxBtn);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,10,0);
    DIconButton *iconBtn= new DIconButton(boxBtn);
    QIcon tryIcon1=QIcon(QIcon::fromTheme("dcc_bt"));
    iconBtn->setIcon(tryIcon1);
    iconBtn->setIconSize(QSize(18,15));
    iconBtn->setFixedSize(QSize(40,35));
    connect(iconBtn,&DIconButton::clicked,this,&CreateTaskWidget::onFileDialogOpen);
    iconBtn->setToolTip(tr("Select file"));
    layout->addWidget(iconBtn);

    QWidget *rightBox= new QWidget(boxBtn);
    QHBoxLayout *layout_right=new QHBoxLayout(rightBox);
    layout->setSpacing(10);
    layout_right->setContentsMargins(0,0,0,0);

    DPushButton *cancelButton = new DPushButton(boxBtn);
    QSizePolicy policy;
    policy = cancelButton->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    cancelButton->setSizePolicy(policy);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton,&DPushButton::clicked,this,&CreateTaskWidget::onCancelBtnClicked);
    layout_right->addWidget(cancelButton);



    m_sureButton = new DSuggestButton(boxBtn);
    m_sureButton->setText(tr("Confirm"));
    policy = m_sureButton->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_sureButton->setSizePolicy(policy);
    connect(m_sureButton,&DPushButton::clicked,this,&CreateTaskWidget::onSureBtnClicked);
    layout_right->addSpacing(20);
    layout_right->addWidget(m_sureButton);
    layout->addWidget(rightBox);
    this->addContent(boxBtn);

    setMaximumSize(this->width(),this->height());
}


void CreateTaskWidget::onFileDialogOpen()
{
    QString btFile = DFileDialog::getOpenFileName(this, tr("Choose Torrent File"), QDir::homePath(), "*.torrent");
    if(btFile != "") {
        //QString _savePath =  Settings::getInstance()->getDownloadSavePath();
        BtInfoDialog *dialog = new BtInfoDialog(btFile,m_defaultDownloadDir);//torrent文件路径
        if(dialog->exec() == QDialog::Accepted) {
            QMap<QString,QVariant> opt;
            QString infoName;
            QString infoHash;
            dialog->getBtInfo(opt, infoName, infoHash);
            emit downLoadTorrentCreate(btFile,opt,infoName,infoHash);
            this->close();
        }
        delete dialog;
    }
}

void CreateTaskWidget::onCancelBtnClicked()
{
    this->close();
}

void CreateTaskWidget::onSureBtnClicked()
{
    QString strUrl = m_texturl->toPlainText();
    if(strUrl.isEmpty())
    {
        qDebug()<<"url is NUll";
        return;
    }
    //获取当前错误地址
    QStringList urlList = strUrl.split("\n");
    urlList = urlList.toSet().toList();
    QStringList errorList;
    for (int i = 0; i < urlList.size(); i++) {
        if(!(isHttp(urlList[i]) || isMagnet(urlList[i])))
        {
            errorList.append(urlList[i]);
        }
    }
    if(errorList == urlList)
    {
        QString warningMsg = tr("The address you entered cannot be resolved correctly. Please try again");
        MessageBox *msg = new MessageBox();
        msg->setWarings(warningMsg, tr("sure"), "");
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
    for (int i = 0;i < errorList.size() ; i++)
    {
        urlList.removeOne(errorList[i]);
    }
    //获取真实url地址,发送到主界面
//    for (int i = 0;i < _urlList.size(); i++)
//    {
//        getTruetUrl(_urlList[i]);
//    }
    //发送至主窗口
    //QString _savePath =  Settings::getInstance()->getDownloadSavePath();
    Settings::getInstance()->setCustomFilePath(m_defaultDownloadDir);
    emit downloadWidgetCreate(urlList,m_defaultDownloadDir, "");
    m_texturl->clear();
    this->hide();
}

void CreateTaskWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) //数据中是否包含URL
    {
        event->acceptProposedAction(); //如果是则接受动作
    }
    else
        event->ignore();
}


void CreateTaskWidget::dropEvent(QDropEvent *event)
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
              //  QString _savePath =  Settings::getInstance()->getDownloadSavePath();
                BtInfoDialog *dialog = new BtInfoDialog(fileName,m_defaultDownloadDir);//torrent文件路径
                int ret = dialog->exec();
                if(ret == QDialog::Accepted) {
                    QMap<QString,QVariant> opt;
                    QString infoName;
                    QString infoHash;
                    dialog->getBtInfo(opt, infoName, infoHash);
                    emit downLoadTorrentCreate(fileName,opt,infoName,infoHash);
                    this->close();
                }
                delete dialog;
             }
            else
            {
                 //textBrowser->append(fileName);
            }
        }
       }
    }
}

void CreateTaskWidget::setUrl(QString url)
{
    QString setTextUrl;
    QString textUrl = m_texturl->toPlainText();
    if(textUrl.isEmpty())
    {
        setTextUrl = url;
    }
    else {
        setTextUrl = textUrl + "\n" + url;
    }

    m_texturl->setText(setTextUrl);

    QString savePath =  Settings::getInstance()->getDownloadSavePath();
    this->m_editDir->setText(savePath);
}

bool CreateTaskWidget::isMagnet(QString url)
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

bool CreateTaskWidget::isHttp(QString url)
{

    if( (-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://")))
    {
        return false;
    }
//    QStringList _list= url.split(".");
//    QString _suffix = _list[_list.size()-1];
//    QStringList _type;
//     _type<< "asf"<<"avi"<<"iso"<<"mp3"<<"mpeg"<<"ra"<<"rar"<<"rm"<<"rmvb"<<"tar"<<"wma"<<"wmp"<<"wmv"<<"mov"<<"zip"<<"3gp"<<"chm"<<"mdf"<<"torrent"<<"jar"<<"msi"<<"arj"<<"bin"<<"dll"<<"psd"<<"hqx"<<"sit"<<"lzh"<<"gz"<<"tgz"<<"xlsx"<<"xls"<<"doc"<<"docx"<<"ppt"<<"pptx"<<"flv"<<"swf"<<"mkv"<<"tp"<<"ts"<<"flac"<<"ape"<<"wav"<<"aac"<<"txt"<<"dat"<<"7z"<<"ttf"<<"bat"<<"xv"<<"xvx"<<"pdf"<<"mp4"<<"apk"<<"ipa"<<"epub"<<"mobi"<<"deb"<<"sisx"<<"cab"<<"pxl"<<"xlb"<<"dmg"<<"msu"<<"bz2"<<"exe";
//    if(_type.contains(_suffix))
//    {
//        return true;
//    }
//    for (int i = 0; i < _type.size(); i++) {
//        if(_type[i].toUpper() == _suffix)
//        {
//            return true;
//        }
//    }
    return true;
}

void CreateTaskWidget::onTextChanged()
{
    if(m_texturl->toPlainText().isEmpty())
    {
        m_sureButton->setEnabled(false);
    }
    else
    {
        m_sureButton->setEnabled(true);
    }
}

void CreateTaskWidget::onFilechoosed(const QString &filename)
{
    QFileInfo fileinfo;
    QString strPath;
    fileinfo.setFile(filename);
    if(!fileinfo.isWritable())
    {
       MessageBox *msg=new MessageBox();
       QString title = tr("Permission denied. Please try other folder.");
       msg->setWarings(title, tr("sure"));
       msg->exec();
       strPath = m_editDir->directoryUrl().toString();
      // QString _text = this->getFileEditText(m_defaultDownloadDir);
       this->m_editDir->lineEdit()->setText(m_defaultDownloadDir);
       m_editDir->setDirectoryUrl(m_defaultDownloadDir);
       return;
    }

    this->m_editDir->lineEdit()->setText(filename);
    m_editDir->setDirectoryUrl(filename);
    m_defaultDownloadDir = filename;
}

void CreateTaskWidget::getTruetUrl(QString redirecUrl)
{
    if(isMagnet(redirecUrl))
    {
        emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir, "");
        return;
    }
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest *requset = new QNetworkRequest;                       // 定义请求对象    QString _trueUrl;
    requset->setUrl(QUrl(redirecUrl)); // 设置服务器的uri
    requset->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //manager->get(*requset);
    manager->head(*requset);
    // post信息到服务器
    QObject::connect(manager,
                     &QNetworkAccessManager::finished,
                     this,
                     [ = ](QNetworkReply *reply) {
                        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        //QString length = reply->attribute(QNetworkRequest::OriginalContentLengthAttribute).toString();
                        qDebug() << "FINISHED" << statusCode;
                        switch(statusCode) {
                            case 200: // redirect (Location: [URL])   真实链接
                            {
                                QProcess *p = new QProcess;
                                QStringList list;
                                list<<"-i"<< redirecUrl;
                                p->start("curl", list);
                                p->waitForReadyRead(1000);
                                QString str = p->readAllStandardOutput();
                                if(!str.contains("Content-Disposition: attachment;filename="))  // 为200的真实链接
                                {
                                     emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,"");
                                    return ;
                                }
                                QStringList urlInfoList = str.split("\r\n");
                                for (int i = 0; i < urlInfoList.size(); i++)
                                {
                                    if(urlInfoList[i].startsWith("Content-Disposition:"))  //为405链接
                                    {
                                        int start= urlInfoList[i].lastIndexOf("'");
                                        QString urlName = urlInfoList[i].mid(start);
                                        QString urlNameForZH = QUrl::fromPercentEncoding(urlName.toUtf8());
                                        emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,urlNameForZH);
                                        return ;
                                    }
                                }
                                break;
                            }
                            case 302: // redirect (Location: [URL])  重定向链接
                            {
                                QString strUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
                                emit downloadWidgetCreate(QStringList(strUrl),m_defaultDownloadDir, "");
                               break;
                            }
                            case 405:   //405链接
                            {
                                    QProcess *p = new QProcess;
                                    QStringList list;
                                    list<<"-i"<< redirecUrl;
                                    p->start("curl", list);
                                    p->waitForReadyRead(1000);
                                    QString str = p->readAllStandardOutput();
                                    QStringList urlInfoList = str.split("\r\n");
                                    for (int i = 0; i < urlInfoList.size(); i++)
                                    {
                                        if(urlInfoList[i].startsWith("Content-Disposition:"))  //为405链接
                                        {
                                            int start= urlInfoList[i].lastIndexOf("'");
                                            QString urlName = urlInfoList[i].mid(start);
                                            QString urlNameForZH = QUrl::fromPercentEncoding(urlName.toUtf8());
                                            emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,urlNameForZH);
                                            return ;
                                        }
                                    }
                                    break;
                            }
                            default:
                            {
                                QString warningMsg = QString(tr("%1\nThe address you entered cannot be resolved correctly")).arg(redirecUrl);
                                MessageBox *msg = new MessageBox();
                                msg->setWarings(warningMsg, tr("sure"), "");
                                msg->exec();
                                return;
                            }

                        }
    });
}

void CreateTaskWidget::closeEvent(QCloseEvent *event)
{
    this->m_texturl->clear();
}
