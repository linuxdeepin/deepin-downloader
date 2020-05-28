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
#include "../database/dbinstance.h"

using namespace Global;

MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{
    init();
    initAria2();
    initConnection();
    init_tableData();
}

void MainFrame::init()
{

    m_pSettings = new Settings;


    m_iCurrentListviewRow = 0;

    // 添加设置界面
    DMenu *pSettingsMenu = new DMenu;
    m_pSettingAction = new QAction(tr("setting"), this);
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


    m_pDownLoadingTableView = new TableView(downloading);
    //m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
    m_pDownLoadingTableView->setColumnHidden(4, true);
    connect(m_pDownLoadingTableView, &TableView::header_stateChanged, this, &MainFrame::getHeaderStatechanged);
    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::clear_header_check);
    m_pRecycleTableView = new TableView(recycle);
    m_pRecycleTableView->verticalHeader()->setDefaultSectionSize(30);
    connect(m_pRecycleTableView, &TableView::header_stateChanged,  this, &MainFrame::getHeaderStatechanged);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::clear_header_check);

    m_pRecycleTableView->setColumnHidden(3, true);
    setAcceptDrops(true);
    m_pLeftWidget = new QWidget;
    m_pLeftWidget->setAutoFillBackground(true);
    QVBoxLayout *pLeftLayout = new QVBoxLayout(m_pLeftWidget);
    m_pLeftWidget->setPalette(p);
    m_pLeftWidget->setFixedWidth(132);
    pLeftLayout->setContentsMargins(10, 0, 10, 0);

    m_pRight_Widget = new QWidget;
    m_pNoTask_Widget = new QWidget;
    m_pNoTask_Widget->setAutoFillBackground(true);
    m_pNoTask_Widget->setPalette(p);

    QVBoxLayout *pNoTask_WidgetLayout = new QVBoxLayout(m_pNoTask_Widget);
    pNoTask_WidgetLayout->setContentsMargins(10, 0, 0, 0);
    m_pNoTask_label = new Dtk::Widget::DLabel();
    m_pNoTask_label->setForegroundRole(DPalette::PlaceholderText);
    QFont lable_font;
    lable_font.setPointSize(15);
    lable_font.setBold(QFont::DemiBold);
    lable_font.setFamily("T5");
    QPalette font_p;
    QColor   color = QColor(192, 198, 212, 76);
    font_p.setColor(QPalette::WindowText, color);
    m_pNoTask_label->setFont(lable_font);
    m_pNoTask_label->setText(tr("current no task"));
    m_pNoTask_label->setAlignment(Qt::AlignHCenter);

    pNoTask_WidgetLayout->addWidget(m_pNoTask_label);
    m_pNoTask_tip_Label = new QLabel();
    QFont noTask_tip_Label_font;
    noTask_tip_Label_font.setPointSize(13);
    m_pNoTask_tip_Label->setFont(noTask_tip_Label_font);
    QPalette noTask_tip_Label_p;
    noTask_tip_Label_p.setColor(QPalette::WindowText, QColor(65, 77, 104, 70));
    m_pNoTask_tip_Label->setText(tr("you can click button to start new task"));
    m_pNoTask_tip_Label->setAlignment(Qt::AlignHCenter);
    m_pNoTask_tip_Label->setPalette(noTask_tip_Label_p);
    pNoTask_WidgetLayout->addWidget(m_pNoTask_tip_Label);
    pNoTask_WidgetLayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_pRight_Widget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);
    m_pRightStackwidget = new QStackedWidget(this);

    QPalette p_task_num;
    p_task_num.setBrush(this->backgroundRole(), QBrush(QColor(255, 255, 255, 178)));
    m_pTask_Num_Widget = new QWidget;
    m_pTask_Num_Widget->setFixedHeight(30);
    m_pTask_Num_Widget->setAutoFillBackground(true);
    m_pTask_Num_Widget->setPalette(p_task_num);
    QHBoxLayout *Task_Num_WidgetLayout = new QHBoxLayout(m_pTask_Num_Widget);
    Task_Num_WidgetLayout->setMargin(0);
    m_pTask_Num = new QLabel(tr("0 task"));
    m_pTask_Num->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pTask_Num->setPalette(noTask_tip_Label_p);
    Task_Num_WidgetLayout->addWidget(m_pTask_Num);
    m_pRightStackwidget->addWidget(m_pDownLoadingTableView);
    m_pRightStackwidget->addWidget(m_pRecycleTableView);
    pRightLayout->addWidget(m_pRightStackwidget);
    pRightLayout->addWidget(m_pNoTask_Widget);
    pRightLayout->addWidget(m_pTask_Num_Widget);
    pRightLayout->setSpacing(0);

    pMainHLayout->addWidget(m_pLeftWidget);
    pMainHLayout->addWidget(m_pRight_Widget);

    m_pLeft_list = new DListView;
    m_pLeft_list->setItemSpacing(0);
    m_pLeft_list->setItemSize(QSize(112, 40));
    m_pLeft_list->setItemMargins(QMargins(10, 2, 5, 2));
    m_pLeft_list->setIconSize(QSize(14, 14));
    QFont font;
    font.setFamily("Source Han Sans");
    font.setPointSize(14);
    m_pLeft_list->setFont(font);
    QStandardItemModel* pLeftList_model = new QStandardItemModel(this);

    m_pDownloading_item = new QStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("download"));
    m_pDownloadFinish_item = new QStandardItem(QIcon::fromTheme("dcc_print_done"), tr("finish"));
    m_pRecycle_item = new QStandardItem(QIcon::fromTheme("dcc_list_delete"), tr("recycle"));
    m_pDownloading_item->setBackground(QColor(255, 255, 255));
    m_pDownloadFinish_item->setBackground(QColor(255, 255, 255));
    m_pRecycle_item->setBackground(QColor(255, 255, 255));
    m_pDownloading_item->setEditable(false);
    m_pDownloadFinish_item->setEditable(false);
    m_pRecycle_item->setEditable(false);
    pLeftList_model->appendRow(m_pDownloading_item);
    pLeftList_model->appendRow(m_pDownloadFinish_item);
    pLeftList_model->appendRow(m_pRecycle_item);
    m_pLeft_list->setModel(pLeftList_model);
    pLeftLayout->addWidget(m_pLeft_list, 0);
    //updatetimer = new QTimer(this);
    m_pLeft_list->setCurrentIndex(pLeftList_model->index(0, 0));
    m_pDownLoadingTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    //recycle_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    m_pRightStackwidget->setCurrentIndex(0);

    m_pClipboard = QApplication::clipboard();        //获取当前剪切板
    initTray();
}

