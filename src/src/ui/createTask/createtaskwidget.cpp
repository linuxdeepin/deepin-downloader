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
#include "btinfodialog.h"
#include "messagebox.h"
#include "btinfotableview.h"
#include "taskdelegate.h"
#include "btheaderview.h"
#include "analysisurl.h"
#include "func.h"
#include "../database/dbinstance.h"
#include "aria2rpcinterface.h"

CreateTaskWidget::CreateTaskWidget(DDialog *parent)
    : DDialog(parent)
    , m_analysisUrl(new AnalysisUrl)
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

    QIcon tryIcon(QIcon::fromTheme(":/icons/icon/downloader2.svg"));
    setIcon(tryIcon);
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    //setTitle(tr("New Task"));

    DLabel *msgTitle = new DLabel(this);
    QString titleMsg = tr("New Task");
    msgTitle->setText(titleMsg);
    addContent(msgTitle, Qt::AlignHCenter);
    QString msg = tr("When adding download links, please enter one URL in each line");
    addSpacing(10);
    DLabel *msgLab = new DLabel(this);
    msgLab->setText(msg);
    addContent(msgLab, Qt::AlignHCenter);
    addSpacing(10);
    m_texturl = new DTextEdit(this);

    m_texturl->setReadOnly(false);
    m_texturl->setAcceptDrops(false);
    m_texturl->setPlaceholderText(tr("Enter download links or drag torrent file here"));
    m_texturl->setFixedSize(QSize(500, 154));
   // m_texturl->document()->setMaximumBlockCount(60);
    connect(m_texturl, &DTextEdit::textChanged, this, &CreateTaskWidget::onTextChanged);
    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0, 0, 0, 20));
    m_texturl->setPalette(pal);
    addContent(m_texturl);
    addSpacing(10);

    m_tableView = new BtInfoTableView(this);
    m_tableView->setMouseTracking(true);
    m_tableView->setShowGrid(false);
    m_tableView->setFrameShape(QAbstractItemView::NoFrame);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::EditKeyPressed);
    m_tableView->setAlternatingRowColors(true);

    QFont font;
    font.setPixelSize(13);
    m_tableView->setFont(font);
    headerView *header = new headerView(Qt::Horizontal, m_tableView);
    // header->setStyleSheet("QHeaderView::section stop: 70 #ffffff， stop");
    m_tableView->setHorizontalHeader(header);
    header->setDefaultAlignment(Qt::AlignLeft);
    header->setHighlightSections(false);
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
    m_tableView->setColumnWidth(2, 60);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    DFontSizeManager::instance()->bind(m_tableView, DFontSizeManager::SizeType::T6, 0);
    connect(m_tableView, &BtInfoTableView::hoverChanged, m_delegate, &TaskDelegate::onhoverChanged);
    //connect(m_delegate, &TaskDelegate::editChange, m_tableView, &BtInfoTableView::onEditChange);
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
    m_labelFileSize->setText(QString(tr("Total ") + "0"));
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
    m_checkAll->setText(tr("All"));
    connect(m_checkAll, SIGNAL(clicked()), this, SLOT(onAllCheck()));

    m_checkVideo = new DCheckBox(this);
    m_checkVideo->setText(tr("Videos"));
    connect(m_checkVideo, SIGNAL(clicked()), this, SLOT(onVideoCheck()));

    m_checkPicture = new DCheckBox(this);
    m_checkPicture->setText(tr("Pictures"));
    connect(m_checkPicture, SIGNAL(clicked()), this, SLOT(onPictureCheck()));

    m_checkAudio = new DCheckBox(this);
    m_checkAudio->setText(tr("Music"));
    connect(m_checkAudio, SIGNAL(clicked()), this, SLOT(onAudioCheck()));

    m_checkOther = new DCheckBox(this);
    m_checkOther->setText(tr("Others"));
    connect(m_checkOther, SIGNAL(clicked()), this, SLOT(onOtherCheck()));

    m_checkDoc = new DCheckBox(this);
    m_checkDoc->setText(tr("doc"));
    connect(m_checkDoc, SIGNAL(clicked()), this, SLOT(onDocCheck()));

    m_checkZip = new DCheckBox(this);
    m_checkZip->setText(tr("zip"));
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

    //下载路径所在分区剩余磁盘容量
    m_defaultDownloadDir = Settings::getInstance()->getDownloadSavePath();
    QHBoxLayout *siezhlyt = new QHBoxLayout(this);
    siezhlyt->addStretch();
    QPalette palFree;
    palFree.setColor(QPalette::WindowText, QColor("#8AA1B4"));
    QFont fontFree;
    fontFree.setPixelSize(12);
    m_labelCapacityFree = new DLabel();
    QString freeSize = Aria2RPCInterface::instance()->getCapacityFree(m_defaultDownloadDir);
    m_labelCapacityFree->setPalette(palFree);
    m_labelCapacityFree->setFont(fontFree);
    m_labelCapacityFree->setText(QString(tr("Available:") + freeSize));
    siezhlyt->addWidget(m_labelCapacityFree, 0, Qt::AlignRight);

    m_editDir = new DFileChooserEdit(this);
    m_editDir->lineEdit()->setReadOnly(true);
    m_editDir->lineEdit()->setClearButtonEnabled(false);
    m_editDir->lineEdit()->setEnabled(false);
    m_editDir->lineEdit()->setTextMargins(0,0,m_editDir->lineEdit()->width(),0);
    m_editDir->lineEdit()->setLayout(siezhlyt);
    m_editDir->setFileMode(QFileDialog::FileMode::DirectoryOnly);
    connect(m_editDir, &DFileChooserEdit::fileChoosed, this, &CreateTaskWidget::onFilechoosed);
    m_editDir->setText(m_defaultDownloadDir);
    addContent(m_editDir);
    addSpacing(10);

    QWidget *boxBtn = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(boxBtn);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    DIconButton *iconBtn = new DIconButton(boxBtn);
    //  QIcon tryIcon1(QIcon::fromTheme("dcc_bt"));
    iconBtn->setIcon(QIcon::fromTheme("dcc_bt"));
    iconBtn->setIconSize(QSize(18, 15));
    iconBtn->setFixedSize(QSize(36, 36));
    connect(iconBtn, &DIconButton::clicked, this, &CreateTaskWidget::onFileDialogOpen);
    iconBtn->setToolTip(tr("Select file"));
    layout->addWidget(iconBtn);

    QWidget *rightBox = new QWidget(boxBtn);
    QHBoxLayout *layout_right = new QHBoxLayout(rightBox);
    layout->setSpacing(10);
    layout_right->setContentsMargins(0, 0, 0, 0);

    DPushButton *cancelButton = new DPushButton(boxBtn);
    QSizePolicy policy;
    policy = cancelButton->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    cancelButton->setSizePolicy(policy);
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, &DPushButton::clicked, this, &CreateTaskWidget::onCancelBtnClicked);
    layout_right->addWidget(cancelButton);

    m_sureButton = new DSuggestButton(boxBtn);
    m_sureButton->setText(tr("Confirm"));
    policy = m_sureButton->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_sureButton->setSizePolicy(policy);
    connect(m_sureButton, &DPushButton::clicked, this, &CreateTaskWidget::onSureBtnClicked);
    layout_right->addSpacing(10);
    layout_right->addWidget(m_sureButton);
    layout->addWidget(rightBox);
    addContent(boxBtn);

    setMaximumSize(521, 575);
    setMinimumSize(521, 575);
    connect(m_analysisUrl, SIGNAL(sendFinishedUrl(LinkInfo *)), this, SLOT(updataTabel(LinkInfo *)));
}

