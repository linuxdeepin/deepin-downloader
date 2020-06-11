/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file btinfodialog.cpp
 *
 * @brief BT窗口类
 *
 * @date 2020-06-09 10:50
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

#include "btinfodialog.h"

#include "aria2rpcinterface.h"
#include "btinfodelegate.h"
#include "btheaderview.h"
#include "btinfotableview.h"
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QPixmap>
#include <QStandardItemModel>
#include <QStandardPaths>



BtInfoDialog::BtInfoDialog(QString torrentFile, QString bt_last_save_path)
    :DDialog ()
{
    this->m_torrentFile = torrentFile;
    this->m_defaultDownloadDir = bt_last_save_path;
    this->setFixedSize(500, 525);

    this->setIcon(QIcon::fromTheme(":/icons/icon/downloader3.svg"));
    initUI();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &BtInfoDialog::slot_paletteTypeChanged);
}

BtInfoDialog::~BtInfoDialog() {
    delete m_model;
    delete m_delegate;

    //delete m_tableView;

    delete m_btnOK;

    delete m_labelTitle;
    delete m_labelSelectedFileNum;
    delete m_labelFileSize;
    delete m_labelCapacityFree;

    delete m_labelInfoName;
    delete m_editDir;

    delete m_checkAll;
    delete m_checkVideo;
    delete m_checkAudio;
    delete m_checkPicture;
    delete m_checkOther;

    delete m_widget;
}

QString BtInfoDialog::getSelected()
{
    QString selected = "";
    for(int i = 0;i < m_model->rowCount();i++) {
        QString checked =  m_model->data(m_model->index(i, 0)).toString();
        if(checked == "1") {
            QString index = m_model->data(m_model->index(i, 4)).toString();
            selected += index;
            selected += ",";
        }
    }
    return selected.mid(0, selected.length() - 1);
}

QString BtInfoDialog::getSaveto()
{
    return this->m_editDir->text().split("  ")[0];
}

QString BtInfoDialog::getName()
{
    return this->m_labelInfoName->text();
}



void BtInfoDialog::initUI()
{
    m_ariaInfo = Aria2RPCInterface::Instance()->getBtInfo(m_torrentFile);
    this->setTitle(" ");
    this->setWindowTitle(tr(""));

    this->m_labelTitle = new DLabel(this);
    this->m_labelTitle->setFixedSize(this->width(), 30);
    this->m_labelTitle->setAlignment(Qt::AlignCenter);
    this->m_labelTitle->move(0, 48);
    this->m_labelTitle->setText(tr("New Task"));
    this->m_folderIcon = new DLabel(this);
    this->m_folderIcon->setPixmap(QPixmap(":/icons/icon/folder.svg"));///usr/share/icons/bloom/places/32/folder.svg
    this->m_folderIcon->move(45, 92);

    //下载信息名称
    this->m_labelInfoName = new DLabel(this);
    this->m_labelInfoName->setGeometry(85, 89, 356, 40);
    this->m_labelInfoName->setText(m_ariaInfo.name);
    this->m_labelInfoName->setWordWrap(true);

    //总大小标签
    QFont font2;
    font2.setPixelSize(12);
    QPalette pal;
    pal.setColor(QPalette::WindowText, QColor("#8AA1B4"));
    this->m_labelFileSize = new DLabel(this);
    this->m_labelFileSize->setAlignment(Qt::AlignRight);
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(this->m_ariaInfo.totalLengthByets);
    m_labelFileSize->setText(QString(tr("Total :")+ _size));
    this->m_labelFileSize->setFont(font2);
    this->m_labelFileSize->setPalette(pal);

    //选中文件数
    this->m_labelSelectedFileNum = new DLabel(this);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(m_ariaInfo.files.size())).arg(_size));
    this->m_labelSelectedFileNum->setFont(font2);
    this->m_labelSelectedFileNum->setPalette(pal);

    //文件列表
    this->m_widget = new DWidget(this);
    this->m_tableView = new BtInfoTableView(m_widget);
    this->m_tableView->setMouseTracking(true);
    this->m_widget->setGeometry(15, 142, 471, 235);
    this->m_widget->setAutoFillBackground(true);

    QVBoxLayout *vb = new QVBoxLayout(m_widget);
    vb->setContentsMargins(10, 0, 10, 5);
    vb->addWidget(m_tableView);
    QHBoxLayout *hb = new QHBoxLayout();
    hb->addWidget(m_labelSelectedFileNum, Qt::AlignLeft);
    hb->addStretch();
    hb->addWidget(m_labelFileSize, Qt::AlignRight);
 //   hb->setGeometry(1);
    hb->setGeometry(QRect(15,381,471,20));
