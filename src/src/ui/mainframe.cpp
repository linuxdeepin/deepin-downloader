/**
* @file mainframe.cpp
* @brief 主界面类
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:40
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "mainframe.h"
#include <QLayout>
#include <QAction>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QHeaderView>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QClipboard>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QDebug>
#include <DMenu>
#include <DLabel>
#include <DTitlebar>
#include "aria2rpcinterface.h"
#include "aria2const.h"
#include "tableView.h"
#include "topButton.h"
#include "aria2rpcinterface.h"
#include "newtaskwidget.h"
#include "settingswidget.h"
#include "tableModel.h"
#include "log.h"
#include "global.h"
#include "clipboardtimer.h"
#include "messagebox.h"
#include "deleteitemthread.h"
#include "../database/dbinstance.h"

using namespace Global;

MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{
    init();
    initAria2();
    initConnection();
    initTabledata();
}

void MainFrame::init()
{

    m_pSettings = new Settings;


    m_iCurrentListviewRow = 0;

    // 添加设置界面
    DMenu *pSettingsMenu = new DMenu;
    m_pSettingAction = new QAction(tr("Settings"), this);
    pSettingsMenu->addAction(m_pSettingAction);
    titlebar()->setMenu(pSettingsMenu);

    setMinimumSize(838, 636);
    this->setTitlebarShadowEnabled(true);

    m_pToolBar = new TopButton(this);
    titlebar()->setCustomWidget(m_pToolBar, false);

    QPalette p;
    p.setColor(QPalette::Background, QColor(255, 255, 255));

    QFrame *pMainWidget = new QFrame;
    pMainWidget->setFrameShape(QFrame::NoFrame);
    QHBoxLayout *pMainHLayout = new QHBoxLayout(pMainWidget);
    this->setCentralWidget(pMainWidget);
    pMainHLayout->setContentsMargins(0, 0, 0, 0);
    pMainHLayout->setSpacing(0);


    m_pDownLoadingTableView = new TableView(downloading, m_pToolBar);
    m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
    m_pDownLoadingTableView->setColumnHidden(4, true);

    m_pRecycleTableView = new TableView(recycle, m_pToolBar);
    m_pRecycleTableView->verticalHeader()->setDefaultSectionSize(30);


    m_pRecycleTableView->setColumnHidden(3, true);
    setAcceptDrops(true);
    m_pLeftWidget = new QWidget;
    m_pLeftWidget->setAutoFillBackground(true);
    QVBoxLayout *pLeftLayout = new QVBoxLayout(m_pLeftWidget);
    m_pLeftWidget->setPalette(p);
    m_pLeftWidget->setFixedWidth(152);
    pLeftLayout->setContentsMargins(10, 0, 10, 0);

    m_pRight_Widget = new QWidget;
    m_pnotaskWidget = new QWidget;
    m_pnotaskWidget->setAutoFillBackground(true);
    m_pnotaskWidget->setPalette(p);

    QVBoxLayout *pNoTask_WidgetLayout = new QVBoxLayout(m_pnotaskWidget);
    pNoTask_WidgetLayout->setContentsMargins(10, 0, 0, 0);
    m_pnotaskLabel = new Dtk::Widget::DLabel();
    m_pnotaskLabel->setForegroundRole(DPalette::PlaceholderText);
    QFont lableFont;
    lableFont.setPointSize(15);
    lableFont.setBold(QFont::DemiBold);
    lableFont.setFamily("T5");
    QPalette font_p;
    QColor   color = QColor(192, 198, 212, 76);
    font_p.setColor(QPalette::WindowText, color);
    m_pnotaskLabel->setFont(lableFont);
    m_pnotaskLabel->setText(tr("No download tasks"));
    m_pnotaskLabel->setAlignment(Qt::AlignHCenter);

    pNoTask_WidgetLayout->addWidget(m_pnotaskLabel);
    m_pnotaskTipLabel = new QLabel();
    QFont noTask_tip_Label_font;
    noTask_tip_Label_font.setPointSize(13);
    m_pnotaskTipLabel->setFont(noTask_tip_Label_font);
    QPalette noTask_tip_Label_p;
    noTask_tip_Label_p.setColor(QPalette::WindowText, QColor(65, 77, 104, 70));
    m_pnotaskTipLabel->setText(tr("Click + to create new task"));
    m_pnotaskTipLabel->setAlignment(Qt::AlignHCenter);
    m_pnotaskTipLabel->setPalette(noTask_tip_Label_p);
    pNoTask_WidgetLayout->addWidget(m_pnotaskTipLabel);
    pNoTask_WidgetLayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_pRight_Widget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);
    m_pRightStackwidget = new QStackedWidget(this);

    QPalette p_task_num;
    p_task_num.setBrush(this->backgroundRole(), QBrush(QColor(255, 255, 255, 178)));
    m_ptaskNumWidget = new QWidget;
    m_ptaskNumWidget->setFixedHeight(30);
    m_ptaskNumWidget->setAutoFillBackground(true);
    m_ptaskNumWidget->setPalette(p_task_num);
    QHBoxLayout *Task_Num_WidgetLayout = new QHBoxLayout(m_ptaskNumWidget);
    Task_Num_WidgetLayout->setMargin(0);
    m_ptaskNum = new QLabel(tr("0 task"));
    m_ptaskNum->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_ptaskNum->setPalette(noTask_tip_Label_p);
    Task_Num_WidgetLayout->addWidget(m_ptaskNum);
    m_pRightStackwidget->addWidget(m_pDownLoadingTableView);
    m_pRightStackwidget->addWidget(m_pRecycleTableView);
    pRightLayout->addWidget(m_pRightStackwidget);
    pRightLayout->addWidget(m_pnotaskWidget);
    pRightLayout->addWidget(m_ptaskNumWidget);
    pRightLayout->setSpacing(0);

    pMainHLayout->addWidget(m_pLeftWidget);
    pMainHLayout->addWidget(m_pRight_Widget);

    m_pleftList = new DListView;
    m_pleftList->setItemSpacing(0);
    m_pleftList->setItemSize(QSize(132, 40));
    m_pleftList->setItemMargins(QMargins(10, 2, 5, 2));
    m_pleftList->setIconSize(QSize(14, 14));
    QFont font;
    font.setFamily("Source Han Sans");
    font.setPixelSize(14);
    m_pleftList->setFont(font);
    QStandardItemModel* pLeftList_model = new QStandardItemModel(this);

    m_pDownloading_item = new QStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("Downloading"));
    //QFont f = m_pDownloading_item->font();
    m_pDownloadFinish_item = new QStandardItem(QIcon::fromTheme("dcc_print_done"), tr("Completed"));
    m_pRecycle_item = new QStandardItem(QIcon::fromTheme("dcc_list_delete"), tr("Trash"));
    m_pDownloading_item->setBackground(QColor(255, 255, 255));
    m_pDownloadFinish_item->setBackground(QColor(255, 255, 255));
    m_pRecycle_item->setBackground(QColor(255, 255, 255));
    m_pDownloading_item->setEditable(false);
    m_pDownloadFinish_item->setEditable(false);
    m_pRecycle_item->setEditable(false);
    pLeftList_model->appendRow(m_pDownloading_item);
    pLeftList_model->appendRow(m_pDownloadFinish_item);
    pLeftList_model->appendRow(m_pRecycle_item);
    m_pleftList->setModel(pLeftList_model);
    pLeftLayout->addWidget(m_pleftList, 0);
    //updatetimer = new QTimer(this);
    m_pleftList->setCurrentIndex(pLeftList_model->index(0, 0));
    m_pDownLoadingTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    //recycle_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    m_pRightStackwidget->setCurrentIndex(0);

    m_pClipboard = new ClipboardTimer;       //获取当前剪切板

    m_pUpdatetimer = new QTimer(this);
    initTray();
}

void MainFrame::initTray()
{
    QIcon tryIcon = QIcon(":/icons/icon/downloader5.svg");
        m_pSystemTray = new QSystemTrayIcon(this);
        m_pSystemTray->setIcon(tryIcon);
        m_pSystemTray->setToolTip(tr("Download Manager"));

        QAction *pShowMainAct = new QAction(tr("Show main window"), this);
        QAction *pNewDownloadAct = new QAction(tr("New task"), this);
        QAction *pStartAllAct = new QAction(tr("Continue all tasks"), this);
        QAction *pPauseAllAct = new QAction(tr("Pause all tasks"), this);
        QAction *pQuitAct = new QAction(tr("Exit"), this);

        QMenu* pTrayMenu = new QMenu(this);
        pTrayMenu->addAction(pShowMainAct);
        pTrayMenu->addAction(pNewDownloadAct);
        pTrayMenu->addAction(pStartAllAct);
        pTrayMenu->addAction(pPauseAllAct);
        pTrayMenu->addAction(pQuitAct);

        //连接信号与槽
        connect(pShowMainAct, &QAction::triggered, [=](){this->show();});
        connect(pNewDownloadAct, &QAction::triggered, [=](){createNewTask("");});
        connect(pQuitAct, &QAction::triggered, this, &MainFrame::onTrayQuitClick);
        connect(m_pSystemTray, &QSystemTrayIcon::activated, this, &MainFrame::onActivated);
        m_pSystemTray->setContextMenu(pTrayMenu);
        m_pSystemTray->show();
}

void MainFrame::initConnection()
{
    connect(m_pDownLoadingTableView, &TableView::signalHeaderStatechanged, this, &MainFrame::getHeaderStatechanged);
    connect(m_pDownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(m_pRecycleTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(m_pRecycleTableView, &TableView::signalHeaderStatechanged,  this, &MainFrame::getHeaderStatechanged);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::signalClearHeaderCheck);
    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::signalClearHeaderCheck);
    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::signalClearHeaderCheck);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::signalClearHeaderCheck);
    connect(m_pDownLoadingTableView->getTableModel(), &TableModel::signalCheckChange, this,&MainFrame::slotCheckChange);
    connect(m_pRecycleTableView->getTableModel(), &TableModel::signalCheckChange, this,&MainFrame::slotCheckChange);
    connect(m_pDownLoadingTableView, &TableView::pressed, this, &MainFrame::slotTableItemSelected);
    connect(m_pRecycleTableView, &TableView::pressed, this, &MainFrame::slotTableItemSelected);


    connect(m_pSettingAction,&QAction::triggered, this, &MainFrame::onSettingsMenuClicked);
    connect(m_pClipboard, &ClipboardTimer::sendClipboardText,this,&MainFrame::onClipboardDataChanged);
    connect(m_pleftList, &DListView::clicked, this, &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::getPalettetypechanged);
    connect(m_pUpdatetimer, &QTimer::timeout, this, &MainFrame::updateMainUI);

    connect(m_pToolBar, &TopButton::newDownloadBtnClicked, this, &MainFrame::onNewBtnClicked);
    connect(this, &MainFrame::tableChanged, m_pToolBar, &TopButton::getTableChanged);
    connect(m_pToolBar, &TopButton::getSearchEditTextChange, this, &MainFrame::slotSearchEditTextChanged);
    connect(m_pToolBar, &TopButton::startDownloadBtnClicked, this, &MainFrame::onStartDownloadBtnClicked);
    connect(m_pToolBar, &TopButton::pauseDownloadBtnClicked, this, &MainFrame::onPauseDownloadBtnClicked);
    connect(m_pToolBar, &TopButton::deleteDownloadBtnClicked, this, &MainFrame::onDeleteDownloadBtnClicked);
    //connect(m_pToolBar, &TopButton::getSearchEditFocus, this, &MainFrame::onSearchEditFocusChanged);
}

void MainFrame::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::Trigger) {
        if(this->isHidden()) {
            // 恢复窗口显示
            this->show();
            this->setWindowState(Qt::WindowActive);
            this->activateWindow();
            //if(this->minimumAct) {
                this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            //}
        } else {
            this->hide();
        }
    }
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    if(Settings::getInstance()->getIsShowTip()){
        MessageBox *msg = new MessageBox();
        connect(msg, &MessageBox::signalCloseConfirm, this, &MainFrame::onMessageBoxConfirmClick);
        msg->setExit();
        msg->exec();
    } else {
        onMessageBoxConfirmClick();
    }
    event->ignore();
    this->hide();
}

void MainFrame::createNewTask(QString url)
{
    static newTaskWidget *pNewTaskWidget = new newTaskWidget();
    connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewDowloadUrl);
    connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewdowloadSlot);
    pNewTaskWidget->exec();
}
void MainFrame::onTrayQuitClick()
{
    m_pDownLoadingTableView->saveDataBeforeClose();
    m_pRecycleTableView->saveDataBeforeClose();
    qApp->quit();
}

void MainFrame::onMessageBoxConfirmClick()
{
    if(Settings::getInstance()->getCloseMainWindowSelected()){
        onTrayQuitClick();
    }
}

MainFrame::~MainFrame()
{

}


void MainFrame::initAria2()
{
    Aria2RPCInterface::Instance()->init();//启动aria2c
    connect(Aria2RPCInterface::Instance(), SIGNAL(signalRPCSuccess(QString, QJsonObject)), this, SLOT(slotRpcSuccess(QString, QJsonObject)));
    connect(Aria2RPCInterface::Instance(), SIGNAL(signalRPCError(QString, QString, int)), this, SLOT(slotRpcError(QString, QString, int)));

    qDebug() << "MainFrame initAria2 Finished";
}

void MainFrame::initTabledata()
{
    QList<S_Task> list;
    S_Task tbTask;
    int     task_num = 0;

    DBInstance::getAllTask(list);
    for(int i = 0; i < list.length(); i++) {
        DataItem *data = new DataItem;
        data->gid = list.at(i).m_gid;
        data->url = list.at(i).m_url;
        data->time = "0";
        data->speed = "0kb/s";
        data->taskId = list.at(i).m_taskId;
        S_Task_Status taskStatus;
        DBInstance::getTaskStatusById(data->taskId, taskStatus);
        if(taskStatus.m_taskId != "") {
            data->percent = 0;
            data->fileName = list.at(i).m_downloadFilename;
            data->savePath = list.at(i).m_downloadPath;
            data->Ischecked = 0;
            data->totalLength = taskStatus.m_totalLength;
            data->completedLength = taskStatus.m_compeletedLength;
            if(taskStatus.m_downloadStatus == Global::Status::Active) {
                data->status = Global::Status::Lastincomplete;
            } else {
                data->status = taskStatus.m_downloadStatus;
            }
            data->total = taskStatus.m_totalFromSource;
            if(data->status == Global::Status::Lastincomplete) {
                ++task_num;
            }
            if(data->status != Global::Status::Removed) {
                if(data->status == Global::Status::Complete) {
                    data->time = taskStatus.m_modifyTime.toString("yyyy-MM-dd hh:mm:ss");
                }
                if(data->status == Global::Status::Lastincomplete) {
                    m_pnotaskWidget->hide();
                    QVariant autostart_unfinished_task_switchbutton = m_pSettings->getAutostartUnfinishedTaskState();
                    m_pDownLoadingTableView->getTableModel()->append(data);
                    if(autostart_unfinished_task_switchbutton.toBool()) {
                        QString savePath = getDownloadSavepathFromConfig();
                        QMap<QString, QVariant> opt;
                        opt.insert("dir", savePath);
                        S_Url_Info *url_info = new S_Url_Info;
                        S_Url_Info getUrlInfo;
                        DBInstance::getUrlById(data->taskId, getUrlInfo);
                        if(getUrlInfo.m_taskId != "") {
                            if(getUrlInfo.m_downloadType == "torrent") {
                                QString select_num = getUrlInfo.m_selectedNum;
                                QMap<QString, QVariant> opt;
                                opt.insert("dir",         savePath);
                                opt.insert("select-file", select_num);
                                if(!QFile(getUrlInfo.m_seedFile).exists()) {
                                    showWarningMsgbox(tr("seed file not exists or broken;"));
                                    qDebug() << "seed file not exists or broken;";
                                } else {
                                    Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
                                    if(m_pUpdatetimer->isActive() == false) {
                                        m_pUpdatetimer->start(2 * 1000);
                                    }
                                }
                            }
                        } else {
                            //deal_download_upload_limit_period();
                            Aria2RPCInterface::Instance()->addUri(data->url, opt, data->taskId);
                            if(m_pUpdatetimer->isActive() == false) {
                                m_pUpdatetimer->start(2 * 1000);
                            }
                        }
                    }
                } else {
                    m_pDownLoadingTableView->getTableModel()->append(data);
                }
            } else {
                DelDataItem *deldata = new DelDataItem;
                deldata->taskId = data->taskId;
                deldata->gid = data->gid;
                deldata->url = data->url;
                deldata->status = data->status;
                deldata->fileName = data->fileName;
                deldata->savePath = data->savePath;
                deldata->deleteTime = taskStatus.m_modifyTime.toString("yyyy-MM-dd hh:mm:ss");
                deldata->totalLength = data->totalLength;
                deldata->completedLength = data->completedLength;
                deldata->finishTime = taskStatus.m_finishTime.toString("yyyy-MM-dd hh:mm:ss");
                m_pRecycleTableView->getTableModel()->append(deldata);
            }
        }
    }
    m_pDownLoadingTableView->refreshTableView(0);

    setTaskNum(m_iCurrentListviewRow);
}

void MainFrame::setTaskNum(int num)
{
    QList<DataItem *> data_list = m_pDownLoadingTableView->getTableModel()->dataList();
    QList<DataItem *> render_list = m_pDownLoadingTableView->getTableModel()->renderList();

    QList<DelDataItem *> m_recycleList = m_pRecycleTableView->getTableModel()->recyleList();
    int activeCount = 0;
    int finishCount = 0;
    int recycleCount = 0;
    QString active_num;

    if(num == 0) {
        int i = 0;
        for(const auto *item : render_list) {
            if((item->status == Global::Status::Active) || (item->status == Global::Status::Paused) ||
               (item->status == Global::Status::Lastincomplete) || (item->status == Global::Status::Error) ||
               (item->status == Global::Waiting)) {
                if(!m_pDownLoadingTableView->isRowHidden(i)) {
                    ++activeCount;
                }
            }
            i++;
        }
        active_num = QString::number(activeCount) + tr(" item tasks");
        if(activeCount == 0) {
            m_pnotaskWidget->show();
        } else {
            m_pnotaskWidget->hide();
        }
    } else if(num == 1) {
        int j = 0;
        for(const auto *item : render_list) {
            if(item->status == Global::Status::Complete) {
                if(!m_pDownLoadingTableView->isRowHidden(j)) {
                    ++finishCount;
                }
            }
            j++;
        }
        active_num = QString::number(finishCount) + tr(" files");
        if(finishCount == 0) {
            m_pnotaskWidget->show();
        } else {
            m_pnotaskWidget->hide();
        }
    } else {
        int k = 0;
        for(const auto *item : m_recycleList) {
            if(!m_pRecycleTableView->isRowHidden(k)) {
                ++recycleCount;
            }
            k++;
        }
        active_num = QString::number(recycleCount) + tr(" files");
        if(recycleCount == 0) {
            m_pnotaskWidget->show();
        } else {
            m_pnotaskWidget->hide();
        }
    }
    m_ptaskNum->setText(active_num);
}

void MainFrame::onSettingsMenuClicked()
{
    DSettingsDialog *pSettingsDialog = new DSettingsDialog(this);
    pSettingsDialog->widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);
//    pSettingsDialog->widgetFactory()->registerWidget("downloadtraysetting", Settings::createDownloadTraySettingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloaddiskcachesetting", Settings::createDownloadDiskCacheSettiingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloadspeedlimitsetting", Settings::createDownloadSpeedLimitSettiingHandle);
    pSettingsDialog->updateSettings( "Settings",m_pSettings->m_pSettings );
    pSettingsDialog->exec();
    delete pSettingsDialog;
    m_pSettings->m_pSettings->sync();

}

void MainFrame::onClipboardDataChanged(QString url)
{
    createNewTask(url);
}

void MainFrame::onListClicked(const QModelIndex &index)
{
    m_iCurrentListviewRow = index.row();
    QString DownloadTask_Lable_Text;
    if((index.row() == 0) || (index.row() == 1)) {
        m_pRightStackwidget->setCurrentIndex(0);
        m_pDownLoadingTableView->refreshTableView(index.row());
        m_pDownLoadingTableView->horizontalHeader()->reset();
        bool switched = true;
        m_pDownLoadingTableView->reset(switched);
        if(index.row() == 1) {
            //connect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(30);
            m_pnotaskWidget->show();
            m_pnotaskLabel->setText(tr("No finished tasks"));
            m_pnotaskTipLabel->hide();
        } else {
            //disconnect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
            m_pnotaskLabel->setText(tr("No download tasks"));
            m_pnotaskWidget->show();
            m_pnotaskTipLabel->show();
        }
    } else {
        m_pRightStackwidget->setCurrentIndex(1);
        m_pnotaskWidget->show();
        m_pnotaskLabel->setText(tr("No deleted tasks"));
        m_pnotaskTipLabel->hide();
    }
    clearTableItemCheckStatus();

    emit switchTableSignal();

    setTaskNum(m_iCurrentListviewRow);

    // search begin
    slotSearchEditTextChanged(this->m_pToolBar->getSearchText());
    emit tableChanged(index.row());

}

void MainFrame::getPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    m_pleftList->setPalette(DGuiApplicationHelper::instance()->applicationPalette());

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        QPalette deeptheme_palette;
        deeptheme_palette.setBrush(QPalette::Background,
                                   DGuiApplicationHelper::instance()->applicationPalette().base());
        m_pLeftWidget->setPalette(deeptheme_palette);
        m_pDownloading_item->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        m_pDownloadFinish_item->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        m_pRecycle_item->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        QPalette label_palette;
        label_palette.setBrush(QPalette::Text,
                               DGuiApplicationHelper::instance()->applicationPalette().placeholderText());

        m_pnotaskTipLabel->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        QPalette tableview_palette;

        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());

        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_pnotaskWidget->setPalette(tableview_palette);
        m_ptaskNumWidget->setPalette(tableview_palette);
        m_pDownloading_item->setIcon(QIcon::fromTheme("dcc_list_downloading_dark"));
        m_pDownloadFinish_item->setIcon(QIcon::fromTheme("dcc_print_done_dark"));
        m_pRecycle_item->setIcon(QIcon::fromTheme("dcc_list_delete_dark"));
        QPalette noTask_tip_Label_p;
        noTask_tip_Label_p.setBrush(QPalette::WindowText,
                                    DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_ptaskNum->setPalette(noTask_tip_Label_p);
    }
    else if(DGuiApplicationHelper::instance()->themeType()==1) {
        QPalette p;
        p.setColor(QPalette::Background, QColor(255, 255, 255));
        QPalette tableview_palette;
        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());
        m_pLeftWidget->setPalette(p);
        m_pnotaskWidget->setPalette(tableview_palette);
        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_ptaskNumWidget->setPalette(tableview_palette);

        m_pDownloading_item->setIcon(QIcon::fromTheme("dcc_list_downloading"));
        m_pDownloadFinish_item->setIcon(QIcon::fromTheme("dcc_print_done"));
        m_pRecycle_item->setIcon(QIcon::fromTheme("dcc_list_delete"));
        m_pDownloading_item->setBackground(QColor(255, 255, 255));
        m_pDownloadFinish_item->setBackground(QColor(255, 255, 255));
        m_pRecycle_item->setBackground(QColor(255, 255, 255));

        QPalette font_p;
        QColor   color = QColor(192, 198, 212, 76);
        font_p.setColor(QPalette::WindowText, color);

        // noTask_label->setPalette(font_p);
        QPalette noTask_tip_Label_p;
        noTask_tip_Label_p.setColor(QPalette::WindowText, QColor(65, 77, 104, 70));
        m_ptaskNum->setPalette(noTask_tip_Label_p);
    }
}

void MainFrame::getHeaderStatechanged(bool isChecked)
{
    // ToolBar禁用按钮联动：表头全选复选框状体变化 begin
    int cnt = (m_iCurrentListviewRow == 2 ? m_pRecycleTableView->getTableModel()->rowCount(QModelIndex())
               : m_pDownLoadingTableView->getTableModel()->rowCount(QModelIndex()));

    if(cnt > 0) {
        if(m_iCurrentListviewRow == 0) {
            m_pToolBar->enableStartBtn(isChecked);
            m_pToolBar->enablePauseBtn(isChecked);
            m_pToolBar->enableDeleteBtn(isChecked);
        } else {
            m_pToolBar->enableStartBtn(false);
            m_pToolBar->enablePauseBtn(false);
            m_pToolBar->enableDeleteBtn(isChecked);
        }
    } else {
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }

    // end
    if((m_iCurrentListviewRow == 0) || (m_iCurrentListviewRow == 1)) {
        QList<DataItem *> render_list = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int j = 0; j < render_list.size(); j++) {
            DataItem *data = render_list.at(j);
            if(!isChecked) {
                if(m_iCurrentListviewRow == 0) {
                    m_iDownloadingHeaderCheckStatus = 0;
                }
                if(m_iCurrentListviewRow == 1) {
                    m_iFinishHeaderCheckStatus = 0;
                }
                data->Ischecked = false;
            } else {
                if(m_iCurrentListviewRow == 0) {
                    m_iDownloadingHeaderCheckStatus = 2;
                }
                if(m_iCurrentListviewRow == 1) {
                    m_iFinishHeaderCheckStatus = 2;
                }
                data->Ischecked = true;
            }

            m_pDownLoadingTableView->reset();
        }
    } else {
        QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
        for(int j = 0; j < recycle_list.size(); j++) {
            DelDataItem *data = recycle_list.at(j);
            if(!isChecked) {
                data->Ischecked = false;
            } else {
                data->Ischecked = true;
            }

            m_pRecycleTableView->reset();
        }
    }
}

void MainFrame::getNewDowloadUrl(QString url, QString savePath)
{
    QStringList urlList = url.split("\n");
    urlList = urlList.toSet().toList();   //url去重
    bool isExitsUrl = false;
    //判断url是否在数据中已存在
    for (int i = 0;i < urlList.size(); i++)
    {
        DBInstance::isExistUrl(urlList[i],isExitsUrl);
        if(isExitsUrl)
        {
            urlList.removeAt(i);
            --i;
        }
    }
    if(urlList.isEmpty())
    {
        qDebug()<<"url is NUll";
        return;
    }
    //将url加入数据库和aria
    S_Task _task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    for (int i = 0; i < urlList.size(); i++)
    {
        _task = getUrlToName(urlList[i], savePath);
        DBInstance::addTask(_task);
        Aria2RPCInterface::Instance()->addNewUri(_task.m_url,savePath,_task.m_taskId);
    }

    m_pnotaskWidget->hide();
    //定时器打开
}

S_Task MainFrame::getUrlToName(QString url, QString savePath)
{
    //获取url文件名
    QString fileName;
    if(url.startsWith("magnet") && url.contains("&"))
    {
        fileName = url.split("&")[0];
        if(fileName.contains("btih:"))
        {
            fileName=fileName.split("btih:")[1]+".torrent";
        }
        else
        {
            fileName=url.right(40);
        }
    }
    else
    {
         fileName=QString(url).right(url.length() - url.lastIndexOf('/') - 1);
    }

    //对url进行转码
    if(!fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
    {
        const QByteArray _byte=fileName.toLatin1();
        QString decode=QUrl::fromPercentEncoding(_byte);
        if(decode.contains("?"))
        {
            decode=decode.split("?")[0];
        }
        fileName=decode;
    }

    S_Task task;
    task.m_taskId = QUuid::createUuid().toString();
    task.m_gid = "";
    task.m_gidIndex = 0;
    task.m_url = url;
    task.m_downloadPath = savePath + "/" +fileName;
    task.m_downloadFilename = fileName;
    task.m_createTime = QDateTime::currentDateTime();
    return task;
}

void MainFrame::slotContextMenu(QPoint pos)
{
    int chkedCnt = 0;

    if(m_iCurrentListviewRow == 2) {
        QList<DelDataItem *> recyleList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recyleList.size(); i++) {
            if(recyleList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
    } else {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); i++) {
            if(selectList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
    }
    if(chkedCnt == 0) {
        return;
    }

    int activeCount = 0;
    int pauseCount = 0;
    int renamCount = 0;
    int errorCount = 0;
    QMenu *delmenlist = new QMenu(this);
    if(m_iCurrentListviewRow == 0) {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked == 1) {
                DataItem *data = selectList.at(i);
                if(data->status == Global::Status::Active) {
                    ++activeCount;
                }
                if((data->status == Global::Status::Paused) || (data->status == Global::Status::Lastincomplete)) {
                    ++pauseCount;
                }
                if(data->status == Global::Status::Error) {
                    ++errorCount;
                }
            }
        }
        if(pauseCount > 0) {
            QAction *pActionStart = new QAction();
            pActionStart->setText(tr("Continue"));
            delmenlist->addAction(pActionStart);
            //connect(pActionStart, &QAction::triggered, this, &MainFrame::onStartDownloadBtnClicked);
        }
        if(activeCount > 0) {
            QAction *pActionPause = new QAction();
            pActionPause->setText(tr("Pause"));
            delmenlist->addAction(pActionPause);
            //connect(pActionPause, &QAction::triggered, this, &MainFrame::onPauseDownloadBtnClicked);
        }
        if(errorCount > 0) {
            QAction *pActionredownload = new QAction();
            pActionredownload->setText(tr("Download again"));
            delmenlist->addAction(pActionredownload);
            //connect(pActionredownload, &QAction::triggered, this, &MainFrame::redownload_Action);
        }
        delmenlist->addSeparator();
    }
    if(m_iCurrentListviewRow == 2) {
        QAction *returned_to_origin = new QAction();
        returned_to_origin->setText(tr("Restore"));
        delmenlist->addAction(returned_to_origin);
        //connect(returned_to_origin, &QAction::triggered, this, &MainFrame::returnTo_origin_Action);
    }
    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 2)) {
        QAction *pActionopenFile = new QAction();
        pActionopenFile->setText(tr("Open"));
        delmenlist->addAction(pActionopenFile);
        //connect(pActionopenFile, &QAction::triggered, this, &MainFrame::open_file_Action);
    }
    QAction *pActionopenFoler = new QAction();
    pActionopenFoler->setText(tr("Open folder"));
    delmenlist->addAction(pActionopenFoler);
    //connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::open_Folder_Action);


    //    if(m_iCcurrentListviewRow==0||m_iCcurrentListviewRow==1)
    //    {
    //        QAction *pAction_move_to_directory= new QAction();
    //        pAction_move_to_directory->setText(tr("move to directory"));
    //        delmenlist->addAction(pAction_move_to_directory);
    //        connect(pAction_move_to_directory,&QAction::triggered,this,
    // &MainFrame::moveTo_directory_Action);

    //    }
    if(m_iCurrentListviewRow == 0) {
        delmenlist->addSeparator();
    }
    if(m_iCurrentListviewRow == 1) {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked == 1) {
                DataItem *data = selectList.at(i);
                if(data->status == Global::Status::Complete) {
                    ++renamCount;
                }
            }
        }

        if(renamCount == 1) {
            QAction *pAction_rename = new QAction();
            pAction_rename->setText(tr("Rename"));
            delmenlist->addAction(pAction_rename);
            delmenlist->addSeparator();
            //connect(pAction_rename, &QAction::triggered, this, &MainFrame::rename_Action);
        }
    }
    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 2)) {
        QAction *pActionredownload = new QAction();
        pActionredownload->setText(tr("Download again"));
        delmenlist->addAction(pActionredownload);
        //connect(pActionredownload, &QAction::triggered, this, &MainFrame::redownload_Action);
    }
    QAction *pAction_copy_download_url = new QAction();
    pAction_copy_download_url->setText(tr("Copy download link"));
    delmenlist->addAction(pAction_copy_download_url);
    delmenlist->addSeparator();
    //connect(pAction_copy_download_url, &QAction::triggered, this, &MainFrame::copy_download_url_Action);
    if((m_iCurrentListviewRow == 0) || (m_iCurrentListviewRow == 1)) {
        QAction *pActiondel_downloading = new QAction();
        pActiondel_downloading->setText(tr("Delete"));
        delmenlist->addAction(pActiondel_downloading);
        //connect(pActiondel_downloading, &QAction::triggered, this, &MainFrame::del_downloading_Action);
    }
    QAction *pAction_delete_permanently = new QAction();
    pAction_delete_permanently->setText(tr("Permanently delete"));
    delmenlist->addAction(pAction_delete_permanently);
    //connect(pAction_delete_permanently, &QAction::triggered, this, &MainFrame::delete_permanently_Action);
    if(m_iCurrentListviewRow == 2) {
        QAction *pAction_clear_recycle = new QAction();
        pAction_clear_recycle->setText(tr("Empty"));
        delmenlist->addAction(pAction_clear_recycle);
        //connect(pAction_clear_recycle, &QAction::triggered, this, &MainFrame::clear_recycle_Action);
    }

    delmenlist->exec(QCursor::pos());
    delete  delmenlist;
}

void MainFrame::slotCheckChange(bool checked, int flag)
{
    // ToolBar禁用按钮联动：列表内复选框状态变化 begin
    int chkedCnt = 0;

    if(m_iCurrentListviewRow == 2) {
        QList<DelDataItem *> recyleList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recyleList.size(); i++) {
            if(recyleList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
    } else {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); i++) {
            if(selectList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
    }

    if(chkedCnt > 0) {
        if(m_iCurrentListviewRow == 0) {
            this->m_pToolBar->enableStartBtn(true);
            this->m_pToolBar->enablePauseBtn(true);
            this->m_pToolBar->enableDeleteBtn(true);
        } else if(m_iCurrentListviewRow == 1) {
            this->m_pToolBar->enableStartBtn(false);
            this->m_pToolBar->enablePauseBtn(false);
            this->m_pToolBar->enableDeleteBtn(true);
        } else if(m_iCurrentListviewRow == 2) {
            this->m_pToolBar->enableStartBtn(false);
            this->m_pToolBar->enablePauseBtn(false);
            this->m_pToolBar->enableDeleteBtn(true);
        }
    } else {
        this->m_pToolBar->enableStartBtn(false);
        this->m_pToolBar->enablePauseBtn(false);
        this->m_pToolBar->enableDeleteBtn(false);
    }

    // end
}

void MainFrame::clearTableItemCheckStatus()
{
    if((m_iCurrentListviewRow == 0) || (m_iCurrentListviewRow == 1)) {
        QList<DataItem *> render_list = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int j = 0; j < render_list.size(); j++) {
            DataItem *data = render_list.at(j);
            data->Ischecked = false;
            m_pDownLoadingTableView->reset();
        }
    } else {
        QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
        for(int j = 0; j < recycle_list.size(); j++) {
            DelDataItem *data = recycle_list.at(j);
            data->Ischecked = false;
            m_pRecycleTableView->reset();
        }
    }
}

void MainFrame::slotSearchEditTextChanged(QString text)
{
    m_searchContent = text;
    m_pDownLoadingTableView->searchEditTextChanged(text);
    m_pRecycleTableView->searchEditTextChanged(text);
    setTaskNum(m_iCurrentListviewRow);
}

void MainFrame::getNewDownloadTorrent(QString btPath,QMap<QString,QVariant> opt,QString infoName, QString infoHash)
{
    QString selectedNum=opt.value("select-file").toString();
    if(selectedNum.isNull())
    {
        qDebug()<<"select is null";
        return;
    }
    //数据库是否已存在相同的地址
    QList<S_Url_Info> urlList;
    DBInstance::getAllUrl(urlList);
    for (int i = 0; i < urlList.size(); i++)
    {
        if((urlList[i].m_infoHash==infoHash) && (urlList[i].m_selectedNum == selectedNum))
        {
            qDebug()<<"has the same download!";
            return;
        }
    }
    //将任务添加如task表中
    S_Task task;
    QString strId = QUuid::createUuid().toString();
    task.m_taskId = strId;
    task.m_gid = "";
    task.m_gidIndex = 0;
    task.m_url = "";
    task.m_downloadPath = "";
    task.m_downloadFilename = infoName;
    task.m_createTime = QDateTime::currentDateTime();
    DBInstance::addTask(task);

    //将任务添加如url_info表中
     S_Url_Info _urlInfo;
     _urlInfo.m_taskId = strId;
     _urlInfo.m_url = "";
     _urlInfo.m_downloadType = "torrent";
     _urlInfo.m_seedFile = btPath;
     _urlInfo.m_selectedNum = selectedNum;
     _urlInfo.m_infoHash = "";
     DBInstance::addUrl(_urlInfo);

     //开始下载
     Aria2RPCInterface::Instance()->addTorrent(btPath, opt, strId);
     //定时器打开
}
void MainFrame::getNewdowloadSlot(QString url, QString savepath)
{
    QMap<QString, QVariant> opt;

    opt.insert("dir", savepath);
    QStringList url_list = url.split("\n");
    int sameUrl = 0;
    QList<QString> repeat_url_list;
    QStringList    addUrlList;
    for(int j = 0; j < url_list.size(); j++) {
        if(addUrlList.size() == 0) {
            addUrlList.append(url_list.at(j));
        } else {
            bool foundSame = false;
            for(int k = 0; k < addUrlList.size(); k++) {
                QString urldecode;
                QString addUrldecode;
                if(!url_list.at(j).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte_download = url_list.at(j).toLatin1();
                    urldecode = QUrl::fromPercentEncoding(byte_download);
                } else {
                    urldecode = url_list.at(j);
                }
                if(!addUrlList.at(k).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte_download = addUrlList.at(k).toLatin1();
                    addUrldecode = QUrl::fromPercentEncoding(byte_download);
                } else {
                    addUrldecode = addUrlList.at(k);
                }
                if(addUrldecode == urldecode) {
                    foundSame = true;
                    continue;
                }
            }
            if(!foundSame) {
                addUrlList.append(url_list.at(j));
            }
        }
    }
    if(addUrlList.size() > 0) {
        for(int i = 0; i < addUrlList.size(); i++) {
            int     sameurlCount = 0;
            QUuid   id = QUuid::createUuid();
            QString strId = id.toString();
            QString download_url = addUrlList.at(i);
            //Tb_Task tb_task;
            QList<DataItem *> data_list = m_pDownLoadingTableView->getTableModel()->dataList();
            QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
            QString downloadUrldecode;
            if(!download_url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                const QByteArray byte_download = download_url.toLatin1();
                downloadUrldecode = QUrl::fromPercentEncoding(byte_download);
            } else {
                downloadUrldecode = download_url;
            }
            for(int i = 0; i < data_list.size(); i++) {
                DataItem *data = data_list.at(i);

                QString urldecode;
                if(!data->url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte = data->url.toLatin1();
                    urldecode = QUrl::fromPercentEncoding(byte);
                } else {
                    urldecode = data->url;
                }

                if(!download_url.isEmpty()) {
                    if((urldecode == downloadUrldecode) || (download_url == urldecode)) {
                        repeat_url_list.append(download_url);
                        ++sameurlCount;
                        sameUrl = sameUrl + sameurlCount;
                    }
                }
            }
            if(sameUrl == 0) {
                for(int i = 0; i < recycle_list.size(); i++) {
                    DelDataItem *data = recycle_list.at(i);
                    QString urldecode;
                    if(!data->url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                        const QByteArray byte = data->url.toLatin1();
                        urldecode = QUrl::fromPercentEncoding(byte);
                    } else {
                        urldecode = data->url;
                    }
                    if((urldecode == downloadUrldecode) || (download_url == urldecode)) {
                        ++sameurlCount;
                        repeat_url_list.append(download_url);
                        sameUrl = sameUrl + sameurlCount;
                    }
                }
            }

            if(sameurlCount == 0) {
                //deal_download_upload_limit_period();
                QString filename;
                if(download_url.startsWith("magnet")) {
                    // filename
                    if(download_url.contains("&")) {
                        filename = download_url.split("&")[0];
                        if(filename.contains("btih:")) {
                            filename = filename.split("btih:")[1] + ".torrent";
                        }
                    } else {
                        filename = download_url.right(40);
                    }
                } else {
                    filename = QString(download_url).right(download_url.length() - download_url.lastIndexOf('/') - 1);
                }

                if(!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray filename_byte = filename.toLatin1();
                    QString filenamedecode = QUrl::fromPercentEncoding(filename_byte);
                    if(filenamedecode.contains("?")) {
                        filenamedecode = filenamedecode.split("?")[0];
                    }
                    filename = filenamedecode;
                }
                S_Task addTask;
                addTask.m_taskId = strId;
                addTask.m_gid = "";
                addTask.m_gidIndex = 0;
                addTask.m_url = download_url;
                addTask.m_downloadPath = savepath + "/" + filename;
                addTask.m_downloadFilename = filename;
                addTask.m_createTime = QDateTime::currentDateTime();
                DBInstance::addTask(addTask);
                m_pnotaskWidget->hide();
                if(m_pUpdatetimer->isActive() == false) {
                    m_pUpdatetimer->start(2 * 1000);
                }
            }
        }
        if(sameUrl > 0) {
            QString warning_msg = tr("has ") + QString::number(sameUrl) + tr(" the same download");
            showWarningMsgbox(warning_msg, sameUrl, repeat_url_list);
        }
    }
}


void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
    MessageBox *msg = new MessageBox();

    msg->setWarings(title, tr("sure"), "", sameUrlCount, sameUrlList);
    msg->exec();
}

void MainFrame::showDeleteMsgbox(bool permanently)
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::DeleteDownload_sig, this, &MainFrame::getDeleteConfirmSlot);
    msg->setDelete(permanently);
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }
}

void MainFrame::slotAria2Remove(QString gId, QString id)
{
    Aria2RPCInterface::Instance()->remove(gId, id);
}

void MainFrame::getDeleteConfirmSlot(bool ischecked, bool permanent)
{
    QString gid;
    QString aria_temp_file;
    QString save_path;
    QString task_id;
    bool    ifDeleteLocal = permanent || ischecked;

    if(m_pUpdatetimer->isActive()) {
        m_pUpdatetimer->stop();
    }

    if(m_iCurrentListviewRow == 2) {
        QList<DelDataItem> threadRecycleDeleteList;
        for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
            DelDataItem tempdata;
            tempdata.status = m_pRecycleDeleteList.at(i)->status;
            tempdata.Ischecked = m_pRecycleDeleteList.at(i)->Ischecked;
            tempdata.taskId = m_pRecycleDeleteList.at(i)->taskId;
            tempdata.fileName = m_pRecycleDeleteList.at(i)->fileName;
            tempdata.completedLength =  m_pRecycleDeleteList.at(i)->completedLength;
            tempdata.savePath = m_pRecycleDeleteList.at(i)->savePath;
            tempdata.gid = m_pRecycleDeleteList.at(i)->gid;
            tempdata.url = m_pRecycleDeleteList.at(i)->url;
            tempdata.totalLength = m_pRecycleDeleteList.at(i)->totalLength;
            tempdata.deleteTime = m_pRecycleDeleteList.at(i)->deleteTime;
            tempdata.finishTime = m_pRecycleDeleteList.at(i)->finishTime;
            threadRecycleDeleteList.append(tempdata);
        }

        DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadRecycleDeleteList,
                                                                  m_pRecycleTableView,
                                                                  Aria2RPCInterface::Instance(),
                                                                  ifDeleteLocal,
                                                                  "recycle_delete");
        connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, this, &MainFrame::slotAria2Remove);
        pDeleteItemThread->start();

        for(int i = 0; i < m_pRecycleDeleteList.size(); i++) {
            DelDataItem *data = new DelDataItem;
            data = m_pRecycleDeleteList.at(i);
            task_id = data->taskId;

            S_Task_Status tb_download_status;
            DBInstance::delTask(task_id);

            m_pRecycleTableView->getTableModel()->removeItem(data);
        }
        setTaskNum(m_iCurrentListviewRow);
    } else {
        DataItem *data = new DataItem;

        QList<DataItem> thread_delete_list;


        for(int i = 0; i < m_pDeleteList.size(); i++) {
            DataItem tempdata;

            tempdata.status = m_pDeleteList.at(i)->status;
            tempdata.percent = m_pDeleteList.at(i)->percent;
            tempdata.total = m_pDeleteList.at(i)->total;
            tempdata.Ischecked = m_pDeleteList.at(i)->Ischecked;
            tempdata.taskId = m_pDeleteList.at(i)->taskId;
            tempdata.fileName = m_pDeleteList.at(i)->fileName;
            tempdata.completedLength =  m_pDeleteList.at(i)->completedLength;
            tempdata.totalLength =  m_pDeleteList.at(i)->totalLength;
            tempdata.savePath = m_pDeleteList.at(i)->savePath;
            tempdata.speed = m_pDeleteList.at(i)->speed;
            tempdata.gid = m_pDeleteList.at(i)->gid;
            tempdata.url = m_pDeleteList.at(i)->url;
            tempdata.time = m_pDeleteList.at(i)->time;
            tempdata.createTime = m_pDeleteList.at(i)->createTime;

            thread_delete_list.append(tempdata);
        }

        DeleteItemThread *pDeleteItemThread = new DeleteItemThread(thread_delete_list,
                                                                  m_pDownLoadingTableView,
                                                                  Aria2RPCInterface::Instance(),
                                                                  ifDeleteLocal,
                                                                  "download_delete");
        connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, this,&MainFrame::slotAria2Remove);
        pDeleteItemThread->start();

        for(int i = 0; i < m_pDeleteList.size(); i++) {
            data = m_pDeleteList.at(i);
            save_path = data->savePath;
            gid = data->gid;
            task_id = data->taskId;
            QDateTime finish_time;
            if(data->status == Complete) {
                finish_time = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
            } else {
                finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
            }

            S_Task_Status  get_status;
            S_Task_Status downloadStatus(data->taskId,
                                                                                   Global::Status::Removed,
                                                                                   QDateTime::currentDateTime(),
                                                                                   data->completedLength,
                                                                                   data->speed,
                                                                                   data->totalLength,
                                                                                   data->percent,
                                                                                   data->total,
                                                                                   finish_time);


            if(permanent || ischecked) {
                S_Task_Status tb_download_status;
                DBInstance::delTask(task_id);
                //Aria2RPCInterface::Instance()->purgeDownloadResult(data->gid);
            }

            if(!permanent && !ischecked) {
                DelDataItem *delData = new DelDataItem;
                delData->taskId = data->taskId;
                delData->gid = data->gid;
                delData->url = data->url;
                delData->fileName = data->fileName;
                delData->savePath = data->savePath;
                delData->Ischecked = false;
                delData->totalLength = data->totalLength;
                delData->completedLength = data->completedLength;
                delData->deleteTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                delData->finishTime = data->time;

                m_pRecycleTableView->getTableModel()->append(delData);
                m_pRecycleTableView->update();

                if(DBInstance::getTaskStatusById(delData->taskId, get_status)) {
                    DBInstance::updateTaskStatusById(downloadStatus);
                } else {
                    DBInstance::addTaskStatus(downloadStatus);
                }
            }

            m_pDownLoadingTableView->getTableModel()->removeItem(data);
        }

        setTaskNum(m_iCurrentListviewRow);
    }
    if(this->m_searchContent != "") {
        slotSearchEditTextChanged(m_searchContent);
    }
    if(m_pUpdatetimer->isActive() == false) {
        m_pUpdatetimer->start(2 * 1000);
    }
}

QString   MainFrame::getDownloadSavepathFromConfig()
{
    QVariant downloadRadioGroup =
        m_pSettings->m_pSettings->getOption("basic.downloadDirectory.downloadDirectoryFileChooser");

    QString path = "";

    QString currentValue = downloadRadioGroup.toString();

    // 无设置信息
    if(currentValue.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    } else {
        // 用户设置默认目录
        if(currentValue.contains("custom;")) {
            path = currentValue.section(QString(';'), 1, 1);
            if(path.isEmpty()) {
                path =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        }
        // 自动修改为上次使用目录
        else {
            QString config_path = QString("%1/%2/%3/last_save_path")
                                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                  .arg(qApp->organizationName())
                                  .arg(qApp->applicationName());
            QFile file;
            file.setFileName(config_path);
            if(file.exists()) {
                bool isOK = file.open(QIODevice::ReadOnly);
                if(isOK == true) {
                    QByteArray array = file.readAll();

                    path = array;
                } else {
                    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
                file.close();
            } else {
                path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        }
    }
    return path;
}

void MainFrame::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control) {
        m_bCtrlKey_press = true;
    }
    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
        return;
    }
    QWidget::keyPressEvent(event);
}

void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
    if(m_bCtrlKey_press == true) {
        m_bCtrlKey_press = false;
    }
    QWidget::keyReleaseEvent(event);
}

void MainFrame::resizeEvent(QResizeEvent *event)
{
    m_pDownLoadingTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pDownLoadingTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_pRecycleTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pRecycleTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
}

void MainFrame::onNewBtnClicked()
{
    createNewTask("");
}

void MainFrame::onStartDownloadBtnClicked()
{
    QList<DataItem *> selectList;
    int selectedCount = 0;

    if(m_iCurrentListviewRow == 0) {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked && !m_pDownLoadingTableView->isRowHidden(i)) {
                ++selectedCount;
                if(selectList.at(i)->status != Global::Status::Active) {
                    if(selectList.at(i)->status == Global::Status::Lastincomplete) {
                        // QString  save_path=selectList.at(i)->savePath;
                        QString file_path = selectList.at(i)->savePath;
                        QString file_name = selectList.at(i)->fileName;
                        QString default_savePath = getDownloadSavepathFromConfig();
                        QString save_path;
                        if(default_savePath != selectList.at(i)->savePath) {
                            int name_length = file_name.size();
                            int file_path_length = file_path.size();
                            int folder_path_length = file_path_length - name_length - 1;
                            save_path = file_path.left(folder_path_length);
                        } else {
                            save_path = default_savePath;
                        }

                        QMap<QString, QVariant> opt;
                        opt.insert("dir", save_path);


                        S_Url_Info *url_info = new S_Url_Info;
                        S_Url_Info getUrlInfo;
                        DBInstance::getUrlById(selectList.at(i)->taskId, getUrlInfo);
                        if(getUrlInfo.m_taskId != "") {
                            if(getUrlInfo.m_downloadType == "torrent") {
                                QString select_num = getUrlInfo.m_selectedNum;
                                QMap<QString, QVariant> opt;
                                opt.insert("dir",         save_path);
                                opt.insert("select-file", select_num);
                                if(!QFile(getUrlInfo.m_seedFile).exists()) {
                                    showWarningMsgbox(tr("seed file not exists or broken;"));
                                    qDebug() << "seed file not exists or broken;";
                                } else {
                                    Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
                                    if(m_pUpdatetimer->isActive() == false) {
                                        m_pUpdatetimer->start(2 * 1000);
                                    }
                                }
                            }
                        } else {
                            //deal_download_upload_limit_period();
                            Aria2RPCInterface::Instance()->addUri(selectList.at(i)->url, opt, selectList.at(i)->taskId);
                            if(m_pUpdatetimer->isActive() == false) {
                                m_pUpdatetimer->start(2 * 1000);
                            }
                        }
                    } else {
                        Aria2RPCInterface::Instance()->unpause(selectList.at(i)->gid, selectList.at(i)->taskId);
                    }
                }
            }
        }
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
        qDebug() << "no item is selected,please check items!";
    }
}

void MainFrame::onPauseDownloadBtnClicked()
{
    QList<DataItem *> selectList;
    int selectedCount = 0;

    if(m_iCurrentListviewRow == 0) {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked && !m_pDownLoadingTableView->isRowHidden(i)) {
                ++selectedCount;
                if(selectList.at(i)->status != Global::Status::Paused) {
                    Aria2RPCInterface::Instance()->pause(selectList.at(i)->gid, selectList.at(i)->taskId);
                    QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
                    S_Task_Status  get_status;
                    S_Task_Status downloadStatus(selectList.at(i)->taskId,
                                                                                           Global::Status::Paused,
                                                                                           QDateTime::currentDateTime(),
                                                                                           selectList.at(
                                                                                               i)->completedLength,
                                                                                           selectList.at(i)->speed,
                                                                                           selectList.at(
                                                                                               i)->totalLength,
                                                                                           selectList.at(i)->percent,
                                                                                           selectList.at(i)->total,
                                                                                           finish_time);

                    if(DBInstance::getTaskStatusById(selectList.at(i)->taskId, get_status)) {
                       DBInstance::updateTaskStatusById(downloadStatus);
                    } else {
                        DBInstance::addTaskStatus(downloadStatus);
                    }
                }
            }
        }
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    }
}

void MainFrame::onDeleteDownloadBtnClicked()
{
    delDownloadingAction();
}

void MainFrame::delDownloadingAction()
{
    int selected_count = 0;

    if(m_iCurrentListviewRow == 2) {
        QList<DelDataItem *> pRecycleSelectlist;
        m_pRecycleDeleteList.clear();
        pRecycleSelectlist = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < pRecycleSelectlist.size(); ++i) {
            if((pRecycleSelectlist.at(i)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                m_pRecycleDeleteList.append(pRecycleSelectlist.at(i));
                selected_count++;
            }
        }
    } else {
        QList<DataItem *> pSelectList;
        m_pDeleteList.clear();
        pSelectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < pSelectList.size(); ++i) {
            DataItem *data = new DataItem;

            if(m_iCurrentListviewRow == 1) {
                if(pSelectList.at(i)->status == Complete) {
                    if((pSelectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_pDeleteList.append(data);
                        ++selected_count;
                    }
                }
            } else {
                if(pSelectList.at(i)->status != Complete) {
                    if((pSelectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        data = pSelectList.at(i);
                        m_pDeleteList.append(data);
                        ++selected_count;
                    }
                }
            }
        }
    }
    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showDeleteMsgbox(false);
    }
}

void MainFrame::slotRpcSuccess(QString method, QJsonObject json)
{
    if((method == ARIA2C_METHOD_ADD_URI)
       || (method == ARIA2C_METHOD_ADD_TORRENT)
       || (method == ARIA2C_METHOD_ADD_METALINK)) {
        m_pDownLoadingTableView->aria2MethodAdd(json, m_searchContent);
    } else if(method == ARIA2C_METHOD_TELL_STATUS) {
        m_pDownLoadingTableView->aria2MethodStatusChanged(json, m_iCurrentListviewRow, m_searchContent);
    } else if(method == ARIA2C_METHOD_SHUTDOWN) {
        m_pDownLoadingTableView->aria2MethodShutdown(json);
    } else if(method == ARIA2C_METHOD_GET_FILES) {
        m_pDownLoadingTableView->aria2MethodGetFiles(json, m_iCurrentListviewRow);
    } else if(method == ARIA2C_METHOD_UNPAUSE) {
        m_pDownLoadingTableView->aria2MethodUnpause(json, m_iCurrentListviewRow);
    }
    else if(method == ARIA2C_METHOD_FORCE_REMOVE) {
        m_pDownLoadingTableView->aria2MethodForceRemove(json);
    }
}

void MainFrame::slotRpcError(QString method, QString id, int error)
{
    qDebug() << "slot rpc error method is:" << method << error;

    // save_data_before_close();
    if(error == 400) {
        if((method == ARIA2C_METHOD_FORCE_REMOVE) && id.startsWith("REDOWNLOAD_")) {
            // 当重新下载400移除失败时直接重新下载
            QStringList sp = id.split("_");
            QString     taskId = sp.at(2);
            int  rd = sp.at(1).toInt();
            emit signalRedownload(taskId, rd);
        }
    }
}

void MainFrame::slotTableItemSelected(const QModelIndex &selected)
{
    bool chked = selected.model()->data(selected, TableModel::DataRole::Ischecked).toBool();

    if(m_bCtrlKey_press == false) {
        QList<DataItem *> data_list = m_pDownLoadingTableView->getTableModel()->dataList();
        for(int i = 0; i < data_list.size(); i++) {
            data_list.at(i)->Ischecked = false;
        }
        QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recycle_list.size(); i++) {
            recycle_list.at(i)->Ischecked = false;
        }
        m_pDownLoadingTableView->reset();
        m_pRecycleTableView->reset();
        ((TableModel *)selected.model())->setData(selected.model()->index(selected.row(), 0),
                                                      true,
                                                      TableModel::Ischecked);
    } else if(m_bCtrlKey_press == true) {
        m_pDownLoadingTableView->reset();
        m_pRecycleTableView->reset();
        ((TableModel *)selected.model())->setData(selected.model()->index(selected.row(), 0),
                                                      !chked,
                                                      TableModel::Ischecked);
    }
}


void MainFrame::updateMainUI()
{
    const QList<DataItem *> renderList = m_pDownLoadingTableView->getTableModel()->renderList();
    const QList<DataItem *> dataList = m_pDownLoadingTableView->getTableModel()->dataList();
    int activeCount = 0;
    for(const auto *item : dataList) {
        if((item->status == Global::Status::Active) || (item->status == Global::Status::Waiting)) {
            Aria2RPCInterface::Instance()->tellStatus(item->gid, item->taskId);
        }
    }

    for(const auto *item : dataList) {
        if((item->status == Global::Status::Active) || (item->status == Global::Status::Waiting) ||
           (item->status == Global::Status::Paused) || (item->status == Global::Status::Lastincomplete) ||
           (item->status == Global::Status::Error)) {
            ++activeCount;
        }
    }

    /*  if(this->g_search_content != "")
       {
          slot_searchEditTextChange(this->g_search_content);
       }*/
    if(activeCount == 0) {
        if(m_pUpdatetimer->isActive()) {
            m_pUpdatetimer->stop();
        }
    }
    setTaskNum(m_iCurrentListviewRow);
}

