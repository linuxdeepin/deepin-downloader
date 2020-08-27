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
#include "btinfotableview.h"
#include "taskdelegate.h"
#include "btheaderview.h"
#include "analysisurl.h"
#include "../database/dbinstance.h"
#include "aria2rpcinterface.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>
#include <QStandardItemModel>
#include <QMimeDatabase>
#include <QDesktopWidget>

CreateTaskWidget::CreateTaskWidget(DDialog *parent):
    DDialog(parent),
    m_analysisUrl(new AnalysisUrl)
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
    setIcon(tryIcon);
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);
    //setTitle(tr("New Task"));

    DLabel * msgTitle= new DLabel(this);
    QString titleMsg = tr("New Task");
    msgTitle->setText(titleMsg);
    addContent(msgTitle,Qt::AlignHCenter);
    QString msg = tr("When adding download links, please enter one URL in each line");
    addSpacing(10);
    DLabel * msgLab= new DLabel(this);
    msgLab->setText(msg);
    addContent(msgLab,Qt::AlignHCenter);
    addSpacing(10);
    m_texturl= new DTextEdit(this);

    m_texturl->setReadOnly(false);
    m_texturl->setAcceptDrops(false);
    m_texturl->setPlaceholderText(tr("Enter download links or drag torrent file here"));
    m_texturl->setFixedSize(QSize(500,154));
    connect(m_texturl,&DTextEdit::textChanged,this,&CreateTaskWidget::onTextChanged);
    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0,0,0,20));
    m_texturl->setPalette(pal);
    addContent(m_texturl);
    addSpacing(10);

    m_tableView = new BtInfoTableView(this);
    m_tableView->setMouseTracking(true);
    m_tableView->setShowGrid(false);
    m_tableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setFrameShape(QAbstractItemView::NoFrame);

    QFont font;
    font.setPixelSize(13);
    m_tableView->setFont(font);

    headerView *_headerView = new headerView(Qt::Horizontal, m_tableView);
    m_tableView->setHorizontalHeader(_headerView);
    _headerView->setDefaultAlignment(Qt::AlignLeft);
    _headerView->setHighlightSections(false);


    m_tableView->verticalHeader()->hide();
    m_tableView->verticalHeader()->setDefaultSectionSize(46);


    m_delegate = new TaskDelegate(this);
    m_tableView->setItemDelegate(m_delegate);

    m_model = new QStandardItemModel();
    m_tableView->setModel(m_model);

    m_model->setColumnCount(5);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, "");
    m_model->setHeaderData(2, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Size"));
    m_model->setHeaderData(4, Qt::Horizontal, "long");
    m_model->setHeaderData(5, Qt::Horizontal, "url");

    m_tableView->setColumnHidden(1, true);
    m_tableView->setColumnHidden(4, true);
    m_tableView->setColumnHidden(5, true);
    m_tableView->setColumnHidden(7, true);

    m_tableView->setColumnWidth(0, 290);
  //  tableView->setColumnWidth(1, 260);
    m_tableView->setColumnWidth(2, 60);
  //  tableView->setColumnWidth(3, 60);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    DFontSizeManager::instance()->bind(m_tableView,DFontSizeManager::SizeType::T6, 0);
    connect(m_tableView, &BtInfoTableView::hoverChanged, m_delegate, &TaskDelegate::onhoverChanged);
    //connect(m_tableView, &BtInfoTableView::doubleIndex, this, CreateTaskWidget::changeUrlName);
    addContent(m_tableView);

    QWidget *labelWidget = new QWidget(this);
    QHBoxLayout *hllyt = new QHBoxLayout(labelWidget);
    //总大小标签
    QFont font2;
    font2.setPixelSize(12);
    QPalette pal2;
    pal2.setColor(QPalette::WindowText, QColor("#8AA1B4"));
    m_labelFileSize = new DLabel(this);
    m_labelFileSize->setAlignment(Qt::AlignRight);
    m_labelFileSize->setText(QString(tr("Total ")+ "0"));
    m_labelFileSize->setFont(font2);
    m_labelFileSize->setPalette(pal2);

    //选中文件数
    m_labelSelectedFileNum = new DLabel(this);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(0)).arg("0"));
    m_labelSelectedFileNum->setFont(font2);
    m_labelSelectedFileNum->setPalette(pal2);
    hllyt->addWidget(m_labelSelectedFileNum);
    hllyt->addStretch();
    hllyt->addWidget(m_labelFileSize);
    labelWidget->setLayout(hllyt);
    addContent(labelWidget);

    //Checkbox
    m_checkWidget = new QWidget(this);
    QHBoxLayout *hlyt = new QHBoxLayout(m_checkWidget);
    m_checkAll = new DCheckBox(this);
  //  m_checkAll->setGeometry(15, 401, 95, 29);
    m_checkAll->setText(tr("All"));
    //m_checkAll->setText(tr("全选"));
    connect(m_checkAll, SIGNAL(clicked()), this, SLOT(onAllCheck()));

    m_checkVideo = new DCheckBox(this);
   // m_checkVideo->setGeometry(100, 401, 95, 29);
    m_checkVideo->setText(tr("Videos"));
    connect(m_checkVideo, SIGNAL(clicked()), this, SLOT(onVideoCheck()));

    m_checkPicture = new DCheckBox(this);
   // m_checkPicture->setGeometry(185, 401, 95, 29);
    m_checkPicture->setText(tr("Pictures"));
   // m_checkPicture->setText(tr("图片"));
    connect(m_checkPicture, SIGNAL(clicked()), this, SLOT(onPictureCheck()));


    m_checkAudio = new DCheckBox(this);
  //  m_checkAudio->setGeometry(270, 401, 95, 29);
    m_checkAudio->setText(tr("Music"));
   // m_checkAudio->setText(tr("音乐"));
    connect(m_checkAudio, SIGNAL(clicked()), this, SLOT(onAudioCheck()));

    m_checkOther = new DCheckBox(this);
   // m_checkOther->setGeometry(355, 401, 95, 29);    //Aria2cInterface::bytesFormat(this->info.totalLengthByets)try(375, 401, 95, 29);
    m_checkOther->setText(tr("Others"));
 //   m_checkOther->setText(tr("其他"));
    connect(m_checkOther, SIGNAL(clicked()), this, SLOT(onOtherCheck()));

    m_checkDoc = new DCheckBox(this);
   // m_checkDoc->setGeometry(270, 401, 95, 29);
    m_checkDoc->setText(tr("doc"));
 //   m_checkDoc->setText(tr("文档"));
    connect(m_checkDoc, SIGNAL(clicked()), this, SLOT(onDocCheck()));

    m_checkZip = new DCheckBox(this);
   // m_checkDoc->setGeometry(270, 401, 95, 29);
    m_checkZip->setText(tr("zip"));
 //   m_checkZip->setText(tr("压缩包"));
    connect(m_checkZip, SIGNAL(clicked()), this, SLOT(onZipCheck()));

    hlyt->addWidget(m_checkAll);
    hlyt->addWidget(m_checkVideo);
    hlyt->addWidget(m_checkPicture);
    hlyt->addWidget(m_checkAudio);
    hlyt->addWidget(m_checkDoc);
    hlyt->addWidget(m_checkZip);
    hlyt->addWidget(m_checkOther);

    m_checkWidget->setLayout(hlyt);
    addContent(m_checkWidget);
    addSpacing(4);

    m_editDir = new DFileChooserEdit(this);
    m_editDir->lineEdit()->setReadOnly(true);
    m_editDir->lineEdit()->setClearButtonEnabled(false);
    m_editDir->setFileMode(QFileDialog::FileMode::DirectoryOnly);
    connect(m_editDir, &DFileChooserEdit::fileChoosed, this, &CreateTaskWidget::onFilechoosed);
    QString savePath =  Settings::getInstance()->getDownloadSavePath();
    m_editDir->setText(savePath);
    addContent(m_editDir);
    addSpacing(10);
    m_defaultDownloadDir = savePath;

    QWidget *boxBtn= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(boxBtn);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    DIconButton *iconBtn= new DIconButton(boxBtn);
    QIcon tryIcon1=QIcon(QIcon::fromTheme("dcc_bt"));
    iconBtn->setIcon(tryIcon1);
    iconBtn->setIconSize(QSize(18,15));
    iconBtn->setFixedSize(QSize(36,36));
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
    layout_right->addSpacing(10);
    layout_right->addWidget(m_sureButton);
    layout->addWidget(rightBox);
    addContent(boxBtn);

    setMaximumSize(521,575);
    setMinimumSize(521,575);

    connect(m_analysisUrl, SIGNAL(sendFinishedUrl(LinkInfo*)), this, SLOT(updataTabel(LinkInfo*)));
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
            close();
        }
        delete dialog;
    }
}

