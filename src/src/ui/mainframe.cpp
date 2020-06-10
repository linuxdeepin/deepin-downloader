/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainframe.cpp
 *
 * @brief 主界面类
 *
 * @date 2020-06-09 09:41
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
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

#include "mainframe.h"
#include <DMenu>
#include <DLabel>
#include <DTitlebar>

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
#include <QDesktopServices>
#include <QDebug>

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
#include "btinfodialog.h"
#include "../database/dbinstance.h"

using namespace Global;

MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{
    init();
    initAria2();
    initConnection();
    initTabledata();
    setPaletteType();
}

void MainFrame::init()
{
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
    m_pLeftWidget->setFixedWidth(132);
    pLeftLayout->setContentsMargins(10, 0, 10, 0);

    m_pRight_Widget = new QWidget;
    m_pNotaskWidget = new QWidget;
    m_pNotaskWidget->setAutoFillBackground(true);
    m_pNotaskWidget->setPalette(p);

    QVBoxLayout *pNoTask_WidgetLayout = new QVBoxLayout(m_pNotaskWidget);
    pNoTask_WidgetLayout->setContentsMargins(10, 0, 0, 0);
    m_pNotaskLabel = new Dtk::Widget::DLabel();
    m_pNotaskLabel->setForegroundRole(DPalette::PlaceholderText);
    QFont lableFont;
    lableFont.setPointSize(15);
    lableFont.setBold(QFont::DemiBold);
    lableFont.setFamily("T5");
    QPalette font_p;
    QColor   color = QColor(192, 198, 212, 76);
    font_p.setColor(QPalette::WindowText, color);
    m_pNotaskLabel->setFont(lableFont);
    m_pNotaskLabel->setText(tr("No download tasks"));
    m_pNotaskLabel->setAlignment(Qt::AlignHCenter);

    pNoTask_WidgetLayout->addWidget(m_pNotaskLabel);
    m_pNotaskTipLabel = new QLabel();
    QFont noTask_tip_Label_font;
    noTask_tip_Label_font.setPointSize(13);
    m_pNotaskTipLabel->setFont(noTask_tip_Label_font);
    QPalette noTask_tip_Label_p;
    noTask_tip_Label_p.setColor(QPalette::WindowText, QColor(65, 77, 104, 70));
    m_pNotaskTipLabel->setText(tr("Click + to create new task"));
    m_pNotaskTipLabel->setAlignment(Qt::AlignHCenter);
    m_pNotaskTipLabel->setPalette(noTask_tip_Label_p);
    pNoTask_WidgetLayout->addWidget(m_pNotaskTipLabel);
    pNoTask_WidgetLayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_pRight_Widget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);
    m_pRightStackwidget = new QStackedWidget(this);

    QPalette p_task_num;
    p_task_num.setBrush(this->backgroundRole(), QBrush(QColor(255, 255, 255, 178)));
    m_pTaskNumWidget = new QWidget;
    m_pTaskNumWidget->setFixedHeight(30);
    m_pTaskNumWidget->setAutoFillBackground(true);
    m_pTaskNumWidget->setPalette(p_task_num);
    QHBoxLayout *Task_Num_WidgetLayout = new QHBoxLayout(m_pTaskNumWidget);
    Task_Num_WidgetLayout->setMargin(0);
    m_pTaskNum = new QLabel(tr("0 task"));
    m_pTaskNum->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pTaskNum->setPalette(noTask_tip_Label_p);
    Task_Num_WidgetLayout->addWidget(m_pTaskNum);
    m_pRightStackwidget->addWidget(m_pDownLoadingTableView);
    m_pRightStackwidget->addWidget(m_pRecycleTableView);
    pRightLayout->addWidget(m_pRightStackwidget);
    pRightLayout->addWidget(m_pNotaskWidget);
    pRightLayout->addWidget(m_pTaskNumWidget);
    pRightLayout->setSpacing(0);

    pMainHLayout->addWidget(m_pLeftWidget);
    pMainHLayout->addWidget(m_pRight_Widget);

    m_pLeftList = new DListView;
    m_pLeftList->setItemSpacing(0);
    m_pLeftList->setItemSize(QSize(112, 40));
    m_pLeftList->setItemMargins(QMargins(10, 2, 5, 2));
    m_pLeftList->setIconSize(QSize(14, 14));
    QFont font;
    font.setFamily("Source Han Sans");
    font.setPixelSize(14);
    m_pLeftList->setFont(font);
    QStandardItemModel *pLeftList_model = new QStandardItemModel(this);

    m_pDownloading_item = new QStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("Downloading"));

    // QFont f = m_pDownloading_item->font();
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
    m_pLeftList->setModel(pLeftList_model);
    pLeftLayout->addWidget(m_pLeftList, 0);

    // updatetimer = new QTimer(this);
    m_pLeftList->setCurrentIndex(pLeftList_model->index(0, 0));
    m_pDownLoadingTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pRecycleTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_pRightStackwidget->setCurrentIndex(0);

    m_pClipboard = new ClipboardTimer; // 获取当前剪切板

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

    QMenu *pTrayMenu = new QMenu(this);
    pTrayMenu->addAction(pShowMainAct);
    pTrayMenu->addAction(pNewDownloadAct);
    pTrayMenu->addAction(pStartAllAct);
    pTrayMenu->addAction(pPauseAllAct);
    pTrayMenu->addAction(pQuitAct);

    // 连接信号与槽
    connect(pShowMainAct,    &QAction::triggered, [ = ]() {
        this->show();
    });
    connect(pNewDownloadAct, &QAction::triggered, [ = ]() {
        createNewTask("");
    });
    connect(pQuitAct,      &QAction::triggered,         this, &MainFrame::onTrayQuitClick);
    connect(m_pSystemTray, &QSystemTrayIcon::activated, this, &MainFrame::onActivated);
    m_pSystemTray->setContextMenu(pTrayMenu);
    m_pSystemTray->show();
}