void CreateTaskWidget::onFileDialogOpen()
{
    QString btFile = DFileDialog::getOpenFileName(this, tr("Choose Torrent File"), QDir::homePath(), "*.torrent");
    if (btFile != "") {
        //QString _savePath =  Settings::getInstance()->getDownloadSavePath();
        BtInfoDialog dialog(btFile, m_defaultDownloadDir); //= new BtInfoDialog(); //torrent文件路径
        if (dialog.exec() == QDialog::Accepted) {
            QMap<QString, QVariant> opt;
            QString infoName;
            QString infoHash;
            dialog.getBtInfo(opt, infoName, infoHash);
            emit downLoadTorrentCreate(btFile, opt, infoName, infoHash);
            close();
        }
    }
}

void CreateTaskWidget::onCancelBtnClicked()
{
    if (m_analysisUrl != nullptr) {
        delete m_analysisUrl;
        m_analysisUrl = nullptr;
    }
    close();
}

void CreateTaskWidget::onSureBtnClicked()
{
    if (!Func::isNetConnect()) {
        showNetErrorMsg();
        return;
    }

    QVector<LinkInfo> urlList;
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            LinkInfo linkInfo;
            linkInfo.urlName = m_model->data(m_model->index(i, 1)).toString();
            linkInfo.type = m_model->data(m_model->index(i, 2)).toString();
            linkInfo.urlSize = m_model->data(m_model->index(i, 3)).toString();
            linkInfo.length = m_model->data(m_model->index(i, 4)).toLongLong();
            linkInfo.url = m_model->data(m_model->index(i, 5)).toString();
            linkInfo.urlTrueLink = m_model->data(m_model->index(i, 6)).toString();
            urlList.append(linkInfo);
        }
    }
    Settings::getInstance()->setCustomFilePath(m_defaultDownloadDir);
    emit downloadWidgetCreate(urlList, m_defaultDownloadDir);

    m_texturl->clear();
    if (m_analysisUrl != nullptr) {
        delete m_analysisUrl;
        m_analysisUrl = nullptr;
    }
    hide();
}

void CreateTaskWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //数据中是否包含URL
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); //如果是则接受动作
    } else {
        event->ignore();
    }
}

void CreateTaskWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mineData = event->mimeData(); //获取MIME数据
    if (!mineData->hasUrls()) { //如数据中包含URL
        return;
    }
    QList<QUrl> urlList = mineData->urls(); //获取URL列表
    if (urlList.isEmpty())
        return;
    QString fileName;
    for (int i = 0; i < urlList.size(); ++i) {
        fileName = urlList.at(i).toString();
        if (!fileName.isEmpty()) { //若文件路径不为空
            if (fileName.startsWith("file:") && fileName.endsWith(".torrent")) {
                fileName = fileName.right(fileName.length() - 7);
                BtInfoDialog dialog(fileName, m_defaultDownloadDir); //= new BtInfoDialog(); //torrent文件路径
                int ret = dialog.exec();
                if (ret == QDialog::Accepted) {
                    QMap<QString, QVariant> opt;
                    QString infoName;
                    QString infoHash;
                    dialog.getBtInfo(opt, infoName, infoHash);
                    emit downLoadTorrentCreate(fileName, opt, infoName, infoHash);
                    close();
                }
            }
        }
    }
}

void CreateTaskWidget::setUrl(QString url)
{
    if (m_analysisUrl == nullptr) {
        m_analysisUrl = new AnalysisUrl;
        connect(m_analysisUrl, SIGNAL(sendFinishedUrl(LinkInfo *)), this, SLOT(updataTabel(LinkInfo *)));
    }
    QString setTextUrl;
    QString textUrl = m_texturl->toPlainText();
    if (textUrl.isEmpty()) {
        setTextUrl = url;
    } else {
        setTextUrl = textUrl + "\n" + url;
    }
    m_texturl->setText(setTextUrl);
    QString savePath = Settings::getInstance()->getDownloadSavePath();
    m_editDir->setText(savePath);
    m_defaultDownloadDir = savePath;
}

bool CreateTaskWidget::isMagnet(QString url)
{
    url = url.toLower();
    return url.mid(0, 20) == "magnet:?xt=urn:btih:";
}

bool CreateTaskWidget::isHttp(QString url)
{
    url = url.toLower();
    if ((url.startsWith("ftp:")) && (url.startsWith("http://")) && (url.startsWith("https://"))) {
        return false;
    }
    return true;
}