void MainFrame::initTray()
{
    QIcon tryIcon = QIcon(":/icons/icon/downloader5.svg");
        m_pSystemTray = new QSystemTrayIcon(this);
        m_pSystemTray->setIcon(tryIcon);
        m_pSystemTray->setToolTip(tr("下载器"));

        QAction *pShowMainAct = new QAction(tr("显示主界面"), this);

        QAction *pNewDownloadAct = new QAction(tr("新建任务"), this);
        QAction *pStartAllAct = new QAction(tr("开始全部任务"), this);
        QAction *pPauseAllAct = new QAction(tr("暂停全部任务"), this);
        //QAction *pShutdownAct = new QAction(tr("关机"), this);
        //QAction *pSleepAct = new QAction(tr("休眠"), this);
        //QAction *pQuiteAct = new QAction(tr("退出下载器"), this);
//        QMenu *pAfterDownloadMenu = new QMenu(tr("下载完成后"), this);
//        pAfterDownloadMenu->addAction(pShutdownAct);
//        pAfterDownloadMenu->addAction(pSleepAct);
//        pAfterDownloadMenu->addAction(pQuiteAct);

//        QAction *pShowAct = new QAction(tr("显示"), this);
//        QAction *pShowOnDownloadAct = new QAction(tr("下载时显示"), this);
//        QAction *pHideAct = new QAction(tr("隐藏"), this);
//        QMenu *pTraySetMenu = new QMenu(tr("托盘设置"), this);
//        pTraySetMenu->addAction(pShowAct);
//        pTraySetMenu->addAction(pShowOnDownloadAct);
//        pTraySetMenu->addAction(pHideAct);

        QAction *pQuitAct = new QAction(tr("退出"), this);


        QMenu* pTrayMenu = new QMenu(this);
        pTrayMenu->addAction(pShowMainAct);
        pTrayMenu->addAction(pNewDownloadAct);
        pTrayMenu->addAction(pStartAllAct);
        pTrayMenu->addAction(pPauseAllAct);
        //pTrayMenu->addMenu(pAfterDownloadMenu);
        //pTrayMenu->addMenu(pTraySetMenu);
        pTrayMenu->addAction(pQuitAct);

        //连接信号与槽
        connect(pShowMainAct, &QAction::triggered, [=](){this->show();});
        connect(pNewDownloadAct, &QAction::triggered, [=](){createNewTask("");});
        connect(pQuitAct, &QAction::triggered, [=](){qApp->quit();});
        connect(m_pSystemTray, &QSystemTrayIcon::activated, this, &MainFrame::onActivated);
        m_pSystemTray->setContextMenu(pTrayMenu);
        m_pSystemTray->show();
}