void MainFrame::initConnection()
{
    connect(m_pDownLoadingTableView,
            &TableView::signalHeaderStatechanged,
            this,
            &MainFrame::getHeaderStatechanged);
    connect(m_pDownLoadingTableView,
            &TableView::customContextMenuRequested,
            this,
            &MainFrame::onContextMenu);
    connect(m_pDownLoadingTableView,
            &TableView::pressed,
            this,
            &MainFrame::onTableItemSelected);
    connect(m_pDownLoadingTableView,
            &TableView::signalRedownload,
            this,
            &MainFrame::onRedownload,
            Qt::QueuedConnection);
    connect(m_pDownLoadingTableView->getTableModel(),
            &TableModel::signalCheckChange,
            this,
            &MainFrame::onCheckChanged);

    connect(m_pRecycleTableView,
            &TableView::signalHeaderStatechanged,
            this,
            &MainFrame::getHeaderStatechanged);
    connect(m_pRecycleTableView,
            &TableView::customContextMenuRequested,
            this,
            &MainFrame::onContextMenu);
    connect(m_pRecycleTableView,
            &TableView::pressed,
            this,
            &MainFrame::onTableItemSelected);
    connect(m_pRecycleTableView,
            &TableView::signalRedownload,
            this,
            &MainFrame::onRedownload,
            Qt::QueuedConnection);
    connect(m_pRecycleTableView->getTableModel(),
            &TableModel::signalCheckChange,
            this,
            &MainFrame::onCheckChanged);

    connect(this,
            &MainFrame::switchTableSignal,
            m_pDownLoadingTableView,
            &TableView::signalClearHeaderCheck);
    connect(this,
            &MainFrame::switchTableSignal,
            m_pRecycleTableView,
            &TableView::signalClearHeaderCheck);


    connect(m_pSettingAction,
            &QAction::triggered,
            this,
            &MainFrame::onSettingsMenuClicked);
    connect(m_pClipboard,
            &ClipboardTimer::sendClipboardText,
            this,
            &MainFrame::onClipboardDataChanged);
    connect(m_pClipboard,
            &ClipboardTimer::sentBtText,
            this,
            &MainFrame::onClipboardDataForBt,
            Qt::UniqueConnection);
    connect(m_pLeftList,
            &DListView::clicked,
            this,
            &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::paletteTypeChanged,
            this,
            &MainFrame::onPalettetypechanged);
    connect(m_pUpdatetimer,
            &QTimer::timeout,
            this,
            &MainFrame::updateMainUI);

    connect(m_pToolBar,
            &TopButton::newDownloadBtnClicked,
            this,
            &MainFrame::onNewBtnClicked);
    connect(m_pToolBar,
            &TopButton::getSearchEditTextChange,
            this,
            &MainFrame::onSearchEditTextChanged);
    connect(m_pToolBar,
            &TopButton::startDownloadBtnClicked,
            this,
            &MainFrame::onStartDownloadBtnClicked);
    connect(m_pToolBar,
            &TopButton::pauseDownloadBtnClicked,
            this,
            &MainFrame::onPauseDownloadBtnClicked);
    connect(m_pToolBar,
            &TopButton::deleteDownloadBtnClicked,
            this,
            &MainFrame::onDeleteDownloadBtnClicked);
    connect(this,
            &MainFrame::tableChanged,
            m_pToolBar,
            &TopButton::getTableChanged);
    connect(this,
            &MainFrame::signalRedownload,
            this,
            &MainFrame::onRedownload,
            Qt::QueuedConnection);

    connect(Settings::getInstance(),
            &Settings::downloadSettingsChanged,
            this,
            &MainFrame::downloadLimitPeriod);
}

void MainFrame::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::Trigger) {
        if(this->isHidden()) {
            // 恢复窗口显示
            this->show();
            this->setWindowState(Qt::WindowActive);
            this->activateWindow();

            // if(this->minimumAct) {
            this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

            // }
        } else {
            this->hide();
        }
    }
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    if(Settings::getInstance()->getIsShowTip()) {
        MessageBox *msg = new MessageBox();
        connect(msg, &MessageBox::signalCloseConfirm, this, &MainFrame::onMessageBoxConfirmClick);
        msg->setExit();
        msg->exec();
    } else {
        onMessageBoxConfirmClick();
    }
    event->ignore();
}

void MainFrame::createNewTask(QString url)
{
    static newTaskWidget *pNewTaskWidget = new newTaskWidget();

    pNewTaskWidget->setUrl(url);
    connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewDowloadUrl, Qt::UniqueConnection);
    connect(pNewTaskWidget, &newTaskWidget::newDownLoadTorrent, this, &MainFrame::getNewDownloadTorrent, Qt::UniqueConnection);
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
    if(Settings::getInstance()->getCloseMainWindowSelected()) {
        onTrayQuitClick();
    } else {
        hide();
    }
}

MainFrame::~MainFrame()
{
}

void MainFrame::initAria2()
{
    Aria2RPCInterface::Instance()->init(); // 启动Aria2RPCInterface::Instance()
    connect(Aria2RPCInterface::Instance(), SIGNAL(signalRPCSuccess(QString,QJsonObject)), this,
            SLOT(onRpcSuccess(QString,QJsonObject)));
    connect(Aria2RPCInterface::Instance(), SIGNAL(signalRPCError(QString,QString,int)), this,
            SLOT(onRpcError(QString,QString,int)));

    qDebug() << "MainFrame initAria2 Finished";
}

void MainFrame::initTabledata()
{
    QList<S_Task> list;
    S_Task tbTask;
    int    task_num = 0;

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
            data->percent = taskStatus.m_percent;
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
                    m_pNotaskWidget->hide();
                    QVariant autostart_unfinished_task_switchbutton =
                        Settings::getInstance()->getAutostartUnfinishedTaskState();
                    m_pDownLoadingTableView->getTableModel()->append(data);
                    if(autostart_unfinished_task_switchbutton.toBool()) {
                        QString savePath = getDownloadSavepathFromConfig();
                        QMap<QString, QVariant> opt;
                        opt.insert("dir", savePath);
                        S_Url_Info *url_info = new S_Url_Info;
                        S_Url_Info  getUrlInfo;
                        DBInstance::getUrlById(data->taskId, getUrlInfo);
                        if(getUrlInfo.m_taskId != "") {
                            if(getUrlInfo.m_downloadType == "torrent") {
                                QString select_num = getUrlInfo.m_selectedNum;
                                QMap<QString, QVariant> opt;
                                opt.insert("dir",           savePath);
                                opt.insert("select-file", select_num);
                                if(!QFile(getUrlInfo.m_seedFile).exists()) {
                                    showWarningMsgbox(tr("seed file not exists or broken;"));
                                    qDebug() << "seed file not exists or broken;";
                                } else {
                                    Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile,
                                                                              opt,
                                                                              getUrlInfo.m_taskId);
                                    if(m_pUpdatetimer->isActive() == false) {
                                        m_pUpdatetimer->start(2 * 1000);
                                    }
                                }
                            }
                        } else {
                            downloadLimitPeriod();
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
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
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
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
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
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
        }
    }
    m_pTaskNum->setText(active_num);
}