//    vb->addLayout(hb);

    //Checkbox
    this->m_checkAll = new DCheckBox(this);
    this->m_checkAll->setGeometry(15, 401, 95, 29);
    this->m_checkAll->setText(tr("All"));
    this->m_checkAll->setChecked(true);
    connect(this->m_checkAll, SIGNAL(clicked()), this, SLOT(slot_checkAll()));

    this->m_checkVideo = new DCheckBox(this);
    this->m_checkVideo->setGeometry(100, 401, 95, 29);
    this->m_checkVideo->setText(tr("Videos"));
    this->m_checkVideo->setChecked(true);
    connect(this->m_checkVideo, SIGNAL(clicked()), this, SLOT(slot_checkVideo()));

    this->m_checkPicture = new DCheckBox(this);
    this->m_checkPicture->setGeometry(185, 401, 95, 29);
    this->m_checkPicture->setText(tr("Pictures"));
    this->m_checkPicture->setChecked(true);
    connect(this->m_checkPicture, SIGNAL(clicked()), this, SLOT(slot_checkPicture()));

    this->m_checkAudio = new DCheckBox(this);
    this->m_checkAudio->setGeometry(270, 401, 95, 29);
    this->m_checkAudio->setText(tr("Music"));
    this->m_checkAudio->setChecked(true);
    connect(this->m_checkAudio, SIGNAL(clicked()), this, SLOT(slot_checkAudio()));

    this->m_checkOther = new DCheckBox(this);
    this->m_checkOther->setGeometry(355, 401, 95, 29);    //Aria2cInterface::bytesFormat(this->info.totalLengthByets)try(375, 401, 95, 29);
    this->m_checkOther->setText(tr("Others"));
    this->m_checkOther->setChecked(true);
    connect(this->m_checkOther, SIGNAL(clicked()), this, SLOT(slot_checkOther()));

    //下载路径所在分区剩余磁盘容量
    this->m_labelCapacityFree = new DLabel();
    this->m_labelCapacityFree->setGeometry(350, 363, 86, 23);
    QString _freeSize = Aria2RPCInterface::Instance()->getCapacityFree(this->m_defaultDownloadDir);
   // this->m_labelCapacityFree->setText(tr("Free space:") + _freeSize);
    this->m_labelCapacityFree->setPalette(pal);
    this->m_labelCapacityFree->setFont(font2);

    //
    this->m_editDir = new DFileChooserEdit(this);
    this->m_editDir->setGeometry(15, 435, 471, 36);
    QString _text = this->getFileEditText(this->m_defaultDownloadDir);
    this->m_editDir->setText(_text);
    this->m_editDir->setClearButtonEnabled(false);
    this->m_editDir->setFileMode(QFileDialog::DirectoryOnly);
    this->m_editDir->lineEdit()->setEnabled(false);
    connect(this->m_editDir, &DFileChooserEdit::fileChoosed, this, &BtInfoDialog::slot_filechoosed);

    //确定按钮
    this->m_btnOK = new DPushButton(this);
    //this->btnOK->setFixedWidth(190);
    this->m_btnOK->setGeometry(160, 480, 191, 35);
    this->m_btnOK->setText(tr("Download Now"));
    connect(this->m_btnOK, SIGNAL(clicked()), this, SLOT(slot_btnOK()));

    //文件列表配置
    this->m_tableView->setShowGrid(false);
    this->m_tableView->setSelectionMode(QAbstractItemView::NoSelection);
    this->m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->m_tableView->setAlternatingRowColors(true);
    this->m_tableView->setFrameShape(QAbstractItemView::NoFrame);

    QFont font;
    font.setPixelSize(13);
    this->m_tableView->setFont(font);

    headerView *_headerView = new headerView(Qt::Horizontal, this->m_tableView);
    this->m_tableView->setHorizontalHeader(_headerView);
    _headerView->setDefaultAlignment(Qt::AlignLeft);
    _headerView->setHighlightSections(false);

    this->m_tableView->verticalHeader()->hide();
    this->m_tableView->verticalHeader()->setDefaultSectionSize(46);

    this->m_delegate = new BtInfoDelegate(this);
    this->m_tableView->setItemDelegate(m_delegate);

    this->m_model = new QStandardItemModel();
    this->m_tableView->setModel(this->m_model);

    m_model->setColumnCount(5);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, "");
    m_model->setHeaderData(2, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Size"));
    m_model->setHeaderData(4, Qt::Horizontal, "index");

    for(int i = 0; i < m_ariaInfo.files.length();i++) {
        Aria2cBtFileInfo file = m_ariaInfo.files.at(i);
        QList<QStandardItem*> list;
        list << new QStandardItem("1");
        list << new QStandardItem(file.path.mid(file.path.lastIndexOf("/") + 1));
        list << new QStandardItem(file.path.mid(file.path.lastIndexOf(".") + 1));
        list << new QStandardItem(file.length);
        list << new QStandardItem(QString::number(file.index));
        list << new QStandardItem(QString::number(file.lengthBytes));
        this->m_model->appendRow(list);
    }

    this->m_tableView->setColumnHidden(1, true);
    this->m_tableView->setColumnHidden(4, true);
    this->m_tableView->setColumnHidden(5, true);

    this->m_tableView->setColumnWidth(0, 290);
    //this->tableView->setColumnWidth(1, 260);
    this->m_tableView->setColumnWidth(2, 60);
    //this->tableView->setColumnWidth(3, 60);
    this->m_tableView->horizontalHeader()->setStretchLastSection(true);



    connect(this->m_tableView, &BtInfoTableView::signal_hoverChanged, this->m_delegate, &BtInfoDelegate::slot_hoverChanged);

    slot_paletteTypeChanged(DGuiApplicationHelper::ColorType::LightType);
}