void MainFrame::initConnection()
{
    connect(m_pToolBar, &TopButton::newDownloadBtnClicked, this, &MainFrame::onNewBtnClicked);
    connect(m_pSettingAction,&QAction::triggered, this, &MainFrame::onSettingsMenuClicked);
    connect(m_pClipboard, &QClipboard::dataChanged,this,&MainFrame::onClipboardDataChanged);
    connect(m_pLeft_list, &DListView::clicked, this, &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::getPalettetypechanged);
    connect(m_pDownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(m_pRecycleTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::clear_header_check);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::clear_header_check);
    connect(this, &MainFrame::tableChanged, m_pToolBar, &TopButton::get_table_changed);
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
    this->hide();
    event->ignore();
}

void MainFrame::createNewTask(QString url)
{
    newTaskWidget *pNewTaskWidget = new newTaskWidget(url);
    connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewDowloadUrl);
    pNewTaskWidget->exec();
}
void MainFrame::on_tray_quit_click()
{
    //save_data_before_close();
    qApp->quit();
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

void MainFrame::init_tableData()
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
        data->taskId = list.at(i).m_task_id;
        S_Task_Status taskStatus;
        DBInstance::getTaskStatusById(data->taskId, taskStatus);
        if(taskStatus.m_task_id != nullptr) {
            data->percent = 0;
            data->fileName = list.at(i).m_download_filename;
            data->savePath = list.at(i).m_download_path;
            data->Ischecked = 0;
            data->totalLength = taskStatus.m_totalLength;
            data->completedLength = taskStatus.m_compeletedLength;
            if(taskStatus.m_download_status == Global::Status::Active) {
                data->status = Global::Status::Lastincomplete;
            } else {
                data->status = taskStatus.m_download_status;
            }
            data->total = taskStatus.m_totalFromSource;
            if(data->status == Global::Status::Lastincomplete) {
                ++task_num;
            }
            if(data->status != Global::Status::Removed) {
                if(data->status == Global::Status::Complete) {
                    data->time = taskStatus.m_modify_time.toString("yyyy-MM-dd hh:mm:ss");
                }
                if(data->status == Global::Status::Lastincomplete) {
                    m_pNoTask_Widget->hide();
                    //QVariant autostart_unfinished_task_switchbutton = m_settings->settings->getOption(
                    //    "basic.select_multiple.autostart_unfinished_task_switchbutton");
                    m_pDownLoadingTableView->getTableModel()->append(data);
/*                    if(autostart_unfinished_task_switchbutton.toBool()) {
                        QString save_path = get_download_savePath_from_config();
                        QMap<QString, QVariant> opt;
                        opt.insert("dir", save_path);
                        Tb_Url_Info *url_info = new Tb_Url_Info;
                        Tb_Url_Info *get_url_info;
                        get_url_info = url_info->getTbUrlInfoByTaskId(data->taskId);
                        if(get_url_info != nullptr) {
                            if(get_url_info->download_type == "torrent") {
                                QString select_num = get_url_info->selectedNum;
                                QMap<QString, QVariant> opt;
                                opt.insert("dir",         save_path);
                                opt.insert("select-file", select_num);
                                if(!QFile(get_url_info->seedFile).exists()) {
                                    show_Warning_MsgBox(tr("seed file not exists or broken;"));
                                } else {
                                    aria2c->addTorrent(get_url_info->seedFile, opt, get_url_info->task_id);
                                    if(updatetimer->isActive() == false) {
                                        updatetimer->start(2 * 1000);
                                    }
                                }
                            }
                        } else {
                            deal_download_upload_limit_period();
                            aria2c->addUri(data->url, opt, data->taskId);
                            if(updatetimer->isActive() == false) {
                                updatetimer->start(2 * 1000);
                            }
                        }
                    }*/
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
                deldata->deleteTime = taskStatus.m_modify_time.toString("yyyy-MM-dd hh:mm:ss");
                deldata->totalLength = data->totalLength;
                deldata->completedLength = data->completedLength;
                deldata->finishTime = taskStatus.m_finish_time.toString("yyyy-MM-dd hh:mm:ss");
                m_pRecycleTableView->getTableModel()->append(deldata);
            }
        }
    }
    refreshTableView(0);

    setTaskNum(m_iCurrentListviewRow);
}