void MainFrame::setPaletteType()
{
    m_pLeftList->setPalette(DGuiApplicationHelper::instance()->applicationPalette());

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

        m_pNotaskTipLabel->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        QPalette tableview_palette;

        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());

        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_pNotaskWidget->setPalette(tableview_palette);
        m_pTaskNumWidget->setPalette(tableview_palette);
        m_pDownloading_item->setIcon(QIcon::fromTheme("dcc_list_downloading_dark"));
        m_pDownloadFinish_item->setIcon(QIcon::fromTheme("dcc_print_done_dark"));
        m_pRecycle_item->setIcon(QIcon::fromTheme("dcc_list_delete_dark"));
        QPalette noTask_tip_Label_p;
        noTask_tip_Label_p.setBrush(QPalette::WindowText,
                                    DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_pTaskNum->setPalette(noTask_tip_Label_p);
    } else if(DGuiApplicationHelper::instance()->themeType() == 1) {
        QPalette p;
        p.setColor(QPalette::Background, QColor(255, 255, 255));
        QPalette tableview_palette;
        tableview_palette.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());
        m_pLeftWidget->setPalette(p);
        m_pNotaskWidget->setPalette(tableview_palette);
        m_pDownLoadingTableView->setPalette(tableview_palette);
        m_pRecycleTableView->setPalette(tableview_palette);
        m_pTaskNumWidget->setPalette(tableview_palette);

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
        m_pTaskNum->setPalette(noTask_tip_Label_p);
    }
}

void MainFrame::onSettingsMenuClicked()
{
    DSettingsDialog *pSettingsDialog = new DSettingsDialog(this);

    pSettingsDialog->widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);

    //
    //  pSettingsDialog->widgetFactory()->registerWidget("downloadtraysetting",
    // Settings::createDownloadTraySettingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloaddiskcachesetting",
                                                     Settings::createDownloadDiskCacheSettiingHandle);
    pSettingsDialog->widgetFactory()->registerWidget("downloadspeedlimitsetting",
                                                     Settings::createDownloadSpeedLimitSettiingHandle);
    pSettingsDialog->updateSettings("Settings", Settings::getInstance()->m_pSettings);
    pSettingsDialog->exec();
    delete pSettingsDialog;
    Settings::getInstance()->m_pSettings->sync();
}

void MainFrame::onClipboardDataChanged(QString url)
{
    createNewTask(url);
}

void MainFrame::onClipboardDataForBt(QString url)
{
    QString _savePath = Settings::getInstance()->getDownloadSavePath();

    BtInfoDialog *pBt = new BtInfoDialog(url, _savePath); // torrent文件路径
    int ret = pBt->exec();

    if(ret == QDialog::Accepted) {
        QMap<QString, QVariant> opt;
        QString infoName;
        QString infoHash;
        pBt->getBtInfo(opt, infoName, infoHash);
        this->getNewDownloadTorrent(url, opt, infoName, infoHash);
    }
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
            // connect(m_pDownLoadingTableView,
            // SIGNAL(doubleClicked(QModelIndex)), this,
            // SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(30);
            m_pNotaskWidget->show();
            m_pNotaskLabel->setText(tr("No finished tasks"));
            m_pNotaskTipLabel->hide();
        } else {
            // disconnect(m_pDownLoadingTableView,
            // SIGNAL(doubleClicked(QModelIndex)), this,
            // SLOT(get_doubleClicked(QModelIndex)));
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
            m_pNotaskLabel->setText(tr("No download tasks"));
            m_pNotaskWidget->show();
            m_pNotaskTipLabel->show();
        }
    } else {
        m_pRightStackwidget->setCurrentIndex(1);
        m_pNotaskWidget->show();
        m_pNotaskLabel->setText(tr("No deleted tasks"));
        m_pNotaskTipLabel->hide();
    }
    clearTableItemCheckStatus();

    emit switchTableSignal();

    setTaskNum(m_iCurrentListviewRow);

    // search begin
    onSearchEditTextChanged(this->m_pToolBar->getSearchText());
    emit tableChanged(index.row());
}

void MainFrame::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    setPaletteType();
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

    urlList = urlList.toSet().toList(); // url去重
    bool isExitsUrl = false;
    QStringList _sameUrl;

    // 判断url是否在数据中已存在
    for(int i = 0; i < urlList.size(); i++) {
        DBInstance::isExistUrl(urlList[i], isExitsUrl);
        if(isExitsUrl) {
            _sameUrl.append(urlList[i]);
            urlList.removeAt(i);
            --i;
        }
    }
    if(!_sameUrl.isEmpty()) {
        QString warning_msg = tr("has ") + QString::number(_sameUrl.size()) + tr(" the same download");
        showWarningMsgbox(warning_msg, _sameUrl.size(), _sameUrl);
    }
    if(urlList.isEmpty()) {
        qDebug() << "url is NULl";
        return;
    }

    // 将url加入数据库和aria
    S_Task _task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    for(int i = 0; i < urlList.size(); i++) {
        _task = getUrlToName(urlList[i], savePath);
        DBInstance::addTask(_task);
        Aria2RPCInterface::Instance()->addNewUri(_task.m_url, savePath, _task.m_taskId);
    }

    m_pNotaskWidget->hide();

    // 定时器打开
    if(m_pUpdatetimer->isActive() == false) {
        m_pUpdatetimer->start(2 * 1000);
    }
}