void CreateTaskWidget::onCancelBtnClicked()
{
    close();
}

void CreateTaskWidget::onSureBtnClicked()
{
    QVector<LinkInfo *> urlList;
    for(int i = 0;i < m_model->rowCount();i++) {
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            LinkInfo *linkInfo;
            linkInfo = new LinkInfo;
            linkInfo->urlName = m_model->data(m_model->index(i, 1)).toString();
            linkInfo->type = m_model->data(m_model->index(i, 2)).toString();
            linkInfo->urlSize= m_model->data(m_model->index(i, 3)).toString();
            linkInfo->length = m_model->data(m_model->index(i, 4)).toLongLong();
            linkInfo->url = m_model->data(m_model->index(i, 5)).toString();
            linkInfo->urlTrueLink = m_model->data(m_model->index(i, 6)).toString();
            urlList.append(linkInfo);
        }
    }
    Settings::getInstance()->setCustomFilePath(m_defaultDownloadDir);
    emit downloadWidgetCreate(urlList,m_defaultDownloadDir);
    m_texturl->clear();
    hide();
}

void CreateTaskWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //数据中是否包含URL
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); //如果是则接受动作
    }
    else
        event->ignore();
}


void CreateTaskWidget::dropEvent(QDropEvent *event)
{

    const QMimeData *mineData=event->mimeData();//获取MIME数据
    if(mineData->hasUrls()){        //如数据中包含URL
        QList<QUrl>urlList=mineData->urls();    //获取URL列表
        if(urlList.isEmpty())return ;
        QString fileName;
        for(int i=0;i<urlList.size();++i){
            fileName=urlList.at(i).toString();
            if(fileName.isEmpty())
                return;
            if(!fileName.isEmpty()){                     //若文件路径不为空
                if(fileName.startsWith("file:")&&fileName.endsWith(".torrent")){
                    fileName=fileName.right(fileName.length()-7);
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
                else{

                }
            }
       }
    }
}

void CreateTaskWidget::setUrl(QString url)
{
    QString setTextUrl;
    QString textUrl = m_texturl->toPlainText();
    if(textUrl.isEmpty()){
        setTextUrl = url;
    }
    else {
        setTextUrl = textUrl + "\n" + url;
    }
    m_texturl->setText(setTextUrl);
    QString savePath =  Settings::getInstance()->getDownloadSavePath();
    m_editDir->setText(savePath);
}

bool CreateTaskWidget::isMagnet(QString url)
{
    QString str = url;
    if(str.mid(0,20) == "magnet:?xt=urn:btih:"){
        return  true;
    }
    else{
        return  false;
    }
}

bool CreateTaskWidget::isHttp(QString url)
{
    if( (-1 == url.indexOf("ftp:")) && (-1 == url.indexOf("http://")) && (-1 == url.indexOf("https://"))){
        return false;
    }
    return true;
}

void CreateTaskWidget::onTextChanged()
{
//    if(m_texturl->toPlainText().isEmpty()){
//        hideTableWidget();
//    }
    m_texturl->toPlainText().isEmpty()? hideTableWidget(): showTableWidget();
//    if(m_texturl->toPlainText().isEmpty()){
//        hideTableWidget();
//    }
//    else {

//    }

    QStringList urlList = m_texturl->toPlainText().split("\n");
    for (int i = 0; i< urlList.size(); i++) {
        if(urlList[i] == ""){
            if(i <= m_model->rowCount()){
                m_model->takeRow(i);
            }
        }
    }
    urlList.removeAll(QString(""));
    QMap<QString, LinkInfo> urlInfoMap;
    for (int i = 0; i < urlList.size(); i++) {
        urlList[i] = urlList[i].simplified();
    }
    urlList.removeAll(QString(""));
    urlList.removeDuplicates();

    for (int i = 0 ; i < urlList.size(); i++) {
        if(isMagnet(urlList[i])){

            continue;
        }
        QString name;
        QString type;
        getUrlToName(urlList[i],name,type);
        setData(i, name, type, "", urlList[i], 0,urlList[i]);

        LinkInfo urlInfo;
        urlInfo.url = urlList[i];
        urlInfo.index = i;
        urlInfoMap.insert(urlList[i],urlInfo);
    }

    while(urlList.size() < m_model->rowCount()){
        m_model->takeRow(m_model->rowCount()-1);
    }

    m_analysisUrl->setUrlList(urlInfoMap);
}

void CreateTaskWidget::onFilechoosed(const QString &filename)
{
    QFileInfo fileinfo;
    QString strPath;
    fileinfo.setFile(filename);
    if(!fileinfo.isWritable()){
       MessageBox *msg=new MessageBox();
       QString title = tr("Permission denied. Please try other folder.");
       msg->setWarings(title, tr("sure"));
       msg->exec();
       strPath = m_editDir->directoryUrl().toString();
      // QString _text = this->getFileEditText(m_defaultDownloadDir);
       m_editDir->lineEdit()->setText(m_defaultDownloadDir);
       m_editDir->setDirectoryUrl(m_defaultDownloadDir);
       return;
    }
    m_editDir->lineEdit()->setText(filename);
    m_editDir->setDirectoryUrl(filename);
    m_defaultDownloadDir = filename;
}

void CreateTaskWidget::getTruetUrl(QString redirecUrl)
{
    if(isMagnet(redirecUrl)){
       // emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir, "");
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
                                   //  emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,"");
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
                                      //  emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,urlNameForZH);
                                        return ;
                                    }
                                }
                                break;
                            }
                            case 302: // redirect (Location: [URL])  重定向链接
                            {
                                QString strUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
                              //  emit downloadWidgetCreate(QStringList(strUrl),m_defaultDownloadDir, "");
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
                                       //     emit downloadWidgetCreate(QStringList(redirecUrl),m_defaultDownloadDir,urlNameForZH);
                                            return ;
                                        }
                                    }
                                    break;
                            }
                            default:
                            {
                                QString warningMsg = QString(tr("The address you entered cannot be resolved correctly"));
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
    Q_UNUSED(event);
    m_texturl->clear();
}