void MainFrame::refreshTableView(const int &index, bool isClearSelection)
{
    switch(index) {
    case 0: {
        m_pDownLoadingTableView->getTableModel()->switchDownloadingMode();
        m_pDownLoadingTableView->setColumnHidden(3, false);
        m_pDownLoadingTableView->setColumnHidden(4, true);

        // 联动工具栏按钮 begin
        int chkedCnt = 0;
        if(m_iCurrentListviewRow == 0) {
            QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
            for(int i = 0; i < selectList.size(); i++) {
                if(selectList.at(i)->Ischecked) {
                    chkedCnt++;
                }
            }
            if(chkedCnt > 0) {
                m_pToolBar->enableStartBtn(true);
                m_pToolBar->enablePauseBtn(true);
                m_pToolBar->enableDeleteBtn(true);
            } else {
                m_pToolBar->enableStartBtn(false);
                m_pToolBar->enablePauseBtn(false);
                m_pToolBar->enableDeleteBtn(false);
            }
        }

        // end
        break;
    }

    case 1:
        m_pDownLoadingTableView->getTableModel()->switchFinishedMode();

        m_pDownLoadingTableView->setColumnHidden(3, true);
        m_pDownLoadingTableView->setColumnHidden(4, false);
        break;
    }


    m_pDownLoadingTableView->update();
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
            m_pNoTask_Widget->show();
        } else {
            m_pNoTask_Widget->hide();
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
            m_pNoTask_Widget->show();
        } else {
            m_pNoTask_Widget->hide();
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
            m_pNoTask_Widget->show();
        } else {
            m_pNoTask_Widget->hide();
        }
    }
    m_pTask_Num->setText(active_num);
}

void MainFrame::onNewBtnClicked()
{
    createNewTask("");
}

void MainFrame::onSettingsMenuClicked()
{

//    SettingsWidget *pSettingWidget = new  SettingsWidget;
//    QDesktopWidget *pDeskWdg = QApplication::desktop();
//    QRect rctAvaild = pDeskWdg->availableGeometry();
//    pSettingWidget->move((rctAvaild.width() - pSettingWidget->width()) / 2, (rctAvaild.height() - pSettingWidget->height()) / 2);
//    pSettingWidget->show();

    DSettingsDialog *pSettingsDialog = new DSettingsDialog(this);
    pSettingsDialog->widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);
//    pSettingsDialog->widgetFactory()->registerWidget("downloadtraysetting", Settings::createDownloadTraySettingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloaddiskcachesetting", Settings::createDownloadDiskCacheSettiingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloadspeedlimitsetting", Settings::createDownloadSpeedLimitSettiingHandle);
    pSettingsDialog->updateSettings( m_pSettings->m_pSettings );
    pSettingsDialog->exec();
    delete pSettingsDialog;
    m_pSettings->m_pSettings->sync();

}

void MainFrame::slotRPCSuccess(QString method, QJsonObject json)
{
    if(method == ARIA2C_METHOD_ADD_URI)
    {
        QString id = json.value("id").toString();
        QString gId = json.value("result").toString();

        //根据ID从下载列表是否存在下载任务,如果不存在创建任务,如果存在更新任务状态
    //    DataItem* pFindData = downloading_tableview->get_tableViewModel()->find(id);

    }
}

void MainFrame::slotRPCError(QString method, QString id, int error)
{

}

void MainFrame::onClipboardDataChanged()
{
    QString text = m_pClipboard->text();
    if( (-1 != text.indexOf("ftp:")) || (-1 != text.indexOf("http:")) || (-1 != text.indexOf("https:")) || (-1 != text.indexOf("magnet:"))){
         createNewTask(text);
    }
}