S_Task MainFrame::getUrlToName(QString url, QString savePath)
{
    // 获取url文件名
    QString fileName;

    if(url.startsWith("magnet") && url.contains("&")) {
        fileName = url.split("&")[0];
        if(fileName.contains("btih:")) {
            fileName = fileName.split("btih:")[1] + ".torrent";
        } else {
            fileName = url.right(40);
        }
    } else {
        fileName = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
    }

    // 对url进行转码
    if(!fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        const QByteArray _byte = fileName.toLatin1();
        QString decode = QUrl::fromPercentEncoding(_byte);
        if(decode.contains("?")) {
            decode = decode.split("?")[0];
        }
        fileName = decode;
    }

    S_Task task;
    task.m_taskId = QUuid::createUuid().toString();
    task.m_gid = "";
    task.m_gidIndex = 0;
    task.m_url = url;
    task.m_downloadPath = savePath + "/" + fileName;
    task.m_downloadFilename = fileName;
    task.m_createTime = QDateTime::currentDateTime();
    return task;
}

void MainFrame::onContextMenu(const QPoint &pos)
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
            connect(pActionStart, &QAction::triggered, this, &MainFrame::onStartDownloadBtnClicked);
        }
        if(activeCount > 0) {
            QAction *pActionPause = new QAction();
            pActionPause->setText(tr("Pause"));
            delmenlist->addAction(pActionPause);
            connect(pActionPause, &QAction::triggered, this, &MainFrame::onPauseDownloadBtnClicked);
        }
        if(errorCount > 0) {
            QAction *pActionredownload = new QAction();
            pActionredownload->setText(tr("Download again"));
            delmenlist->addAction(pActionredownload);
            connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        }
        delmenlist->addSeparator();
    }
    if(m_iCurrentListviewRow == 2) {
        QAction *returned_to_origin = new QAction();
        returned_to_origin->setText(tr("Restore"));
        delmenlist->addAction(returned_to_origin);
        connect(returned_to_origin, &QAction::triggered, this, &MainFrame::onReturnOriginActionTriggered);
    }
    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 2)) {
        QAction *pActionopenFile = new QAction();
        pActionopenFile->setText(tr("Open"));
        delmenlist->addAction(pActionopenFile);
        connect(pActionopenFile, &QAction::triggered, this, &MainFrame::onOpenFileActionTriggered);
    }
    QAction *pActionopenFoler = new QAction();
    pActionopenFoler->setText(tr("Open folder"));
    delmenlist->addAction(pActionopenFoler);
    connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
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
            connect(pAction_rename, &QAction::triggered, this, &MainFrame::onRenameActionTriggered);
        }
    }
    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 2)) {
        QAction *pActionredownload = new QAction();
        pActionredownload->setText(tr("Download again"));
        delmenlist->addAction(pActionredownload);
        connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
    }
    QAction *pAction_copy_download_url = new QAction();
    pAction_copy_download_url->setText(tr("Copy download link"));
    delmenlist->addAction(pAction_copy_download_url);
    delmenlist->addSeparator();
    connect(pAction_copy_download_url, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
    if((m_iCurrentListviewRow == 0) || (m_iCurrentListviewRow == 1)) {
        QAction *pActiondel_downloading = new QAction();
        pActiondel_downloading->setText(tr("Delete"));
        delmenlist->addAction(pActiondel_downloading);
        connect(pActiondel_downloading, &QAction::triggered, this, &MainFrame::onDelActionTriggered);
    }
    QAction *pAction_delete_permanently = new QAction();
    pAction_delete_permanently->setText(tr("Permanently delete"));
    delmenlist->addAction(pAction_delete_permanently);
    connect(pAction_delete_permanently, &QAction::triggered, this, &MainFrame::onDeletePermanentActionTriggered);
    if(m_iCurrentListviewRow == 2) {
        QAction *pAction_clear_recycle = new QAction();
        pAction_clear_recycle->setText(tr("Empty"));
        delmenlist->addAction(pAction_clear_recycle);
        connect(pAction_clear_recycle, &QAction::triggered, this, &MainFrame::onClearRecyleActionTriggered);
    }

    delmenlist->exec(QCursor::pos());
    delete  delmenlist;
}

void MainFrame::onCheckChanged(bool checked, int flag)
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

void MainFrame::onSearchEditTextChanged(QString text)
{
    m_SearchContent = text;
    m_pDownLoadingTableView->searchEditTextChanged(text);
    m_pRecycleTableView->searchEditTextChanged(text);
    setTaskNum(m_iCurrentListviewRow);
}

void MainFrame::getNewDownloadTorrent(QString btPath, QMap<QString, QVariant> opt, QString infoName, QString infoHash)
{
    QString selectedNum = opt.value("select-file").toString();

    if(selectedNum.isNull()) {
        qDebug() << "select is null";
        return;
    }

    // 数据库是否已存在相同的地址
    QList<S_Url_Info> urlList;
    DBInstance::getAllUrl(urlList);
    for(int i = 0; i < urlList.size(); i++) {
        if((urlList[i].m_infoHash == infoHash) && (urlList[i].m_selectedNum == selectedNum)) {
            qDebug() << "has the same download!";
            return;
        }
    }

    // 将任务添加如task表中
    S_Task  task;
    QString strId = QUuid::createUuid().toString();
    task.m_taskId = strId;
    task.m_gid = "";
    task.m_gidIndex = 0;
    task.m_url = "";
    task.m_downloadPath = "";
    task.m_downloadFilename = infoName;
    task.m_createTime = QDateTime::currentDateTime();
    DBInstance::addTask(task);

    // 将任务添加如url_info表中
    S_Url_Info urlInfo;
    urlInfo.m_taskId = strId;
    urlInfo.m_url = "";
    urlInfo.m_downloadType = "torrent";
    urlInfo.m_seedFile = btPath;
    urlInfo.m_selectedNum = selectedNum;
    urlInfo.m_infoHash = "";
    DBInstance::addUrl(urlInfo);

    // 开始下载
    Aria2RPCInterface::Instance()->addTorrent(btPath, opt, strId);

    // 定时器打开
}