int BtInfoDialog::exec()
{
    QHBoxLayout layout(m_editDir->lineEdit());
    layout.addWidget(m_labelCapacityFree, 0, Qt::AlignRight | Qt::AlignCenter);
    this->m_editDir->setFixedHeight(36);

    return DDialog::exec();
}


void BtInfoDialog::slot_btnOK()
{
    if(this->getSelected().isNull())
    {
        qDebug()<<"Please Select Download Files!";
        return;
    }

    long _free = Aria2RPCInterface::Instance()->getCapacityFreeByte(m_editDir->text().split("  ")[0]);
    long _total = 0;//选中文件总大小（字节）
    for(int i = 0;i < m_model->rowCount();i++) {
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            _total += m_model->data(m_model->index(i, 5)).toString().toLong();
        }
    }
    if(_free < (_total / 1024)) {//剩余空间比较 KB
        qDebug()<<"Disk capacity is not enough!";
        return;
    }
    QString save_path=m_editDir->text().split("  ")[0];
    Settings::getInstance()->setCustomFilePath(save_path);
    this->close();
    this->accept();
}

void BtInfoDialog::slot_checkAll()
{
    int state = this->m_checkAll->checkState();
    if(state == Qt::Checked) {
        for(int i = 0;i < m_model->rowCount();i++) {
            m_model->setData(m_model->index(i, 0), "1");
        }

        QString _size = Aria2RPCInterface::Instance()->bytesFormat(this->m_ariaInfo.totalLengthByets);
        this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(m_model->rowCount())).arg(_size));
        this->m_checkVideo->setCheckState(Qt::Checked);
        this->m_checkAudio->setCheckState(Qt::Checked);
        this->m_checkPicture->setCheckState(Qt::Checked);
        this->m_checkOther->setCheckState(Qt::Checked);
    }
    else if(state == Qt::Unchecked) {
        for(int i = 0;i < m_model->rowCount();i++) {
            m_model->setData(m_model->index(i, 0), "0");
            this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(0)).arg("0GB"));
        }
        this->m_checkVideo->setCheckState(Qt::Unchecked);
        this->m_checkAudio->setCheckState(Qt::Unchecked);
        this->m_checkPicture->setCheckState(Qt::Unchecked);
        this->m_checkOther->setCheckState(Qt::Unchecked);
    }
}

bool BtInfoDialog::isVideo(QString ext) {
    QString types = "avi,mp4,mkv,flv,f4v,wmv,rmvb,rm,mpeg,mpg,mov,ts,m4v,vob";
    return types.indexOf(ext) != -1;
}

bool BtInfoDialog::isAudio(QString ext) {
    QString types = "mp3,ogg,wav,ape,flac,wma,midi,aac,cda";
    return types.indexOf(ext) != -1;
}

bool BtInfoDialog::isPicture(QString ext) {
    QString types = "jpg,jpeg,gif,png,bmp,svg,psd,tif,ico";
    return types.indexOf(ext) != -1;
}

void BtInfoDialog::slot_checkVideo()
{
    int state = this->m_checkVideo->checkState();
    if(this->m_checkVideo->checkState() == Qt::Checked
            && this->m_checkAudio->checkState() == Qt::Checked
            && this->m_checkPicture->checkState() == Qt::Checked
            && this->m_checkOther->checkState() == Qt::Checked) {
        this->m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        this->m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isVideo(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
            cnt++;
        }
    }
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(this->m_ariaInfo.totalLengthByets);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(_size));
}

