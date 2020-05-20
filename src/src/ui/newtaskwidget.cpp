#include <newtaskwidget.h>

newTaskWidget::newTaskWidget(QString path, DDialog *parent):
    m_savePath(path),
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

    QIcon tryIcon=QIcon(":/ndm_preferencesystem_24px.svg");
    this->setIcon(tryIcon);
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
    this->setTitle(tr("New download task"));

    QString _msg = tr("When you add multiple download links, \n please make sure each row has one link.");
    this->addSpacing(15);
    DLabel * _msgLab= new DLabel(this);
    _msgLab->setText(_msg);
    this->addContent(_msgLab,Qt::AlignHCenter);
    this->addSpacing(15);
    DTextEdit *_textUrl= new DTextEdit(this);

    _textUrl->setReadOnly(false);
    _textUrl->setAcceptDrops(false);
    _textUrl->setPlaceholderText(tr("please input download address or drag file to here"));
   _textUrl->setFixedSize(QSize(454,154));
    QPalette _pal;
    _pal.setColor(QPalette::Base, QColor(0,0,0,20));
    _textUrl->setPalette(_pal);
    this->addContent(_textUrl);
    this->addSpacing(15);


    QWidget *_boxBtn= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(_boxBtn);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,10,0);
    DIconButton *_iconBtn= new DIconButton(_boxBtn);
    QIcon _tryIcon=QIcon(":/ndm_bt_11px.svg");
    _iconBtn->setIcon(_tryIcon);
    _iconBtn->setIconSize(QSize(18,15));
    _iconBtn->setFixedSize(QSize(40,35));
    connect(_iconBtn,&DIconButton::clicked,this,&newTaskWidget::openfileDialog);
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



    DSuggestButton *sure_button = new DSuggestButton(_boxBtn);
    sure_button->setText(tr("Confirm"));
    _policy = sure_button->sizePolicy();
    _policy.setHorizontalPolicy(QSizePolicy::Expanding);
    sure_button->setSizePolicy(_policy);
    connect(sure_button,&DPushButton::clicked,this,&newTaskWidget::onSureBtnClicked);
    layout_right->addSpacing(20);
    layout_right->addWidget(sure_button);
    layout->addWidget(_rightBox);
    this->addContent(_boxBtn);
}


void newTaskWidget::openfileDialog()
{
    QString _btFile = DFileDialog::getOpenFileName(this, tr("Choose Torrent File"), QDir::homePath(), "File(*.torrent)");
  //  if(_btFile != "") {
        BtInfoDialog *_dialog = new BtInfoDialog(_btFile,m_savePath);//torrent文件路径
        if(_dialog->exec() == QDialog::Accepted) {
            QMap<QString,QVariant> opt;
//            opt.insert("dir",dialog->getSaveto());
//            opt.insert("select-file",dialog->getSelected());
//            QString infoName=dialog->getName();
//            QString infoHash=dialog->getBtInfo().infoHash;
//            emit DownloadTorrent_sig(file,opt,infoName,infoHash);
              this->close();
        }
        delete _dialog;
//    }
}

void newTaskWidget::onCancelBtnClicked()
{

}

void newTaskWidget::onSureBtnClicked()
{

}