void MainFrame::onRedownload(QString taskId, int rd)
{
    if((rd == 0) || (rd == 1)) {
        DataItem *data = m_pDownLoadingTableView->getTableModel()->find(taskId);
        if(data == nullptr) {
            return;
        }
        QString url = data->url;
        QString aria_temp_file = data->savePath + ".aria2";
        QString taskId = data->taskId;
        if(!data->savePath.isEmpty()) {
            QFileInfo fi(data->savePath);
            if(fi.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
                QDir tar(data->savePath);
                tar.removeRecursively();
            } else {
                QFile::remove(data->savePath);
            }
            if(QFile::exists(aria_temp_file)) {
                QThread::sleep(1);
                QFile::remove(aria_temp_file);
            }
        }

        QString file_path = data->savePath;
        QString file_name = data->fileName;
        QString default_savePath = getDownloadSavepathFromConfig();
        QString save_path;

        if(default_savePath != data->savePath) {
            int name_length = file_name.size();
            int file_path_length = file_path.size();
            int folder_path_length = file_path_length - name_length - 1;
            save_path = file_path.left(folder_path_length);
        } else {
            save_path = default_savePath;
        }


        m_pDownLoadingTableView->getTableModel()->removeItem(data);

        // QString save_path=get_download_savePath_from_config();

        downloadLimitPeriod();

        DBInstance::delTask(taskId);
        S_Url_Info getUrlInfo;
        DBInstance::getUrlById(taskId, getUrlInfo);
        if(getUrlInfo.m_taskId != "") {
            if(getUrlInfo.m_downloadType == "torrent") {
                QMap<QString, QVariant> opt;
                opt.insert("dir",          save_path);
                opt.insert("select-file", getUrlInfo.m_selectedNum);

                // aria2c->addTorrent(get_url_info->seedFile,opt,get_url_info->task_id);
                S_Task addTask(getUrlInfo.m_taskId,
                               "",
                               0,
                               "",
                               "",
                               file_name,
                               QDateTime::currentDateTime());
                DBInstance::addTask(addTask);
                Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
            }
        } else {
            QUuid   id = QUuid::createUuid();
            QString strId = id.toString();
            QMap<QString, QVariant> opt;
            opt.insert("dir", save_path);
            Aria2RPCInterface::Instance()->addUri(url, opt, strId);

            QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
            if(!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                const QByteArray filename_byte = filename.toLatin1();
                QString filename_decode = QUrl::fromPercentEncoding(filename_byte);
                filename = filename_decode;
            }

            S_Task addTask(strId, "", 0, url, file_path, filename, QDateTime::currentDateTime());
            DBInstance::addTask(addTask);
        }
    } else {
        DelDataItem *data = m_pRecycleTableView->getTableModel()->find(taskId, 2);
        if(data == nullptr) {
            return;
        }
        QString url = data->url;
        QString taskId = data->taskId;
        QString aria_temp_file = data->savePath + ".aria2";
        if(!data->savePath.isEmpty()) {
            QFileInfo fi(data->savePath);
            if(fi.isDir() && data->savePath.contains(data->fileName) && !data->fileName.isEmpty()) {
                QDir tar(data->savePath);
                tar.removeRecursively();
            } else {
                QFile::remove(data->savePath);
            }
            if(QFile::exists(aria_temp_file)) {
                QFile::remove(aria_temp_file);
            }
        }
        QString file_path = data->savePath;
        QString file_name = data->fileName;
        QString default_savePath = getDownloadSavepathFromConfig();
        QString save_path;
        if(default_savePath != data->savePath) {
            int name_length = file_name.size();
            int file_path_length = file_path.size();
            int folder_path_length = file_path_length - name_length - 1;
            save_path = file_path.left(folder_path_length);
        } else {
            save_path = default_savePath;
        }


        m_pRecycleTableView->getTableModel()->removeItem(data);
        downloadLimitPeriod();

        // QString  save_path=get_download_savePath_from_config();
        DBInstance::delTask(taskId);
        QUuid   id = QUuid::createUuid();
        QString strId = id.toString();
        S_Url_Info getUrlInfo;
        DBInstance::getUrlById(taskId, getUrlInfo);
        if(getUrlInfo.m_taskId != "") {
            if(getUrlInfo.m_downloadType == "torrent") {
                if(!getUrlInfo.m_infoHash.isEmpty()) {
                    QFile::remove(getUrlInfo.m_infoHash + ".torrent");
                }
                QMap<QString, QVariant> opt;
                opt.insert("dir",          save_path);
                opt.insert("select-file", getUrlInfo.m_selectedNum);

                // aria2c->addTorrent(get_url_info->seedFile,opt,get_url_info->task_id);

                S_Task addTask(getUrlInfo.m_taskId,
                               "",
                               0,
                               "",
                               "",
                               file_name,
                               QDateTime::currentDateTime());
                DBInstance::addTask(addTask);
                Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile, opt, getUrlInfo.m_taskId);
            }
        } else {
            QMap<QString, QVariant> opt;
            opt.insert("dir", save_path);
            Aria2RPCInterface::Instance()->addUri(url, opt, strId);
            QString filename = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
            if(!filename.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
                const QByteArray filename_byte = filename.toLatin1();
                QString filename_decode = QUrl::fromPercentEncoding(filename_byte);
                filename = filename_decode;
            }
            S_Task addTask(strId, "", 0, url, file_path, filename, QDateTime::currentDateTime());
            DBInstance::addTask(addTask);
        }
    }
    updateMainUI();
    if(m_pUpdatetimer->isActive() == false) {
        m_pUpdatetimer->start(2 * 1000);
    }
}

void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
    MessageBox *msg = new MessageBox();

    msg->setWarings(title, tr("sure"), "", sameUrlCount, sameUrlList);
    msg->exec();
}

void MainFrame::showReloadMsgbox()
{
    MessageBox *msg = new MessageBox();

    // connect(msg,&MessageBox::ReDownload_sig,this,&MainWindow::get_Redowload_confirm_slot);
    QString title = tr("reload will delete this local file,Do you continue?");

    msg->setWarings(title, tr("sure"), tr("cancel"));
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);

        // 重新下载：通知aria2移除下载项

        if((m_iCurrentListviewRow == 0) || (m_iCurrentListviewRow == 1)) {
            for(int i = 0; i < m_reloadList.size(); i++) {
                DataItem *data = m_reloadList.at(i);
                Aria2RPCInterface::Instance()->forceRemove(data->gid,"REDOWNLOAD_"+ QString::number(m_iCurrentListviewRow)+ "_" + data->taskId);
                //Aria2RPCInterface::Instance()->forceRemove(data->gid, data->taskId);
            }
        } else {
            for(int i = 0; i < m_recycleReloadList.size(); i++) {
                DelDataItem *data = m_recycleReloadList.at(i);
                Aria2RPCInterface::Instance()->forceRemove(data->gid,"REDOWNLOAD_" + QString::number(m_iCurrentListviewRow) + "_" + data->taskId);
                //Aria2RPCInterface::Instance()->forceRemove(data->gid, data->taskId);
            }
        }
    }
}