void BtInfoDialog::slot_checkAudio()
{
    int state = this->m_checkAudio->checkState();
    if(this->m_checkVideo->checkState() == Qt::Checked
            && this->m_checkAudio->checkState() == Qt::Checked
            && this->m_checkPicture->checkState() == Qt::Checked
            && this->m_checkOther->checkState() == Qt::Checked) {
        this->m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        this->m_checkAll->setCheckState(Qt::Unchecked);
    }

    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(isAudio(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
            cnt++;
        }
    }
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(total);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(_size));
}

void BtInfoDialog::slot_checkPicture()
{
    int state = this->m_checkPicture->checkState();
    if(this->m_checkVideo->checkState() == Qt::Checked
            && this->m_checkAudio->checkState() == Qt::Checked
            && this->m_checkPicture->checkState() == Qt::Checked
            && this->m_checkOther->checkState() == Qt::Checked) {
        this->m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        this->m_checkAll->setCheckState(Qt::Unchecked);
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
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(total);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(_size));
}

void BtInfoDialog::slot_checkOther()
{
    int state = this->m_checkOther->checkState();
    if(this->m_checkVideo->checkState() == Qt::Checked
            && this->m_checkAudio->checkState() == Qt::Checked
            && this->m_checkPicture->checkState() == Qt::Checked
            && this->m_checkOther->checkState() == Qt::Checked) {
        this->m_checkAll->setCheckState(Qt::Checked);
    }
    else {
        this->m_checkAll->setCheckState(Qt::Unchecked);
    }
    long total = 0;
    int cnt = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString ext = m_model->data(m_model->index(i, 2)).toString();
        if(!isVideo(ext) && !isAudio(ext) && !isPicture(ext)) {
            m_model->setData(m_model->index(i, 0), state == Qt::Checked ? "1" : "0");
        }
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
            cnt++;
        }
    }
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(total);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(_size));
}

void BtInfoDialog::updateSelectedInfo()
{
    int cnt = 0;
    long total = 0;
    for(int i = 0;i < m_model->rowCount();i++) {
        QString v = m_model->data(m_model->index(i, 0)).toString();
        if(v == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
            cnt++;
        }
    }
    QString _size = Aria2RPCInterface::Instance()->bytesFormat(total);
    this->m_labelSelectedFileNum->setText(QString(tr("%1 files selected, %2")).arg(QString::number(cnt)).arg(_size));
}

void BtInfoDialog::slot_filechoosed(const QString &filename)
{
    QFileInfo fileinfo;
    fileinfo.setFile(filename);
    QString _text = this->getFileEditText(filename);
    this->m_editDir->lineEdit()->setText(_text);
    m_editDir->setDirectoryUrl(filename);
}

void BtInfoDialog::slot_paletteTypeChanged(DGuiApplicationHelper::ColorType type)
{
    int themeType = DGuiApplicationHelper::instance()->themeType();
    QPalette p;
    this->m_delegate->setHoverColor(DGuiApplicationHelper::instance()->applicationPalette().frameBorder());

    if(themeType == 1) {
        p.setColor(QPalette::Background, Qt::white);
        this->m_delegate->setHoverColor(QColor(0,0,0,13));

        QPalette pal;
        pal.setColor(QPalette::WindowText, QColor("#8AA1B4"));
        this->m_labelFileSize->setPalette(pal);
        this->m_labelSelectedFileNum->setPalette(pal);
    }
    else {
        p = DGuiApplicationHelper::instance()->applicationPalette();
        this->m_delegate->setHoverColor(QColor(255,255,255,26));

        this->m_labelFileSize->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        this->m_labelSelectedFileNum->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
    }
    this->m_widget->setPalette(p);
}

void BtInfoDialog::getBtInfo(QMap<QString,QVariant> &opt, QString &infoName, QString &infoHash)
{
    opt.insert("dir",this->m_editDir->text().split("  ")[0]);
    opt.insert("select-file",this->getSelected());
    infoName = this->m_labelInfoName->text();
    infoHash = this->m_ariaInfo.infoHash;
}

QString BtInfoDialog::getFileEditText(QString text)
{
    QString _flieEditText =  text+  "    " + tr("Free space:") + Aria2RPCInterface::Instance()->getCapacityFree(text);
    int _count = _flieEditText.count();

    for (int i =0 ; i < _flieEditText.size();i++)
    {
        //判断字符中是否包含中文或者大写字母
        if((_flieEditText[i] >= 'A' && _flieEditText[i] <= 'Z')
                || (_flieEditText[i] >= 0x4E00 && _flieEditText[i] <= 0x9FA5))

        {
            _count++;
        }
    }
    //若路径较短，则用空格进行填充
    if(_count < 61)
    {
       int _fillCount = 61 - _count;
       _flieEditText.insert(text.size(), QString(_fillCount*2, ' '));
    }
    return _flieEditText;
}
