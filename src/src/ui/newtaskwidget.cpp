#include "newtaskwidget.h"

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

    QIcon tryIcon=QIcon(":/icons/images/icon/downloader2.svg")  ;
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
    QIcon _tryIcon=QIcon(":/tab/folder_light.svg");
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

                BtInfoDialog *dialog = new BtInfoDialog(fileName,m_savePath);//torrent文件路径
                int ret = dialog->exec();
                if(ret == QDialog::Accepted) {
                    QMap<QString,QVariant> opt;
                    opt.insert("dir",dialog->getSaveto());
                    opt.insert("select-file",dialog->getSelected());
                    QString infoName=dialog->getName();
                   // QString infoHash=dialog->getBtInfo().infoHash;
                   // emit DownloadTorrent_sig(fileName,opt,infoName,infoHash);
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