void MainFrame::showDeleteMsgbox(bool permanently)
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::DeletedownloadSig, this, &MainFrame::onGetDeleteConfirm);
    msg->setDelete(permanently);
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }
}

void MainFrame::showRenameMsgbox()
{

}

void MainFrame::onAria2Remove(QString gId, QString id)
{
    Aria2RPCInterface::Instance()->remove(gId, id);
}

void MainFrame::onGetDeleteConfirm(bool ischecked, bool permanent)
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
        connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, this, &MainFrame::onAria2Remove);
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

        QList<DataItem> threadDeleteList;


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

            threadDeleteList.append(tempdata);
        }

        DeleteItemThread *pDeleteItemThread = new DeleteItemThread(threadDeleteList,
                                                                   m_pDownLoadingTableView,
                                                                   Aria2RPCInterface::Instance(),
                                                                   ifDeleteLocal,
                                                                   "download_delete");
        connect(pDeleteItemThread, &DeleteItemThread::signalAria2Remove, this, &MainFrame::onAria2Remove);
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

            S_Task_Status get_status;
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

                // Aria2RPCInterface::Instance()->purgeDownloadResult(data->gid);
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
    if(this->m_SearchContent != "") {
        onSearchEditTextChanged(m_SearchContent);
    }
    if(m_pUpdatetimer->isActive() == false) {
        m_pUpdatetimer->start(2 * 1000);
    }
}

QString   MainFrame::getDownloadSavepathFromConfig()
{
    QVariant downloadRadioGroup =
        Settings::getInstance()->m_pSettings->getOption("basic.downloadDirectory.downloadDirectoryFileChooser");

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
                        S_Url_Info  getUrlInfo;
                        DBInstance::getUrlById(selectList.at(i)->taskId, getUrlInfo);
                        if(getUrlInfo.m_taskId != "") {
                            if(getUrlInfo.m_downloadType == "torrent") {
                                QString select_num = getUrlInfo.m_selectedNum;
                                QMap<QString, QVariant> opt;
                                opt.insert("dir",          save_path);
                                opt.insert("select-file", select_num);
                                if(!QFile(getUrlInfo.m_seedFile).exists()) {
                                    showWarningMsgbox(tr("seed file not exists or broken;"));
                                    qDebug() << "seed file not exists or broken;";
                                } else {
                                    Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile,
                                                                              opt,
                                                                              getUrlInfo.m_taskId);
                                    if(m_pUpdatetimer->isActive() == false) {
                                        m_pUpdatetimer->start(2 * 1000);
                                    }
                                }
                            }
                        } else {
                            // deal_download_upload_limit_period();
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
                    S_Task_Status get_status;
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
    onDelActionTriggered();
}

void MainFrame::onRpcSuccess(QString method, QJsonObject json)
{
    if((method == ARIA2C_METHOD_ADD_URI)
       || (method == ARIA2C_METHOD_ADD_TORRENT)
       || (method == ARIA2C_METHOD_ADD_METALINK)) {
        m_pDownLoadingTableView->aria2MethodAdd(json, m_SearchContent);
    } else if(method == ARIA2C_METHOD_TELL_STATUS) {
        m_pDownLoadingTableView->aria2MethodStatusChanged(json, m_iCurrentListviewRow, m_SearchContent);
    } else if(method == ARIA2C_METHOD_SHUTDOWN) {
        m_pDownLoadingTableView->aria2MethodShutdown(json);
    } else if(method == ARIA2C_METHOD_GET_FILES) {
        m_pDownLoadingTableView->aria2MethodGetFiles(json, m_iCurrentListviewRow);
    } else if(method == ARIA2C_METHOD_UNPAUSE) {
        m_pDownLoadingTableView->aria2MethodUnpause(json, m_iCurrentListviewRow);
    } else if(method == ARIA2C_METHOD_FORCE_REMOVE) {
        m_pDownLoadingTableView->aria2MethodForceRemove(json);
    }
}

void MainFrame::onRpcError(QString method, QString id, int error)
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

void MainFrame::onTableItemSelected(const QModelIndex &selected)
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

void MainFrame::onDelActionTriggered()
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

void MainFrame::onRedownloadActionTriggered()
{
    int selected_count = 0;

    if((m_iCurrentListviewRow == 1) || (m_iCurrentListviewRow == 0)) {
        QList<DataItem *> selectList;
        m_reloadList.clear();
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(((m_iCurrentListviewRow == 1) && (selectList.at(i)->status == Complete)) ||
               ((m_iCurrentListviewRow == 0) && (selectList.at(i)->status == Error))) {
                if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                    DataItem *data = selectList.at(i);
                    m_reloadList.append(data);
                    ++selected_count;
                }
            }
        }
    } else {
        QList<DelDataItem *> selectList;
        m_recycleReloadList.clear();
        selectList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->status == Removed) {
                if((selectList.at(i)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                    DelDataItem *data = selectList.at(i);
                    m_recycleReloadList.append(data);
                    ++selected_count;
                }
            }
        }
    }
    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showReloadMsgbox();
    }
}