void MainFrame::onListClicked(const QModelIndex &index)
{
    m_iCurrentListviewRow = index.row();
    QString DownloadTask_Lable_Text;
    if((index.row() == 0) || (index.row() == 1)) {
        m_pRightStackwidget->setCurrentIndex(0);
        refreshTableView(index.row());
        m_pDownLoadingTableView->horizontalHeader()->reset();
        bool switched = true;
        m_pDownLoadingTableView->reset(switched);
        if(index.row() == 1) {
            connect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(30);
            m_pNoTask_Widget->show();
            m_pNoTask_label->setText(tr("current no download finish task"));
            m_pNoTask_tip_Label->hide();
        } else {
            disconnect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
            //m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
            m_pNoTask_label->setText(tr("current no download task"));
            m_pNoTask_Widget->show();
            m_pNoTask_tip_Label->show();
        }
    } else {
        m_pRightStackwidget->setCurrentIndex(1);
        m_pNoTask_Widget->show();
        m_pNoTask_label->setText(tr("current no delete files"));
        m_pNoTask_tip_Label->hide();
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
    m_pLeft_list->setPalette(DGuiApplicationHelper::instance()->applicationPalette());

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

        m_pNoTask_tip_Label->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        QPalette tableview_palette;

        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());

        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_pNoTask_Widget->setPalette(tableview_palette);
        m_pTask_Num_Widget->setPalette(tableview_palette);
        m_pDownloading_item->setIcon(QIcon::fromTheme("dcc_list_downloading_dark"));
        m_pDownloadFinish_item->setIcon(QIcon::fromTheme("dcc_print_done_dark"));
        m_pRecycle_item->setIcon(QIcon::fromTheme("dcc_list_delete_dark"));
        QPalette noTask_tip_Label_p;
        noTask_tip_Label_p.setBrush(QPalette::WindowText,
                                    DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_pTask_Num->setPalette(noTask_tip_Label_p);
    }
    else if(DGuiApplicationHelper::instance()->themeType()==1) {
        QPalette p;
        p.setColor(QPalette::Background, QColor(255, 255, 255));
        QPalette tableview_palette;
        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());
        m_pLeftWidget->setPalette(p);
        m_pNoTask_Widget->setPalette(tableview_palette);
        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_pTask_Num_Widget->setPalette(tableview_palette);

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
        m_pTask_Num->setPalette(noTask_tip_Label_p);
    }
}

