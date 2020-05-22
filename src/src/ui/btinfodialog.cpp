#include "btinfodialog.h"

#include <QDir>
#include <QFont>
#include <QProcess>

#include <DHeaderView>
#include <DFileDialog>
#include <DMessageBox>
#include <QPixmap>
#include "aria2rpcinterface.h"


BtInfoDialog::BtInfoDialog(QString torrentFile, QString bt_last_save_path)
    :DDialog ()
{
    this->m_torrentFile = torrentFile;
    this->m_defaultDownloadDir = bt_last_save_path;
    this->setFixedSize(500, 525);

    this->setIcon(QIcon::fromTheme(":/icons/images/icon/downloader3.svg"));
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
    return this->m_editDir->text();
}

QString BtInfoDialog::getName()
{
    return this->m_labelInfoName->text();
}



void BtInfoDialog::initUI()
{
   m_ariaInfo = Aria2RPCInterface::Instance()->getBtInfo(m_torrentFile);
   // Aria2RPCInterface::getBtInfo(this->m_torrentFile);
   //  = Aria2RPCInterface::getBtInfo(this->m_torrentFile);
    //
    this->setTitle(" ");
    this->setWindowTitle(tr(""));

    this->m_labelTitle = new DLabel(this);
    this->m_labelTitle->setFixedSize(this->width(), 30);
    this->m_labelTitle->setAlignment(Qt::AlignCenter);
    this->m_labelTitle->move(0, 48);
    this->m_labelTitle->setText(tr("Magnet/Torrent Files Select"));

    this->m_folderIcon = new DLabel(this);
    this->m_folderIcon->setPixmap(QPixmap(":/icons/deepin/builtin/light/icons/folder.svg"));///usr/share/icons/bloom/places/32/folder.svg
    this->m_folderIcon->move(45, 92);

    //下载信息名称
    this->m_labelInfoName = new DLabel(this);
    this->m_labelInfoName->setGeometry(85, 89, 356, 40);
    this->m_labelInfoName->setText("xiazai renwu ");
    this->m_labelInfoName->setWordWrap(true);

    //总大小标签
    QFont font2;
    font2.setPixelSize(12);
    QPalette pal;
    pal.setColor(QPalette::WindowText, QColor("#8AA1B4"));
    this->m_labelFileSize = new DLabel(this);
    this->m_labelFileSize->setAlignment(Qt::AlignRight);
    this->m_labelFileSize->setFont(font2);
    this->m_labelFileSize->setPalette(pal);

    //选中文件数
    this->m_labelSelectedFileNum = new DLabel(this);
    this->m_labelSelectedFileNum->setFont(font2);
    this->m_labelSelectedFileNum->setPalette(pal);

    //文件列表
    this->m_widget = new DWidget(this);
    this->m_tableView = new BtInfoTableView(m_widget);
    this->m_tableView->setMouseTracking(true);
    this->m_widget->setGeometry(15, 142, 471, 255);
    this->m_widget->setAutoFillBackground(true);

    QVBoxLayout *vb = new QVBoxLayout(m_widget);
    vb->setContentsMargins(10, 0, 10, 6);
    vb->addWidget(m_tableView);
    QHBoxLayout *hb = new QHBoxLayout();
    hb->addWidget(m_labelSelectedFileNum, Qt::AlignLeft);
    hb->addWidget(m_labelFileSize, Qt::AlignRight);
    vb->addLayout(hb);

    //Checkbox
    this->m_checkAll = new DCheckBox(this);
    this->m_checkAll->setGeometry(15, 401, 95, 29);
    this->m_checkAll->setText(tr("Check All"));
    this->m_checkAll->setChecked(true);
    connect(this->m_checkAll, SIGNAL(clicked()), this, SLOT(slot_checkAll()));

    this->m_checkVideo = new DCheckBox(this);
    this->m_checkVideo->setGeometry(105, 401, 95, 29);
    this->m_checkVideo->setText(tr("Video"));
    this->m_checkVideo->setChecked(true);
    connect(this->m_checkVideo, SIGNAL(clicked()), this, SLOT(slot_checkVideo()));

    this->m_checkAudio = new DCheckBox(this);
    this->m_checkAudio->setGeometry(195, 401, 95, 29);
    this->m_checkAudio->setText(tr("Audio"));
    this->m_checkAudio->setChecked(true);
    connect(this->m_checkAudio, SIGNAL(clicked()), this, SLOT(slot_checkAudio()));

    this->m_checkPicture = new DCheckBox(this);
    this->m_checkPicture->setGeometry(285, 401, 95, 29);
    this->m_checkPicture->setText(tr("Picture"));
    this->m_checkPicture->setChecked(true);
    connect(this->m_checkPicture, SIGNAL(clicked()), this, SLOT(slot_checkPicture()));

    this->m_checkOther = new DCheckBox(this);
    this->m_checkOther->setGeometry(375, 401, 95, 29);
    this->m_checkOther->setText(tr("Other"));
    this->m_checkOther->setChecked(true);
    connect(this->m_checkOther, SIGNAL(clicked()), this, SLOT(slot_checkOther()));

    //下载路径所在分区剩余磁盘容量
    this->m_labelCapacityFree = new DLabel();
    //this->labelCapacityFree->setGeometry(350, 363, 86, 23);
 //   this->labelCapacityFree->setText(tr("Free:") + Aria2cInterface::getCapacityFree(this->defaultDownloadDir));
    this->m_labelCapacityFree->setPalette(pal);
    this->m_labelCapacityFree->setFont(font2);

    //
    this->m_editDir = new DFileChooserEdit(this);
    this->m_editDir->setGeometry(15, 435, 471, 36);
    this->m_editDir->setText(this->m_defaultDownloadDir);
    this->m_editDir->setClearButtonEnabled(false);
    this->m_editDir->setFileMode(QFileDialog::DirectoryOnly);
    this->m_editDir->lineEdit()->setEnabled(false);
    connect(this->m_editDir, &DFileChooserEdit::fileChoosed, this, &BtInfoDialog::slot_filechoosed);

    //确定按钮
    this->m_btnOK = new DPushButton(this);
    //this->btnOK->setFixedWidth(190);
    this->m_btnOK->setGeometry(160, 480, 191, 35);
    this->m_btnOK->setText(tr("Download"));
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

    this->m_tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->m_tableView->horizontalHeader()->setHighlightSections(false);

    this->m_tableView->verticalHeader()->hide();
    this->m_tableView->verticalHeader()->setDefaultSectionSize(46);

    this->m_delegate = new BtInfoDelegate(this);
    this->m_tableView->setItemDelegate(m_delegate);

    this->m_model = new QStandardItemModel();
    this->m_tableView->setModel(this->m_model);

    m_model->setColumnCount(5);
    m_model->setHeaderData(0, Qt::Horizontal, tr("File Name"));
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

void BtInfoDialog::slot_btnSelectDir()
{
    QString dir = DFileDialog::getExistingDirectory(nullptr, tr("Select Downlod to Dir"), this->m_defaultDownloadDir);
    if(dir != "") {
        this->m_editDir->setText(dir);
      //  this->labelCapacityFree->setText(tr("Free:") + Aria2cInterface::getCapacityFree(dir));
    }
}

void BtInfoDialog::slot_btnOK()
{
    //qDebug()<<Aria2cInterface::getCapacityFree(editDir->text());
 //   long free = Aria2cInterface::getCapacityFreeByte(editDir->text());//KB
    long total = 0;//选中文件总大小（字节）
    for(int i = 0;i < m_model->rowCount();i++) {
        if(m_model->data(m_model->index(i, 0)).toString() == "1") {
            total += m_model->data(m_model->index(i, 5)).toString().toLong();
        }
    }
    //qDebug()<<free<<total/1024;
//    if(free < (total / 1024)) {//剩余空间比较 KB
//        MessageBox *msg = new MessageBox(Warnings);
//        msg->set_warning_MsgBox(tr("Disk capacity is not enough!"), tr("sure"));//磁盘容量不足！
//        msg->exec();
//        return;
//    }
    if(this->getSelected() != "") {
        QString save_path=m_editDir->text();
        QString config_path=QString("%1/%2/%3/last_save_path")
                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                .arg(qApp->organizationName())
                .arg(qApp->applicationName());
        QFile file;
        file.setFileName(config_path);
        bool isOK = file.open(QIODevice::WriteOnly);
        if (isOK == true)
        {
            file.write(save_path.toStdString().data());
        }
        file.close();
        this->close();
        this->accept();
    }
    else {
//        MessageBox *msg = new MessageBox(Warnings);
//        msg->set_warning_MsgBox(tr("Please Select Download Files!"), tr("sure"));
//        msg->exec();
    }
}

void BtInfoDialog::slot_checkAll()
{
    int state = this->m_checkAll->checkState();
    if(state == Qt::Checked) {
        for(int i = 0;i < m_model->rowCount();i++) {
            m_model->setData(m_model->index(i, 0), "1");
        }
        //this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(model->rowCount()) + tr(",Total ") + Aria2cInterface::bytesFormat(this->info.totalLengthByets));
        this->m_checkVideo->setCheckState(Qt::Checked);
        this->m_checkAudio->setCheckState(Qt::Checked);
        this->m_checkPicture->setCheckState(Qt::Checked);
        this->m_checkOther->setCheckState(Qt::Checked);
    }
    else if(state == Qt::Unchecked) {
        for(int i = 0;i < m_model->rowCount();i++) {
            m_model->setData(m_model->index(i, 0), "0");
            this->m_labelSelectedFileNum->setText(tr("Selected Files:") + "0" + tr(",Total ") + "0B");
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
    //this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(cnt) + tr(",Total ") + Aria2cInterface::bytesFormat(total));
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
 //   this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(cnt) + tr(",Total ") + Aria2cInterface::bytesFormat(total));
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
    //this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(cnt) + tr(",Total ") + Aria2cInterface::bytesFormat(total));
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
//    this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(cnt) + tr(",Total ") + Aria2cInterface::bytesFormat(total));
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
    //this->labelSelectedFileNum->setText(tr("Selected Files:") + QString::number(cnt) + tr(",Total ") + Aria2cInterface::bytesFormat(total));
}

void BtInfoDialog::slot_filechoosed(const QString &filename)
{
    QFileInfo fileinfo;
    fileinfo.setFile(filename);
    if(!fileinfo.isWritable())
    {
//        MessageBox *msg = new MessageBox(Warnings);
//        msg->set_warning_MsgBox(tr("select directory not writeable"), tr("sure"));
//        msg->exec();
        this->m_editDir->lineEdit()->setText(this->m_defaultDownloadDir);
    }
 //   this->labelCapacityFree->setText(tr("Free:") + Aria2cInterface::getCapacityFree(this->editDir->text()));
}

void BtInfoDialog::slot_paletteTypeChanged(DGuiApplicationHelper::ColorType type)
{
    int themeType = DGuiApplicationHelper::instance()->themeType();
    QPalette p;
    this->m_delegate->setHoverColor(DGuiApplicationHelper::instance()->applicationPalette().frameBorder());

    if(themeType == 1) {
        p.setColor(QPalette::Background, Qt::white);
        //this->delegate->setHoverColor(QColor(0,0,0,13));

        QPalette pal;
        pal.setColor(QPalette::WindowText, QColor("#8AA1B4"));
        this->m_labelFileSize->setPalette(pal);
        this->m_labelSelectedFileNum->setPalette(pal);
    }
    else {
        p = DGuiApplicationHelper::instance()->applicationPalette();
       // this->delegate->setHoverColor(QColor(255,255,255,26));

        this->m_labelFileSize->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        this->m_labelSelectedFileNum->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
    }
    this->m_widget->setPalette(p);

}