void MainFrame::onReturnOriginActionTriggered()
{
    int selected_count = 0;
    QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();

    for(int i = 0; i < recycle_list.size(); i++) {
        DelDataItem *data = recycle_list.at(i);
        if((data->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
            DataItem *returnTo_data = new DataItem;
            ++selected_count;
            if(data->completedLength == data->totalLength) {
                if(data->totalLength != "0B") {
                    returnTo_data->status = Global::Status::Complete;
                } else {
                    returnTo_data->status = Global::Status::Active;
                }
            } else {
                long completedLength = returnTo_data->completedLength.toLong();


                if((returnTo_data->completedLength != "0B") && (returnTo_data->totalLength != "0B")) {
                    returnTo_data->percent =  returnTo_data->completedLength.toLong() * 100.0 /
                                             returnTo_data->totalLength.toLong();

                    if((returnTo_data->percent < 0) || (returnTo_data->percent > 100)) {
                        returnTo_data->percent = 0;
                    }
                } else {
                    returnTo_data->percent = 0;
                }
            }
            returnTo_data->fileName = data->fileName;
            returnTo_data->savePath = data->savePath;
            returnTo_data->totalLength = data->totalLength;
            returnTo_data->completedLength = data->completedLength;
            returnTo_data->url = data->url;
            returnTo_data->gid = data->gid;
            returnTo_data->time = data->finishTime;
            returnTo_data->taskId = data->taskId;
            m_pDownLoadingTableView->getTableModel()->append(returnTo_data);
            if((data->completedLength != data->totalLength) || (data->totalLength == "0B")) {
                QMap<QString, QVariant> opt;
                returnTo_data->url = data->url;
                QString file_path = data->savePath;
                QString file_name = data->fileName;
                QString default_savePath = getDownloadSavepathFromConfig();
                QString save_path;

                if(default_savePath != data->savePath) {
                    int name_length = file_name.size();
                    int file_path_length = file_path.size();
                    int folder_path_length = file_path_length - name_length - 1;
                    save_path = file_path.left(folder_path_length);
                } else {
                    save_path = default_savePath;
                }


                opt.insert("dir", save_path);

                S_Url_Info *url_info = new S_Url_Info;
                S_Url_Info  get_url_info;
                DBInstance::getUrlById(returnTo_data->taskId, get_url_info);
                if(get_url_info.m_taskId != "") {
                    if(get_url_info.m_downloadType == "torrent") {
                        QString select_num = get_url_info.m_selectedNum;
                        QString seed_file_path = get_url_info.m_seedFile;

                        opt.insert("select-file", select_num);

                        if(!QFile(seed_file_path).exists()) {
                            showWarningMsgbox(tr("seed file not exists or broken;"));
                        } else {
                            Aria2RPCInterface::Instance()->addTorrent(seed_file_path, opt, get_url_info.m_taskId);
                            if(m_pUpdatetimer->isActive() == false) {
                                m_pUpdatetimer->start(2 * 1000);
                            }
                        }
                    }
                } else {
                    downloadLimitPeriod();
                    Aria2RPCInterface::Instance()->addUri(returnTo_data->url, opt, returnTo_data->taskId);
                    if(m_pUpdatetimer->isActive() == false) {
                        m_pUpdatetimer->start(2 * 1000);
                    }
                }
            }
            m_pRecycleTableView->getTableModel()->removeItem(data);
            setTaskNum(m_iCurrentListviewRow);
        }
    }
    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        // ToolBar禁用按钮联动：还原后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }
}

void MainFrame::onOpenFileActionTriggered()
{
    QList<DataItem *> selectList;
    QList<DelDataItem *> delList;
    int selected_count = 0;

    if(m_iCurrentListviewRow == 1) {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->status == Complete) {
                if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                    DataItem *data = selectList.at(i);
                    ++selected_count;
                    QString file_path = data->savePath;
                    QString file_name = data->fileName;
                    QString path = QString("file:///") + file_path;
                    bool    is_open = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                }
            }
        }
    } else {
        delList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < delList.size(); ++i) {
            if((delList.at(i)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                DelDataItem *data = delList.at(i);
                ++selected_count;
                QString file_path = data->savePath;
                QString file_name = data->fileName;
                QString path = QString("file:///") + file_path;
                bool    is_open = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
            }
        }
    }

    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    }
}

void MainFrame::onOpenFolderActionTriggered()
{
    QList<DataItem *> selectList;
    QList<DelDataItem *> delList;
    int selected_count = 0;

    if(m_iCurrentListviewRow == 2) {
        delList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int j = 0; j < delList.size(); ++j) {
            if((delList.at(j)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(j)) {
                DelDataItem *deldata = delList.at(j);
                selected_count++;
                QString file_path = deldata->savePath;
                QString file_name = deldata->fileName;
                QString default_savePath = getDownloadSavepathFromConfig();
                QString file_folder_path;

                if(default_savePath != deldata->savePath) {
                    int name_length = file_name.size();
                    int file_path_length = file_path.size();
                    int folder_path_length = file_path_length - name_length - 1;
                    file_folder_path = file_path.left(folder_path_length);
                } else {
                    file_folder_path = default_savePath;
                }

                QString path = QString("file:///") + file_folder_path;
                bool    is_open = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
            }
        }
    } else {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(m_iCurrentListviewRow == 1) {
                if(selectList.at(i)->status == Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        DataItem *data = selectList.at(i);
                        ++selected_count;

                        QString file_path = data->savePath;
                        QString file_name = data->fileName;
                        QString default_savePath = getDownloadSavepathFromConfig();
                        QString file_folder_path;

                        if(default_savePath != data->savePath) {
                            int name_length = file_name.size();
                            int file_path_length = file_path.size();
                            int folder_path_length = file_path_length - name_length - 1;
                            file_folder_path = file_path.left(folder_path_length);
                        } else {
                            file_folder_path = default_savePath;
                        }

                        QString path = QString("file:///") + file_folder_path;
                        bool    is_open = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                    }
                }
            } else {
                if(selectList.at(i)->status != Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        DataItem *data = selectList.at(i);
                        ++selected_count;

                        QString file_path = data->savePath;
                        QString file_name = data->fileName;
                        QString default_savePath = getDownloadSavepathFromConfig();
                        QString file_folder_path;

                        if(default_savePath != data->savePath) {
                            int name_length = file_name.size();
                            int file_path_length = file_path.size();
                            int folder_path_length = file_path_length - name_length - 1;
                            file_folder_path = file_path.left(folder_path_length);
                        } else {
                            file_folder_path = default_savePath;
                        }


                        QString path = QString("file:///") + file_folder_path;
                        bool    is_open = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                    }
                }
            }
        }
    }
    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    }
}