void CreateTaskWidget::onTextChanged()
{
    m_texturl->toPlainText().isEmpty() ? hideTableWidget() : showTableWidget();

    QStringList urlList = m_texturl->toPlainText().split("\n");
    for (int i = 0; i < urlList.size(); i++) {
        if (urlList[i].isEmpty()) {
            if (i <= m_model->rowCount()) {
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

    for (int i = 0; i < urlList.size(); i++) {
        QString name;
        QString type;
        if (isMagnet(urlList[i])) {
            name = urlList[i].split("&")[0];
            if (name.contains("btih:")) {
                name = name.split("btih:")[1] + ".torrent";
            } else {
                name = urlList[i].right(40);
            }
            setData(i, name.mid(0, name.size() - 8), "torrent", "1KB", urlList[i], 1024, urlList[i]);
            continue;
        }

        getUrlToName(urlList[i], name, type);
        setData(i, name, "", "", urlList[i], 0, urlList[i]);

        LinkInfo urlInfo;
        urlInfo.url = urlList[i];
        urlInfo.index = i;
        urlInfoMap.insert(urlList[i], urlInfo);
    }
    m_analysisUrl->setUrlList(urlInfoMap);

    while (urlList.size() < m_model->rowCount()) {
        m_model->takeRow(m_model->rowCount() - 1);
    }
}

void CreateTaskWidget::onFilechoosed(const QString &filename)
{
    QFileInfo fileinfo;
    QString strPath;
    fileinfo.setFile(filename);
    if (!fileinfo.isWritable()) {
        MessageBox msg; //= new MessageBox();
        QString title = tr("Permission denied. Please try other folder.");
        msg.setWarings(title, tr("sure"));
        msg.exec();
        strPath = m_editDir->directoryUrl().toString();
        // QString _text = this->getFileEditText(m_defaultDownloadDir);
        m_editDir->lineEdit()->setText(m_defaultDownloadDir);
        m_editDir->setDirectoryUrl(m_defaultDownloadDir);
        return;
    }
    m_editDir->lineEdit()->setText(filename);
    m_editDir->setDirectoryUrl(filename);
    m_defaultDownloadDir = filename;
    //获取到更改后的大小
    QString freeSize = Aria2RPCInterface::instance()->getCapacityFree(filename);
    QPalette pal;
    pal.setColor(QPalette::WindowText, QColor("#8AA1B4"));
    QFont font;
    font.setPixelSize(12);
    m_labelCapacityFree->setText(QString(tr("Available:") + freeSize));
    m_labelCapacityFree->setPalette(pal);
    m_labelCapacityFree->setFont(font);
}

void CreateTaskWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_texturl->clear();
}

void CreateTaskWidget::showNetErrorMsg()
{
    MessageBox msg; //= new MessageBox();
    //QString title = tr("Network error, check your network and try later");
    msg.setWarings(getNetErrTip(), tr("sure"), ""); //网络连接失败
    msg.exec();
}

void CreateTaskWidget::updataTabel(LinkInfo *linkInfo)
{
    setData(linkInfo->index, linkInfo->urlName, linkInfo->type, linkInfo->urlSize, linkInfo->url, linkInfo->length, linkInfo->urlTrueLink);
}

void CreateTaskWidget::onAllCheck()
{
    int state = m_checkAll->checkState();
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
    }
    bool ret = state == Qt::Checked;
    m_checkVideo->setCheckState(ret ? Qt::Checked : Qt::Unchecked);
    m_checkAudio->setCheckState(ret ? Qt::Checked : Qt::Unchecked);
    m_checkPicture->setCheckState(ret ? Qt::Checked : Qt::Unchecked);
    m_checkOther->setCheckState(ret ? Qt::Checked : Qt::Unchecked);
    m_checkDoc->setCheckState(ret ? Qt::Checked : Qt::Unchecked);
    m_checkZip->setCheckState(ret ? Qt::Checked : Qt::Unchecked);

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isVideo(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));

    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onVideoCheck()
{
    int state = m_checkVideo->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isVideo(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onAudioCheck()
{
    int state = m_checkAudio->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isAudio(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onPictureCheck()
{
    int state = m_checkPicture->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isPicture(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onZipCheck()
{
    int state = m_checkZip->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isZip(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onDocCheck()
{
    int state = m_checkDoc->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (isDoc(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::onOtherCheck()
{
    int state = m_checkOther->checkState();
    if (m_checkVideo->checkState() == Qt::Checked
        && m_checkAudio->checkState() == Qt::Checked
        && m_checkPicture->checkState() == Qt::Checked
        && m_checkOther->checkState() == Qt::Checked
        && m_checkDoc->checkState() == Qt::Checked
        && m_checkZip->checkState() == Qt::Checked) {
        m_checkAll->setCheckState(Qt::Checked);
    } else {
        m_checkAll->setCheckState(Qt::Unchecked);
    }
    long total = 0;
    int cnt = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if (!isVideo(ext) && !isAudio(ext) && !isPicture(ext) && !isDoc(ext) && !isZip(ext)) {
            if (m_model->data(m_model->index(i, 4)).toString().toLong() > 0) {
                m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
            }
        }
        if (m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            cnt++;
        }
    }
    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::getUrlToName(QString url, QString &name, QString &type)
{
    // 获取url文件名
    if (url.startsWith("magnet")) {
        if (url.split("&").size() < 0) {
            return;
        }
        name = url.split("&")[0];
        if (name.contains("btih:")) {
            if (name.split("btih:").size() < 1) {
                return;
            }
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
    if (!name.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        const QByteArray byte = name.toLatin1();
        QString decode = QUrl::fromPercentEncoding(byte);
        if (decode.contains("?")) {
            decode = decode.split("?")[0];
        }
        name = decode;
    }
    QMimeDatabase db;
    type = db.suffixForFileName(name);
    if (!type.isNull()) {
        name = name.mid(0, name.size() - type.size() - 1);
    } else {
        QStringList splitStr = name.split("?");
        if (splitStr.size() > 0)
            ;
        {
            type = db.suffixForFileName(splitStr[0]);
        }
        if (type.isNull()) {
            type = "error";
        }
    }
    name = QUrl::fromPercentEncoding(name.toUtf8());
}

void CreateTaskWidget::setData(int index, QString name, QString type, QString size, QString url, long length, QString trueUrl)
{
    m_model->setItem(index, 0, new QStandardItem(size == "" ? "0" : "1"));
    //if (!name.isNull() && !m_model->item(index, 1))
    //bool ret = !m_model->item(index, 1);
    if (!name.isNull())
        m_model->setItem(index, 1, new QStandardItem(name));

    m_model->setItem(index, 2, new QStandardItem(type));

    if (type == "html" && size.isNull()) {
        m_model->setItem(index, 3, new QStandardItem("1KB"));
        m_model->setItem(index, 4, new QStandardItem(QString::number(1024)));
    } else {
        m_model->setItem(index, 3, new QStandardItem(size));
        m_model->setItem(index, 4, new QStandardItem(QString::number(length)));
    }

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
    for (int i = 0; i < m_model->rowCount(); i++) {
        total += m_model->data(m_model->index(i, 4)).toString().toLong();
    }
    QString totalSize = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelFileSize->setText(QString(tr("Total ") + totalSize));

    updateSelectedInfo();
}

void CreateTaskWidget::updateSelectedInfo()
{
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
    for (int i = 0; i < m_model->rowCount(); i++) {
        QString v = m_model->data(m_model->index(i, 0)).toString();
        QString type = m_model->data(m_model->index(i, 2)).toString();
        if (v == "1") {
            total += m_model->data(m_model->index(i, 4)).toString().toLong();
            if (isVideo(type)) {
                selectVideoCount++;
            } else if (isAudio(type)) {
                selectAudioCount++;
            } else if (isPicture(type)) {
                selectPictureCount++;
            } else if (isZip(type)) {
                selectZipCount++;
            } else if (isDoc(type)) {
                selectDocCount++;
            } else {
                selectOtherCount++;
            }
            cnt++;
        }
        if (isVideo(type)) {
            allVideo++;
        } else if (isAudio(type)) {
            allAudio++;
        } else if (isPicture(type)) {
            allPic++;
        } else if (isDoc(type)) {
            allDoc++;
        } else if (isZip(type)) {
            allZip++;
        } else {
            allOther++;
        }
    }
    for (int i = 0; i < m_model->rowCount(); i++) {
    }
    (allVideo == selectVideoCount) && (allVideo > 0) ? m_checkVideo->setCheckState(Qt::Checked) : m_checkVideo->setCheckState(Qt::Unchecked);
    (allAudio == selectAudioCount) && (allAudio > 0) ? m_checkAudio->setCheckState(Qt::Checked) : m_checkAudio->setCheckState(Qt::Unchecked);
    (allPic == selectPictureCount) && (allPic > 0) ? m_checkPicture->setCheckState(Qt::Checked) : m_checkPicture->setCheckState(Qt::Unchecked);
    (allOther == selectOtherCount) && (allOther > 0) ? m_checkOther->setCheckState(Qt::Checked) : m_checkOther->setCheckState(Qt::Unchecked);
    (allZip == selectZipCount) && (allZip > 0) ? m_checkZip->setCheckState(Qt::Checked) : m_checkZip->setCheckState(Qt::Unchecked);
    (allDoc == selectDocCount) && (allDoc > 0) ? m_checkDoc->setCheckState(Qt::Checked) : m_checkDoc->setCheckState(Qt::Unchecked);

    QString size = Aria2RPCInterface::instance()->bytesFormat(total);
    m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(size));
    m_sureButton->setEnabled(cnt > 0 ? true : false);
}

void CreateTaskWidget::setUrlName(int index, QString name)
{
//    QList<TaskInfo> taskList;
//    DBInstance::getAllTask(taskList);
//    QString curName = name + "." + m_model->data(m_model->index(index, 2)).toString();
//    for (int i = 0; i < taskList.size(); i++) {
//        if (taskList[i].downloadFilename == curName) {
//            return;
//        }
//    }
//    for (int j = 0; j < m_model->rowCount(); j++) {
//        if (j == index)
//            continue;
//        if (name == m_model->data(m_model->index(index, 2)).toString())
//            return;
//    }

    m_model->setItem(index, 1, new QStandardItem(name));
    m_tableView->setColumnHidden(1, true);
}

void CreateTaskWidget::hideTableWidget()
{
    if (m_tableView->isHidden()) {
        return;
    }
    m_tableView->hide();
    m_labelSelectedFileNum->hide();
    m_labelFileSize->hide();
    m_checkWidget->hide();
    m_editDir->hide();
    setMaximumSize(521, 321);
    setMinimumSize(521, 321);

    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width()) / 2, (deskdop->height() - this->height()) / 2);
}

void CreateTaskWidget::showTableWidget()
{
    if (!m_tableView->isHidden()) {
        return;
    }
    m_tableView->show();
    m_labelSelectedFileNum->show();
    m_labelFileSize->show();
    m_checkWidget->show();
    m_editDir->show();
    setMaximumSize(521, 575);
    setMinimumSize(521, 575);

    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width()) / 2, (deskdop->height() - this->height()) / 2);
}

bool CreateTaskWidget::isVideo(QString ext)
{
    QString types = "avi,mp4,mkv,flv,f4v,wmv,rmvb,rm,mpeg,mpg,mov,ts,m4v,vob";
    return types.indexOf(ext) != -1;
}

bool CreateTaskWidget::isAudio(QString ext)
{
    QString types = "mp3,ogg,wav,ape,flac,wma,midi,aac,cda";
    return types.indexOf(ext) != -1;
}

bool CreateTaskWidget::isPicture(QString ext)
{
    QString types = "jpg,jpeg,gif,png,bmp,svg,psd,tif,ico";
    return types.indexOf(ext) != -1;
}
bool CreateTaskWidget::isZip(QString ext)
{
    QString types = "rar,zip,cab,iso,jar,ace,7z,tar,gz,arj,lzh,uue,bz2,z,tar.gz";
    return types.indexOf(ext) != -1;
}
bool CreateTaskWidget::isDoc(QString ext)
{
    QString types = "txt,doc,xls,ppt,docx,xlsx,pptx";
    return types.indexOf(ext) != -1;
}

QString CreateTaskWidget::getNetErrTip()
{
    return QString(tr("Network error, check your network and try later"));
}