void MainFrame::getHeaderStatechanged(bool i)
{
    // ToolBar禁用按钮联动：表头全选复选框状体变化 begin
    int cnt = (m_iCurrentListviewRow == 2 ? m_pRecycleTableView->getTableModel()->rowCount(QModelIndex())
               : m_pDownLoadingTableView->getTableModel()->rowCount(QModelIndex()));

    if(cnt > 0) {
        if(m_iCurrentListviewRow == 0) {
            m_pToolBar->enableStartBtn(i);
            m_pToolBar->enablePauseBtn(i);
            m_pToolBar->enableDeleteBtn(i);
        } else {
            m_pToolBar->enableStartBtn(false);
            m_pToolBar->enablePauseBtn(false);
            m_pToolBar->enableDeleteBtn(i);
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
            if(!i) {
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
            if(!i) {
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
    QStringList _urlList = url.split("\n");
    _urlList = _urlList.toSet().toList();   //url去重
    bool _isExitsUrl = false;
    //判断url是否在数据中已存在
    for (int i = 0;i < _urlList.size(); i++)
    {
        DBInstance::isExistUrl(_urlList[i],_isExitsUrl);
        if(_isExitsUrl)
        {
            _urlList.removeAt(i);
            --i;
        }
    }
    if(_urlList.isEmpty())
    {
        qDebug()<<"url is NUll";
        return;
    }
    //将url加入数据库和aria
    S_Task _task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    for (int i = 0; i < _urlList.size(); i++)
    {
        _task = getUrlToName(_urlList[i], savePath);
        DBInstance::addTask(_task);
        Aria2RPCInterface::Instance()->addNewUri(_task.m_url,savePath,_task.m_task_id);
    }

    m_pNoTask_Widget->hide();
    //定时器打开
}

S_Task MainFrame::getUrlToName(QString url, QString savePath)
{
    //获取url文件名
    QString _fileName;
    if(url.startsWith("magnet") && url.contains("&"))
    {
        _fileName = url.split("&")[0];
        if(_fileName.contains("btih:"))
        {
            _fileName=_fileName.split("btih:")[1]+".torrent";
        }
        else
        {
            _fileName=url.right(40);
        }
    }
    else
    {
         _fileName=QString(url).right(url.length() - url.lastIndexOf('/') - 1);
    }

    //对url进行转码
    if(!_fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
    {
        const QByteArray _byte=_fileName.toLatin1();
        QString _decode=QUrl::fromPercentEncoding(_byte);
        if(_decode.contains("?"))
        {
            _decode=_decode.split("?")[0];
        }
        _fileName=_decode;
    }

    S_Task _task;
    _task.m_task_id = QUuid::createUuid().toString();
    _task.m_gid = "";
    _task.m_gid_index = 0;
    _task.m_url = url;
    _task.m_download_path = savePath + "/" +_fileName;
    _task.m_download_filename = _fileName;
    _task.m_create_time = QDateTime::currentDateTime();
    return _task;
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
            pActionStart->setText(tr("start"));
            delmenlist->addAction(pActionStart);
            //connect(pActionStart, &QAction::triggered, this, &MainFrame::onStartDownloadBtnClicked);
        }
        if(activeCount > 0) {
            QAction *pActionPause = new QAction();
            pActionPause->setText(tr("pause"));
            delmenlist->addAction(pActionPause);
            //connect(pActionPause, &QAction::triggered, this, &MainFrame::onPauseDownloadBtnClicked);
        }
        if(errorCount > 0) {
            QAction *pActionredownload = new QAction();
            pActionredownload->setText(tr("Redownload"));
            delmenlist->addAction(pActionredownload);
            //connect(pActionredownload, &QAction::triggered, this, &MainFrame::redownload_Action);
        }
        delmenlist->addSeparator();
    }
    if(m_iCurrentListviewRow == 2) {
        QAction *returned_to_origin = new QAction();
        returned_to_origin->setText(tr("return to origin"));
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
    // &MainWindow::moveTo_directory_Action);

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
            pAction_rename->setText(tr("rename"));
            delmenlist->addAction(pAction_rename);
            delmenlist->addSeparator();
            //connect(pAction_rename, &QAction::triggered, this, &MainFrame::rename_Action);
        }
    }
    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 2)) {
        QAction *pActionredownload = new QAction();
        pActionredownload->setText(tr("Redownload"));
        delmenlist->addAction(pActionredownload);
        //connect(pActionredownload, &QAction::triggered, this, &MainFrame::redownload_Action);
    }
    QAction *pAction_copy_download_url = new QAction();
    pAction_copy_download_url->setText(tr("Copy Download Url"));
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
    pAction_delete_permanently->setText(tr("Delete permanently"));
    delmenlist->addAction(pAction_delete_permanently);
    //connect(pAction_delete_permanently, &QAction::triggered, this, &MainFrame::delete_permanently_Action);
    if(m_iCurrentListviewRow == 2) {
        QAction *pAction_clear_recycle = new QAction();
        pAction_clear_recycle->setText(tr("Clear"));
        delmenlist->addAction(pAction_clear_recycle);
        //connect(pAction_clear_recycle, &QAction::triggered, this, &MainFrame::clear_recycle_Action);
    }

    delmenlist->exec(QCursor::pos());
    delete  delmenlist;
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
    TableModel *dtModel = this->m_pDownLoadingTableView->getTableModel();
    TableModel *rtModel = this->m_pRecycleTableView->getTableModel();
    if(text == "") {
        for(int i = 0; i < dtModel->rowCount(QModelIndex()); i++) {
            this->m_pDownLoadingTableView->setRowHidden(i, false);
            dtModel->setData(dtModel->index(i, 0), false, TableModel::Ischecked);
        }
        for(int i = 0; i < rtModel->rowCount(QModelIndex()); i++) {
            this->m_pRecycleTableView->setRowHidden(i, false);
            rtModel->setData(dtModel->index(i, 0), false, TableModel::Ischecked);
        }
    } else {
        for(int i = 0; i < dtModel->rowCount(QModelIndex()); i++) {
            this->m_pDownLoadingTableView->setRowHidden(i, false);
            QString fileName = dtModel->data(dtModel->index(i, 1), TableModel::FileName).toString();
            if(!fileName.contains(text, Qt::CaseInsensitive)) {
                this->m_pDownLoadingTableView->setRowHidden(i, true);
            }
            dtModel->setData(dtModel->index(i, 0), false, TableModel::Ischecked);
        }

        for(int i = 0; i < rtModel->rowCount(QModelIndex()); i++) {
            this->m_pRecycleTableView->setRowHidden(i, false);
            QString fileName = rtModel->data(rtModel->index(i, 1), TableModel::FileName).toString();
            if(!fileName.contains(text, Qt::CaseInsensitive)) {
                this->m_pRecycleTableView->setRowHidden(i, true);
            }
            rtModel->setData(dtModel->index(i, 0), false, TableModel::Ischecked);
        }
    }
    this->m_pDownLoadingTableView->reset();
    this->m_pRecycleTableView->reset();
    setTaskNum(m_iCurrentListviewRow);

}