void MainFrame::onRenameActionTriggered()
{
    QList<DataItem *> selectList;
    int selected_count = 0;

    m_renameList.clear();
    selectList = m_pDownLoadingTableView->getTableModel()->renderList();
    for(int i = 0; i < selectList.size(); ++i) {
        if(selectList.at(i)->status == Complete) {
            if(selectList.at(i)->Ischecked == 1) {
                DataItem *data = selectList.at(i);
                m_renameList.append(data);
                ++selected_count;
            }
        }
    }
    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showRenameMsgbox();
    }
}

void MainFrame::onClearRecyleActionTriggered()
{
    // show_clear_MsgBox();
}

void MainFrame::onCopyUrlActionTriggered()
{
    QList<DelDataItem *> recycle_selectList;
    int selected_count = 0;
    QString copyUrl = "";
    QString url;

    if(m_iCurrentListviewRow == 2) {
        recycle_selectList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recycle_selectList.size(); ++i) {
            DelDataItem *data = recycle_selectList.at(i);
            if((data->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                S_Url_Info getUrlInfo;
                DBInstance::getUrlById(data->taskId, getUrlInfo);
                if(getUrlInfo.m_taskId != "") {
                    if(getUrlInfo.m_downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + getUrlInfo.m_infoHash;
                    }
                } else {
                    url = data->url;
                }

                ++selected_count;
                if(copyUrl == "") {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    } else {
        QList<DataItem *> selectList;

        selectList = m_pDownLoadingTableView->getTableModel()->renderList();

        for(int i = 0; i < selectList.size(); ++i) {
            DataItem *data = new DataItem;
            bool isSelect = false;
            if((m_iCurrentListviewRow == 1) && (selectList.at(i)->status == Complete)) {
                if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selected_count;
                    isSelect = true;
                }
            } else {
                if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                    data = selectList.at(i);
                    ++selected_count;
                    isSelect = true;
                }
            }
            if(isSelect) {
                S_Url_Info getUrlInfo;
                QString    url;
                DBInstance::getUrlById(data->taskId, getUrlInfo);
                if(getUrlInfo.m_taskId != "") {
                    if(getUrlInfo.m_downloadType == "torrent") {
                        url = "magnet:?xt=urn:btih:" + getUrlInfo.m_infoHash;
                    }
                } else {
                    url = data->url;
                }
                if(copyUrl == "") {
                    copyUrl = url;
                } else {
                    copyUrl = copyUrl + "\n" + url;
                }
            }
        }
    }


    if(selected_count == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        QClipboard *clipboard = DApplication::clipboard();
        clipboard->setText(copyUrl);

        m_pTaskNum->setText(tr("download uri already copyed"));
    }
}

void MainFrame::onDeletePermanentActionTriggered()
{
    int selectedCount = 0;

    if(m_iCurrentListviewRow == 2) {
        QList<DelDataItem *> recycle_selectList;
        m_pRecycleDeleteList.clear();
        recycle_selectList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recycle_selectList.size(); ++i) {
            if((recycle_selectList.at(i)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                m_pRecycleDeleteList.append(recycle_selectList.at(i));
                selectedCount++;
            }
        }
    } else {
        QList<DataItem *> selectList;
        m_pDeleteList.clear();
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            DataItem *data = new DataItem;
            if(m_iCurrentListviewRow == 1) {
                if(selectList.at(i)->status == Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            } else {
                if(selectList.at(i)->status != Complete) {
                    if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                        data = selectList.at(i);
                        m_pDeleteList.append(data);
                        ++selectedCount;
                    }
                }
            }
        }
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showDeleteMsgbox(true);
    }
}

void MainFrame::downloadLimitPeriod()
{
    QTime   current_time = QTime::currentTime();
    QTime  *periodStartTime = new QTime();
    QTime  *periodEndTime = new QTime();
    QString downloadSpeed, uploadSpeed;

    // get_limit_speed_time(period_start_time, period_end_time);
    S_DownloadSettings settings = Settings::getInstance()->getAllSpeedLimitInfo();

    periodStartTime->setHMS(settings.m_strStartTime.section(":", 0, 0).toInt(),
                              settings.m_strStartTime.section(":", 1, 1).toInt(),
                              settings.m_strStartTime.section(":", 2, 2).toInt());

    periodEndTime->setHMS(settings.m_strEndTime.section(":", 0, 0).toInt(),
                            settings.m_strEndTime.section(":", 1, 1).toInt(),
                            settings.m_strEndTime.section(":", 2, 2).toInt());

    downloadSpeed = settings.m_strMaxDownload;
    uploadSpeed = settings.m_strMaxUpload;

    // 判断当前时间是否在限速时间内
    bool bInPeriod = checkIfInPeriod(&current_time, periodStartTime, periodEndTime);
    if(!bInPeriod) {
        Aria2RPCInterface::Instance()->setDownloadUploadSpeed("102400", "5120");
    } else {
        Aria2RPCInterface::Instance()->setDownloadUploadSpeed(downloadSpeed, uploadSpeed);
    }
}

bool MainFrame::checkIfInPeriod(QTime *currentTime, QTime *periodStartTime, QTime *periodEndTime)
{
    int periodResult = checkTime(periodStartTime, periodEndTime);
    int cmpBeginResult = checkTime(currentTime, periodStartTime);
    int cmpEndResult = checkTime(currentTime, periodEndTime);

    // 开始时间与结束时间相同，只在相同时刻限速
    if(periodResult == 0) {
        // 当前时间和限速时间相同，在限速时间内
        if(cmpBeginResult == 0) {
            return true;
        }
    }
    // 开始时间，小于结束时间
    else if(periodResult == 1) {
        // 当前时间大于开始时间，并且小于结束时间
        if((cmpBeginResult == -1) && (cmpEndResult == 1)) {
            return true;
        }
    }
    // 开始时间，大于结束时间，即限时为开始时间到24点，0点到结束时间
    else {
        if((cmpBeginResult == -1) || (cmpEndResult == 1)) {
            return true;
        }
    }

    return false;
}

int  MainFrame::checkTime(QTime *startTime, QTime *endTime)
{
    if(startTime->hour() == endTime->hour()) {
        if(startTime->minute() == endTime->minute()) {
            return 0;
        } else if(startTime->minute()  < endTime->minute()) {
            return 1;
        } else {
            return -1;
        }
    } else if(startTime->hour() < endTime->hour()) {
        return 1;
    } else {
        return -1;
    }
    return 0;
}
