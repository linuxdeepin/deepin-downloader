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
//#include "messagebox.h"
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

    m_pClipboard = new ClipboardTimer;       //获取当前剪切板
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
    connect(m_pClipboard, &ClipboardTimer::sendClipboardText,this,&MainFrame::onClipboardDataChanged);
    connect(m_pLeft_list, &DListView::clicked, this, &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::getPalettetypechanged);
    connect(m_pDownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(m_pRecycleTableView, &TableView::customContextMenuRequested, this, &MainFrame::slotContextMenu);
    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::clear_header_check);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::clear_header_check);
    connect(this, &MainFrame::tableChanged, m_pToolBar, &TopButton::get_table_changed);
    connect(m_pToolBar, &TopButton::getSearchEditTextChange, this, &MainFrame::slotSearchEditTextChanged);
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
    static newTaskWidget *pNewTaskWidget = new newTaskWidget(url);
    //connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewDowloadUrl);
    pNewTaskWidget->exec();
}
void MainFrame::onTrayQuitClick()
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
                    //QVariant autostart_unfinished_task_switchbutton = m_pSettings->m_pSettings->getOption(
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

//void MainFrame::slotRPCSuccess(QJsonObject json)
//{
//    if(method == ARIA2C_METHOD_ADD_URI
//            || method == ARIA2C_METHOD_ADD_TORRENT
//            || method == ARIA2C_METHOD_ADD_METALINK)
//    {
//        QString id=json.value("id").toString();
//        QString gId = json.value("result").toString();

//        //判断下载列表中是否有这条记录
//        DataItem*  finddata=downloading_tableview->get_tableViewModel()->find(id);
//        if(finddata!=nullptr)//如果没有
//        {
//            finddata->gid=gId;
//            finddata->taskId=id;
//            QDateTime finish_time=QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
//            Tb_Donwload_Task_Status get_status;
//            Tb_Donwload_Task_Status *download_status = new Tb_Donwload_Task_Status(finddata->taskId,Global::Status::Active,QDateTime::currentDateTime(),finddata->completedLength,finddata->speed,finddata->totalLength,finddata->percent,finddata->total,finish_time);

//            if(get_status.getTbDownloadStatusByTaskId(finddata->taskId)!= NULL)
//            {
//                download_status->updateTbDownloadStatusByTaskId(download_status);
//            }
//            else
//            {
//                download_status->addTbDownloadTaskStatus(download_status);
//            }
//            finddata->status=Global::Status::Active;
//        }
//        else//如果有
//        {
//            //获取下载信息
//            //aria2c->tellStatus(gId, gId);
//            Aria2RPCInterface::Instance()->getFiles(gId, id);
//            DataItem *data = new DataItem;
//            data->taskId=id;
//            data->gid = gId;
//            data->Ischecked=0;
//            QDateTime time=QDateTime::currentDateTime();
//            data->createTime=time.toString("yyyy-MM-dd hh:mm:ss");

//            Tb_Task *get_task_info;
//            Tb_Task task_info;
//            get_task_info=task_info.getTbTaskByTaskId(id);
//            Tb_Task *task ;
//            if(get_task_info!=NULL)
//            {

//                task= new Tb_Task(get_task_info->task_id,gId,0,get_task_info->url,get_task_info->download_path,get_task_info->download_filename,time);
//                task_info.updateTbTaskByTaskId(task);
//                data->fileName=get_task_info->download_filename;

//            }
//            else
//            {
//                task= new Tb_Task(id, gId, 0, "", "","Unknow", time);
//                task_info.addTbTask(task);
//            }
//            downloading_tableview->get_tableViewModel()->append(data);
//            if(this->g_search_content!="" && !data->fileName.contains(this->g_search_content))
//            {
//                TableViewModel* dtModel = this->downloading_tableview->get_tableViewModel();
//                downloading_tableview->setRowHidden(dtModel->rowCount(QModelIndex()),true);


//            }
//          }
//        refreshTableView(current_listview_row);
//    }

//}

//void MainFrame::slotRPCError(QString id, int error)
//{