void CreateTaskWidget::showNetErrorMsg()
{
    MessageBox *msg = new MessageBox();
    msg->setWarings(tr("Unable to connect to the network the internet connection failed"), tr("sure"), "");     //网络连接失败
    msg->exec();
}

void CreateTaskWidget::updataTabel(LinkInfo *linkInfo)
{
    setData(linkInfo->index,linkInfo->urlName, linkInfo->type, linkInfo->urlSize, linkInfo->url, linkInfo->length, linkInfo->urlTrueLink);
}

void CreateTaskWidget::onAllCheck()
{
    int state = m_checkAll->checkState();
    for(int i = 0;i < m_model->rowCount();i++) {
        m_model->setData(m_model->index(i, 0), state == Qt::Checked? "1": 0);
    }
    m_checkVideo->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);
    m_checkAudio->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);
    m_checkPicture->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);
    m_checkOther->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);
    m_checkDoc->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);
    m_checkZip->setCheckState(state == Qt::Checked? Qt::Checked: Qt::Unchecked);

}

void CreateTaskWidget::onVideoCheck()
{
    int state = m_checkVideo->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isVideo(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::onAudioCheck()
{
    int state = m_checkAudio->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isAudio(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::onPictureCheck()
{
    int state = m_checkPicture->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isPicture(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::onZipCheck()
{
    int state = m_checkZip->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isZip(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::onDocCheck()
{
    int state = m_checkDoc->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isDoc(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::onOtherCheck()
{
    int state = m_checkOther->checkState();
    if(m_checkVideo->checkState() == Qt::Checked
            && m_checkAudio->checkState() == Qt::Checked
            && m_checkPicture->checkState() == Qt::Checked
            && m_checkOther->checkState() == Qt::Checked
            && m_checkDoc->checkState() == Qt::Checked
            && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }
    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(!isVideo(ext) && !isAudio(ext) && !isPicture(ext) && !isDoc(ext) && !isZip(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
}

void CreateTaskWidget::getUrlToName(QString url, QString &name ,QString &type)
{
    // 获取url文件名
    if(url.startsWith("magnet")) {
        name = url.split("&")[0];
        if(name.contains("btih:")) {
            name = name.split("btih:")[1] + ".torrent";
            type = ".torrent";
        } else {
            name = url.right(40);
            type = ".torrent";
        }
        return;
    }
     name = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
    // 对url进行转码
    if(!name.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        const QByteArray byte = name.toLatin1();
        QString decode = QUrl::fromPercentEncoding(byte);
        if(decode.contains("?")) {
            decode = decode.split("?")[0];
        }
        name = decode;
    }
    QMimeDatabase db;
    type = db.suffixForFileName(name);
    name = name.mid(0,name.size() - type.size()-1);
    name = QUrl::fromPercentEncoding(name.toUtf8());

    return;
}

void CreateTaskWidget::setData(int index, QString name,QString type, QString size, QString url, long length, QString trueUrl)
{

    m_model->setItem(index, 0, new QStandardItem( size == "" ? "0" : "1"));
    if(!name.isNull() && !m_model->item(index, 1))
        m_model->setItem(index, 1, new QStandardItem(name));

    if(!type.isEmpty())
        m_model->setItem(index, 2, new QStandardItem(type));
    m_model->setItem(index, 3, new QStandardItem(size));
    m_model->setItem(index, 4, new QStandardItem(QString::number(length)));
    m_model->setItem(index, 5, new QStandardItem(url));
    m_model->setItem(index, 6, new QStandardItem(trueUrl));
    m_tableView->setColumnWidth(0, 290);
    m_tableView->setColumnWidth(2, 60);
    m_tableView->setColumnHidden(1, true);
    m_tableView->setColumnHidden(4, true);
    m_tableView->setColumnHidden(5, true);
    m_tableView->setColumnHidden(6, true);
    m_tableView->setColumnHidden(7, true);

    long total = 0;
    for (int i = 0; i < m_model->rowCount();i++) {
        total += m_model->data(m_model->index(i, 4)).toString().toLong();
    }
    QString totalSize = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelFileSize->setText(QString(tr("Total ")+ totalSize));

    updateSelectedInfo();


}

void CreateTaskWidget::updateSelectedInfo()
{
//    long total = 0;
//    for(int i = 0;i < m_model->rowCount();i++) {
//        QString v = m_model->data(m_model->index(i, 0)).toString();
//        QString type = m_model->data(m_model->index(i, 2)).toString();
//        if(v == "1") {
//            total += m_model->data(m_model->index(i, 4)).toString().toLong();

//        }
//    }
//    m_sureButton->setEnabled(total> 0? true : false);
    int cnt = 0;
    long total = 0;
    int selectVideoCount = 0;
    int selectAudioCount = 0;
    int selectPictureCount = 0;
    int selectOtherCount = 0;
    int selectZipCount = 0;
    int selectDocCount = 0;
    int allVideo = 0;
    int allAudio = 0;
    int allPic = 0;
    int allOther = 0;
    int allZip = 0;
    int allDoc = 0;
    int all = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString v = m_model->data(m_model->index(i, 0)).toString();
        QString type = m_model->data(m_model->index(i, 2)).toString();
        if(v == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            if(isVideo(type)){
                selectVideoCount++;
            }
            else if(isAudio(type)){
                selectAudioCount++;
            }
            else if(isPicture(type)){
                selectPictureCount++;
            }
            else if (isZip(type)) {
                selectZipCount++;
            }
            else if (isDoc(type)) {
                selectDocCount;
            }
            else {
                selectOtherCount++;
            }
            cnt++;
        }
    }
    for(int i = 0;i < m_model->rowCount();i++) {
        QString type = m_model->data(m_model->index(i, 2)).toString();
            if(isVideo(type)){
                allVideo++;
            }
            else if(isAudio(type)){
                allAudio++;
            }
            else if(isPicture(type)){
                allPic++;
            }
            else if (isDoc(type)) {
                allDoc++;
            }
            else if (isZip(type)) {
                allZip++;
            }
            else {
                allOther++;
            }
    }
    allVideo == selectVideoCount && allVideo > 0 ? m_checkVideo->setCheckState(Qt::Checked) : m_checkVideo->setCheckState(Qt::Unchecked);
    allAudio == selectAudioCount && allAudio > 0 ? m_checkAudio->setCheckState(Qt::Checked) : m_checkAudio->setCheckState(Qt::Unchecked);
    allPic == selectPictureCount && allPic > 0 ? m_checkPicture->setCheckState(Qt::Checked) : m_checkPicture->setCheckState(Qt::Unchecked);
    allOther == selectOtherCount && allOther > 0? m_checkOther->setCheckState(Qt::Checked) : m_checkOther->setCheckState(Qt::Unchecked);
    allZip == selectZipCount && allZip > 0 ? m_checkZip->setCheckState(Qt::Checked) : m_checkZip->setCheckState(Qt::Unchecked);
    allDoc == selectDocCount && allDoc > 0 ? m_checkDoc->setCheckState(Qt::Checked) : m_checkDoc->setCheckState(Qt::Unchecked);
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt> 0? true : false);
}

void CreateTaskWidget::setUrlName(int index, QString name)
{
    QList<Task> taskList;
    DBInstance::getAllTask(taskList);
    QString curName =  name + "." + m_model->data(m_model->index(index, 2)).toString();
    for (int i = 0; i < taskList.size(); i++) {
        if(taskList[i].downloadFilename == curName){
            return;
        }
    }
    m_model->setItem(index, 1, new QStandardItem(name));
    m_tableView->setColumnHidden(1, true);
}

void CreateTaskWidget::hideTableWidget()
{
    if(m_tableView->isHidden()){
        return;
    }
    m_tableView->hide();
    m_labelSelectedFileNum->hide();
    m_labelFileSize->hide();
    m_checkWidget->hide();
    m_editDir->hide();
    setMaximumSize(521,321);
    setMinimumSize(521,321);


    QDesktopWidget *deskdop = QApplication::desktop();

    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
}

void CreateTaskWidget::showTableWidget()
{
    if(!m_tableView->isHidden()){
        return;
    }

    m_tableView->show();
    m_labelSelectedFileNum->show();
    m_labelFileSize->show();
    m_checkWidget->show();
    m_editDir->show();
    setMaximumSize(521,575);
    setMinimumSize(521,575);

    QDesktopWidget *deskdop = QApplication::desktop();

    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
}

bool CreateTaskWidget::isVideo(QString ext) {
    QString types = "avi,mp4,mkv,flv,f4v,wmv,rmvb,rm,mpeg,mpg,mov,ts,m4v,vob";
    return types.indexOf(ext) != -1;
}

bool CreateTaskWidget::isAudio(QString ext) {
    QString types = "mp3,ogg,wav,ape,flac,wma,midi,aac,cda";
    return types.indexOf(ext) != -1;
}

bool CreateTaskWidget::isPicture(QString ext) {
    QString types = "jpg,jpeg,gif,png,bmp,svg,psd,tif,ico";
    return types.indexOf(ext) != -1;
}
bool CreateTaskWidget::isZip(QString ext) {
    QString types = "rar,zip,cab,iso,jar,ace,7z,tar,gz,arj,lzh,uue,bz2,z,tar.gz";
    return types.indexOf(ext) != -1;
}
bool CreateTaskWidget::isDoc(QString ext) {
    QString types = "txt,doc,xls,ppt,docx,xlsx,pptx";
    return types.indexOf(ext) != -1;
}