//}


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
        refreshTableView(index.row());
        m_pDownLoadingTableView->horizontalHeader()->reset();
        bool switched = true;
        m_pDownLoadingTableView->reset(switched);
        if(index.row() == 1) {
            //connect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(30);
            m_pNoTask_Widget->show();
            m_pNoTask_label->setText(tr("current no download finish task"));
            m_pNoTask_tip_Label->hide();
        } else {
            //disconnect(m_pDownLoadingTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(get_doubleClicked(QModelIndex)));
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

void MainFrame::getNewdowloadSlot(QString url, QString savepath)
{
    QMap<QString, QVariant> opt;

    opt.insert("dir", savepath);
    QStringList url_list = url.split("\n");
    int sameUrl = 0;
    QList<QString> repeat_url_list;
    QStringList    add_url_list;
    for(int j = 0; j < url_list.size(); j++) {
        if(add_url_list.size() == 0) {
            add_url_list.append(url_list.at(j));
        } else {
            bool found_same = false;
            for(int k = 0; k < add_url_list.size(); k++) {
                QString url_decode;
                QString add_url_decode;
                if(!url_list.at(j).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte_download = url_list.at(j).toLatin1();
                    url_decode = QUrl::fromPercentEncoding(byte_download);
                } else {
                    url_decode = url_list.at(j);
                }
                if(!add_url_list.at(k).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte_download = add_url_list.at(k).toLatin1();
                    add_url_decode = QUrl::fromPercentEncoding(byte_download);
                } else {
                    add_url_decode = add_url_list.at(k);
                }
                if(add_url_decode == url_decode) {
                    found_same = true;
                    continue;
                }
            }
            if(!found_same) {
                add_url_list.append(url_list.at(j));
            }
        }
    }
    if(add_url_list.size() > 0) {
        for(int i = 0; i < add_url_list.size(); i++) {
            int     sameUrl_count = 0;
            QUuid   id = QUuid::createUuid();
            QString strId = id.toString();
            QString download_url = add_url_list.at(i);
            //Tb_Task tb_task;
            QList<DataItem *> data_list = m_pDownLoadingTableView->getTableModel()->dataList();
            QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
            QString download_url_decode;
            if(!download_url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                const QByteArray byte_download = download_url.toLatin1();
                download_url_decode = QUrl::fromPercentEncoding(byte_download);
            } else {
                download_url_decode = download_url;
            }
            for(int i = 0; i < data_list.size(); i++) {
                DataItem *data = data_list.at(i);

                QString url_decode;
                if(!data->url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                    const QByteArray byte = data->url.toLatin1();
                    url_decode = QUrl::fromPercentEncoding(byte);
                } else {
                    url_decode = data->url;
                }

                if(!download_url.isEmpty()) {
                    if((url_decode == download_url_decode) || (download_url == url_decode)) {
                        repeat_url_list.append(download_url);
                        ++sameUrl_count;
                        sameUrl = sameUrl + sameUrl_count;
                    }
                }
            }
            if(sameUrl == 0) {
                for(int i = 0; i < recycle_list.size(); i++) {
                    DelDataItem *data = recycle_list.at(i);
                    QString url_decode;
                    if(!data->url.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                        const QByteArray byte = data->url.toLatin1();
                        url_decode = QUrl::fromPercentEncoding(byte);
                    } else {
                        url_decode = data->url;
                    }
                    if((url_decode == download_url_decode) || (download_url == url_decode)) {
                        ++sameUrl_count;
                        repeat_url_list.append(download_url);
                        sameUrl = sameUrl + sameUrl_count;
                    }
                }
            }

            if(sameUrl_count == 0) {
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
                    QString filename_decode = QUrl::fromPercentEncoding(filename_byte);
                    if(filename_decode.contains("?")) {
                        filename_decode = filename_decode.split("?")[0];
                    }
                    filename = filename_decode;
                }
                S_Task add_task;
                add_task.m_task_id = strId;
                add_task.m_gid = "";
                add_task.m_gid_index = 0;
                add_task.m_url = download_url;
                add_task.m_download_path = savepath + "/" + filename;
                add_task.m_download_filename = filename;
                add_task.m_create_time = QDateTime::currentDateTime();
                DBInstance::addTask(add_task);
                m_pNoTask_Widget->hide();
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

QString MainFrame::getFileName(const QString &url)
{
    return QString(url).right(url.length() - url.lastIndexOf('/') - 1);
}

QString MainFrame::formatFileSize(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + "B";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + "KB";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + "MB";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + "GB";
    }

    return result;
}

QString MainFrame::formatDownloadSpeed(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + " B/s";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + " KB/s";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + " MB/s";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + " GB/s";
    }

    return result;
}

void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
//    MessageBox *msg = new MessageBox(Warnings);

//    msg->set_warning_MsgBox(title, tr("sure"), "", sameUrlCount, sameUrlList);
//    msg->exec();
}

QString   MainFrame::getDownloadSavepathFromConfig()
{
    QVariant downloadRadioGroup =
        m_pSettings->m_pSettings->getOption("basic.downloadDirectory.downloadDirectoryFileChooser");

    QString path = "";

    QString current_value = downloadRadioGroup.toString();

    // 无设置信息
    if(current_value.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    } else {
        // 用户设置默认目录
        if(current_value.contains("custom;")) {
            path = current_value.section(QString(';'), 1, 1);
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

void MainFrame::aria2MethodAdd(QJsonObject json)
{
    QString id = json.value("id").toString();
    QString gId = json.value("result").toString();

    DataItem *finddata = m_pDownLoadingTableView->getTableModel()->find(id);
    if(finddata != nullptr) {
        finddata->gid = gId;
        finddata->taskId = id;
        QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        S_Task_Status  get_status;
        S_Task_Status *download_status = new S_Task_Status(finddata->taskId,
                                                                               Global::Status::Active,
                                                                               QDateTime::currentDateTime(),
                                                                               finddata->completedLength,
                                                                               finddata->speed,
                                                                               finddata->totalLength,
                                                                               finddata->percent,
                                                                               finddata->total,
                                                                               finish_time);


        S_Task_Status task;
        DBInstance::getTaskStatusById(finddata->taskId, task);
        if(task.m_task_id != "") {
            DBInstance::updateTaskStatusById(*download_status);
        } else {
            DBInstance::addTaskStatus(*download_status);
        }
        finddata->status = Global::Status::Active;
    } else {
        // 获取下载信息
        // aria2c->tellStatus(gId, gId);
        Aria2RPCInterface::Instance()->getFiles(gId, id);
        DataItem *data = new DataItem;
        data->taskId = id;
        data->gid = gId;
        data->Ischecked = 0;
        QDateTime time = QDateTime::currentDateTime();
        data->createTime = time.toString("yyyy-MM-dd hh:mm:ss");

        S_Task get_task_info;
        S_Task  task_info;
        DBInstance::getTaskByID(id, get_task_info);
        S_Task *task;
        if(get_task_info.m_task_id != "") {
            task = new S_Task(get_task_info.m_task_id,
                               gId,
                               0,
                               get_task_info.m_url,
                               get_task_info.m_download_path,
                               get_task_info.m_download_filename,
                               time);
            DBInstance::updateTaskByID(*task);
            data->fileName = get_task_info.m_download_filename;
        } else {
            task = new S_Task(id, gId, 0, "", "", "Unknow", time);
            DBInstance::addTask(*task);
        }
        m_pDownLoadingTableView->getTableModel()->append(data);
        if((m_searchContent != "") && !data->fileName.contains(m_searchContent)) {
            TableModel *dtModel = this->m_pDownLoadingTableView->getTableModel();
            m_pDownLoadingTableView->setRowHidden(dtModel->rowCount(QModelIndex()), true);
        }
    }
    refreshTableView(m_iCurrentListviewRow);
}

void MainFrame::aria2MethodStatusChanged(QJsonObject json)
{
    QJsonObject result = json.value("result").toObject();
    QJsonObject bittorrent = result.value("bittorrent").toObject();
    QString     mode;
    QString     bittorrent_name;
    QString     taskId = json.value("id").toString();
    QString     bittorrent_dir = "";

    if(!bittorrent.isEmpty()) {
        mode = bittorrent.value("mode").toString();
        if(mode == "multi") {
            bittorrent_dir = result.value("dir").toString();
        }
        QJsonObject btInfo = bittorrent.value("info").toObject();
        bittorrent_name = btInfo.value("name").toString();
        QString infoHash = result.value("infoHash").toString();
        S_Url_Info  tb_url_info;
        S_Url_Info get_url_info;
        DBInstance::getUrlById(taskId, get_url_info);
        if(get_url_info.m_task_id != "") {
            if(get_url_info.m_infoHash.isEmpty()) {
                S_Url_Info *url_info = new S_Url_Info(get_url_info.m_task_id,
                                                        get_url_info.m_url,
                                                        get_url_info.m_download_type,
                                                        get_url_info.m_seedFile,
                                                        get_url_info.m_selectedNum,
                                                        infoHash);
                DBInstance::updateUrlById(*url_info);
            }
        }
    }
    QJsonArray files = result.value("files").toArray();

    QString filePath;
    QString fileUri;
    for(int i = 0; i < files.size(); ++i) {
        QJsonObject file = files[i].toObject();
        filePath = file.value("path").toString();
        QJsonArray uri = file.value("uris").toArray();
        for(int j = 0; j < uri.size(); ++j) {
            QJsonObject uriObject = uri[j].toObject();
            fileUri = uriObject.value("uri").toString();
        }
    }

    QString gId = result.value("gid").toString();

    long totalLength = result.value("totalLength").toString().toLong();         //
                                                                                // 字节
    long completedLength = result.value("completedLength").toString().toLong(); //
                                                                                // 字节
    long downloadSpeed = result.value("downloadSpeed").toString().toLong();     //
                                                                                // 字节/每秒
    QString fileName = getFileName(filePath);
    QString statusStr = result.value("status").toString();

    int percent = 0;
    int status = 0;

    if((completedLength != 0) && (totalLength != 0)) {
        double temp_percent = completedLength * 100.0 / totalLength;
        percent = temp_percent;
        if((percent < 0) || (percent > 100)) {
            percent = 0;
        }
        if(completedLength == totalLength) {
            statusStr = "complete";
        }
    }

    if(statusStr == "active") {
        status = Global::Status::Active;
    } else if(statusStr == "waiting") {
        status = Global::Status::Waiting;
    } else if(statusStr == "paused") {
        status = Global::Status::Paused;
    } else if(statusStr == "error") {
        status = Global::Status::Error;
        dealNotificaitonSettings(statusStr, fileName);
    } else if(statusStr == "complete") {
        status = Global::Status::Complete;

        //下载文件为种子文件
        if(fileName.endsWith(".torrent")) {
            emit signalAutoDownloadBt(filePath);
        }

        //下载文件为磁链种子文件
        QString infoHash = result.value("infoHash").toString();
        if(filePath.startsWith("[METADATA]")) {
            QString dir = result.value("dir").toString();

            emit signalAutoDownloadBt(dir + "/" + infoHash + ".torrent");
            fileName = infoHash + ".torrent";
        }

        //
        dealNotificaitonSettings(statusStr, fileName);
    } else if(statusStr == "removed") {
        status = Global::Status::Removed;
    }

    DataItem *data = m_pDownLoadingTableView->getTableModel()->find(taskId);
    if(data == nullptr) {
        return;
    }
    data->gid = gId;
    data->totalLength = formatFileSize(totalLength);
    data->completedLength = formatFileSize(completedLength);
    data->speed = (downloadSpeed != 0) ? formatDownloadSpeed(downloadSpeed) : "0kb/s";

    if(bittorrent.isEmpty()) {
        if(!fileName.isEmpty() && (data->fileName != fileName)) {
            data->fileName = fileName;
        }

        //                if(data->fileName==QObject::tr("Unknown"))
        //                {
        //                    data->fileName = (fileName.isEmpty()) ?
        // Global::UNKNOWN : fileName;
        //                }
        data->status = status;
    } else {
        // data->fileName = (bittorrent_name.isEmpty()) ? Global::UNKNOWN :
        // bittorrent_name;
        if(mode == "multi") {
            filePath = bittorrent_dir + "/" + bittorrent_name;
        }
        if((totalLength != 0) && (totalLength == completedLength)) {
            data->status = Complete;
            dealNotificaitonSettings("complete", filePath);
        } else {
            data->status = status;
        }

        fileUri = "";
    }
    data->percent = percent;
    data->total = totalLength;
    if(filePath != "") {
        data->savePath = filePath;
    } else {
        data->savePath = getDownloadSavepathFromConfig();
    }

    data->url = fileUri;
    data->time = "";

    if((totalLength != completedLength) && (totalLength != 0) &&
       (data->status == Global::Status::Active)) {
        QTime t(0, 0, 0);
        t = t.addSecs((totalLength - completedLength * 1.0) / downloadSpeed);
        data->time = t.toString("mm:ss");
    } else if((totalLength == 0) && (data->status == Global::Status::Active)) {
        data->time = ("--:--");
    } else {
        if(data->time == "") {
            data->time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }

        //              updatetimer->stop();
    }
    S_Task  task;
    S_Task get_task;
    DBInstance::getTaskByID(data->taskId, get_task);
    if(get_task.m_task_id != "") {
        if(get_task.m_url != "") {
            data->url = get_task.m_url;
        }
    }

    m_pDownLoadingTableView->update();
    m_pDownLoadingTableView->reset();
    S_Task_Status  task_status;
    S_Task_Status get_task_status;
    DBInstance::getTaskStatusById(data->taskId, get_task_status);

    QDateTime get_time = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
    S_Task_Status *save_task_status = new S_Task_Status(data->taskId,
                                                                            data->status,
                                                                            get_time,
                                                                            data->completedLength,
                                                                            data->speed,
                                                                            data->totalLength,
                                                                            data->percent,
                                                                            data->total,
                                                                            get_time);
    if(get_task_status.m_task_id != "") {
        DBInstance::addTaskStatus(*save_task_status);
    } else {
        if(get_task_status.m_download_status != data->status) {
            DBInstance::updateTaskStatusById(*save_task_status);
        }
    }
    refreshTableView(m_iCurrentListviewRow);
    if((data->status == Complete) && (this->m_searchContent != "")) {
        slotSearchEditTextChanged(this->m_searchContent);
    }
}

void MainFrame::aria2MethodShutdown(QJsonObject json)
{
    QString result = json.value("result").toString();
    if(result == "OK") {
        m_bShutdownOk = true;
        qDebug() << "close downloadmanager";
        this->close();
        DApplication::exit();
    }
}

void MainFrame::aria2MethodGetFiles(QJsonObject json)
{
    QString   id = json.value("id").toString();
    DataItem *data = m_pDownLoadingTableView->getTableModel()->find(id);
    if(data == nullptr) {// id等于gid
        data = new DataItem();
        QJsonArray  ja = json.value("result").toArray();
        QJsonObject jo = ja.at(0).toObject();
        data->totalLength = jo.value("length").toString().toLong(); // 文件大小
        data->savePath = jo.value("path").toString();               //下载路径，带文件名
        data->fileName = data->savePath.mid(data->savePath.lastIndexOf('/') + 1);
        QJsonArray uris = jo.value("uris").toArray();
        data->url = uris.at(0).toObject().value("uri").toString();  //下载链接
        data->taskId = id;
        m_pDownLoadingTableView->getTableModel()->append(data);
    }
    m_pDownLoadingTableView->reset();
    refreshTableView(m_iCurrentListviewRow);
}

void MainFrame::aria2MethodUnpause(QJsonObject json)
{
    QString gId = json.value("result").toString();
    QString taskId = json.value("id").toString();

    DataItem *data = m_pDownLoadingTableView->getTableModel()->find(taskId);
    if(data != nullptr) {
        data->status = Global::Status::Active;
        refreshTableView(m_iCurrentListviewRow);
    }
}

void MainFrame::aria2MethodForceRemove(QJsonObject json)
{
    QString id = json.value("id").toString();
    if(id.startsWith("REDOWNLOAD_")) { // 重新下载前的移除完成后
        QStringList sp = id.split("_");
        QString     taskId = sp.at(2);
        int rd = sp.at(1).toInt();
        QThread::msleep(500);
        emit signalRedownload(taskId, rd);
    }
}

void MainFrame::dealNotificaitonSettings(QString statusStr, QString fileName)
{
    // 获取免打扰模式值
    QVariant undisturbed_mode_switchbutton = m_pSettings->m_pSettings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    if(undisturbed_mode_switchbutton.toBool()) {
        bool topStatus = this->isTopLevel();
        bool maxStatus = this->isMaximized();
        if(topStatus && maxStatus) {
            return;
        }
    }

    QVariant afterDownloadPlayTone = m_pSettings->m_pSettings->getOption("noticeReminder.remind.afterDownloadPlayTone");
    if(afterDownloadPlayTone.toBool()) {
        //QSound::play(":/resources/wav/downloadfinish.wav");
    } else {
        qDebug() << " not in select down load finsh wav" << endl;
    }

    QVariant downloadInfoNotify = m_pSettings->m_pSettings->getOption("noticeReminder.remind.downloadInfoNotify");
    if(downloadInfoNotify.toBool()) {
        QProcess *p = new QProcess;
        QString   showInfo;
        if(statusStr == "error") {
            showInfo = fileName + tr(" download error");
        } else {
            showInfo = fileName + tr(" download complete");
        }
        p->start("notify-send", QStringList() << showInfo);
        p->waitForStarted();
        p->waitForFinished();
    }
}


void MainFrame::slotRpcSuccess(QString method, QJsonObject json)
{
    if((method == ARIA2C_METHOD_ADD_URI)
       || (method == ARIA2C_METHOD_ADD_TORRENT)
       || (method == ARIA2C_METHOD_ADD_METALINK)) {
        aria2MethodAdd(json);
    } else if(method == ARIA2C_METHOD_TELL_STATUS) {
        aria2MethodStatusChanged(json);
    } else if(method == ARIA2C_METHOD_SHUTDOWN) {
        aria2MethodShutdown(json);
    } else if(method == ARIA2C_METHOD_GET_FILES) {
        aria2MethodGetFiles(json);
    } else if(method == ARIA2C_METHOD_UNPAUSE) {
        aria2MethodUnpause(json);
    }
    else if(method == ARIA2C_METHOD_FORCE_REMOVE) {
        aria2MethodForceRemove(json);
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
