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
#include <QDBusInterface>
#include <DFontSizeManager>

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
#include "tabledatacontrol.h"

using namespace Global;

#define UOS_DOWNLOAD_MANAGER_DESKTOP_PATH  "/usr/share/applications/"

MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
  , m_pCheckItem(nullptr)
  , m_CheckIndex(QModelIndex())
{
    init();
    initTray();
    initAria2();
    initConnection();
    initTabledata();
    setPaletteType();
}

void MainFrame::init()
{
    m_iCurrentLab = downloadingLab;

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
    m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
    m_pDownLoadingTableView->setColumnHidden(4, true);

    m_pRecycleTableView = new TableView(recycle, m_pToolBar);
    m_pRecycleTableView->verticalHeader()->setDefaultSectionSize(48);
    m_pRecycleTableView->setColumnHidden(3, true);
    m_pDownLoadingTableView->getTableControl()->setRecycleTable(m_pRecycleTableView);
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

    QVBoxLayout *pnotaskWidgetlayout = new QVBoxLayout(m_pNotaskWidget);
    pnotaskWidgetlayout->setContentsMargins(10, 0, 0, 0);

    QFont lableFont;
    lableFont.setPointSize(15);
    lableFont.setBold(QFont::DemiBold);
    lableFont.setFamily("T5");
    m_pNotaskLabel = new Dtk::Widget::DLabel();
    m_pNotaskLabel->setFont(lableFont);
    m_pNotaskLabel->setText(tr("No download tasks"));
    m_pNotaskLabel->setAlignment(Qt::AlignHCenter);
    m_pNotaskLabel->setForegroundRole(DPalette::PlaceholderText);
    pnotaskWidgetlayout->addWidget(m_pNotaskLabel);

    m_pNoResultlabel = new Dtk::Widget::DLabel();
    m_pNoResultlabel->setFont(lableFont);
    m_pNoResultlabel->setText(tr("No result"));
    m_pNoResultlabel->setAlignment(Qt::AlignHCenter);
    m_pNoResultlabel->setForegroundRole(DPalette::PlaceholderText);
    m_pNoResultlabel->hide();
    pnotaskWidgetlayout->addWidget(m_pNoResultlabel);

    m_pNotaskTipLabel = new DLabel();
    QFont notaskTipLabelFont;
    notaskTipLabelFont.setPointSize(13);
    notaskTipLabelFont.setFamily("T6");
    m_pNotaskTipLabel->setFont(notaskTipLabelFont);
    m_pNotaskTipLabel->setText(tr("Click + to create new task"));
    m_pNotaskTipLabel->setAlignment(Qt::AlignHCenter);
    m_pNotaskTipLabel->setForegroundRole(DPalette::PlaceholderText);
    pnotaskWidgetlayout->addWidget(m_pNotaskTipLabel);
    pnotaskWidgetlayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_pRight_Widget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);

    m_pRightStackwidget = new QStackedWidget(this);
    m_pRightStackwidget->setCurrentIndex(0);

    QPalette pa;
    pa.setColor(QPalette::Background, QColor(255, 0, 0));
    m_pTaskNumWidget = new QWidget;
    m_pTaskNumWidget->setFixedHeight(30);
    //m_pTaskNumWidget->setPalette(pa);
    QHBoxLayout *TaskNumWidgetlayout = new QHBoxLayout(m_pTaskNumWidget);
    TaskNumWidgetlayout->setMargin(0);
    m_pTaskNum = new QLabel(tr("0 task"));
    m_pTaskNum->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pTaskNum->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
    TaskNumWidgetlayout->addWidget(m_pTaskNum);
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
    QStandardItemModel *pleftlistModel = new QStandardItemModel(this);

    m_pdownloadingItem = new DStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("Downloading"));
    m_pdownloadfinishItem = new DStandardItem(QIcon::fromTheme("dcc_print_done"), tr("Completed"));
    m_precycleItem = new DStandardItem(QIcon::fromTheme("dcc_list_delete"), tr("Trash"));
    m_pdownloadingItem->setEditable(false);
    m_pdownloadfinishItem->setEditable(false);
    m_precycleItem->setEditable(false);
    pleftlistModel->appendRow(m_pdownloadingItem);
    pleftlistModel->appendRow(m_pdownloadfinishItem);
    pleftlistModel->appendRow(m_precycleItem);
    m_pLeftList->setModel(pleftlistModel);
    pLeftLayout->addWidget(m_pLeftList, 0);

    m_pLeftList->setCurrentIndex(pleftlistModel->index(0, 0));

    m_pClipboard = new ClipboardTimer; // 获取当前剪切板
    m_pUpdateTimer = new QTimer(this);
    m_TrayClickTimer = new QTimer(this);
    m_curOpenBtDialogPath = "";
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
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
        this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    });
    connect(pNewDownloadAct, &QAction::triggered, [ = ]() {
        createNewTask("");
    });
    connect(pStartAllAct, &QAction::triggered, [=](){
        const QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        foreach(DataItem* pData, selectList){
            if(pData->status != Global::Status::Complete)
            continueDownload(pData);
        }

        Aria2RPCInterface::Instance()->unpauseAll();
        if(m_pUpdateTimer->isActive() == false) {
            m_pUpdateTimer->start(2 * 1000);
        }
    });
    connect(pPauseAllAct, &QAction::triggered,  [=](){
        Aria2RPCInterface::Instance()->pauseAll();
//        if(m_pUpdatetimer->isActive()) {
//            m_pUpdatetimer->stop();
//        }
    });
    connect(pQuitAct,      &QAction::triggered,         this, &MainFrame::onTrayQuitClick);
    connect(m_pSystemTray, &QSystemTrayIcon::activated, this, &MainFrame::onActivated);
    m_pSystemTray->setContextMenu(pTrayMenu);
    m_pSystemTray->show();
}

void MainFrame::initConnection()
{
    connect(m_pDownLoadingTableView, &TableView::signalHeaderStatechanged, this, &MainFrame::getHeaderStatechanged);
    connect(m_pDownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::onContextMenu, Qt::QueuedConnection);
    connect(m_pDownLoadingTableView, &TableView::pressed, this, &MainFrame::onTableItemSelected);
    connect(m_pDownLoadingTableView->getTableControl(), &tableDataControl::signalRedownload, this, &MainFrame::onRedownload);
    connect(m_pDownLoadingTableView->getTableControl(), &tableDataControl::signalAutoDownloadBt, this, &MainFrame::onClipboardDataForBt);
    connect(m_pDownLoadingTableView->getTableControl(), &tableDataControl::signalDownload, this, &MainFrame::getNewDownloadUrl);
    connect(m_pDownLoadingTableView->getTableModel(), &TableModel::signalCheckChange, this, &MainFrame::onCheckChanged);
    connect(m_pDownLoadingTableView, &TableView::doubleClicked,[=](){
        onOpenFileActionTriggered();
    });

    connect(m_pRecycleTableView, &TableView::signalHeaderStatechanged, this, &MainFrame::getHeaderStatechanged);
    connect(m_pRecycleTableView, &TableView::customContextMenuRequested, this, &MainFrame::onContextMenu, Qt::QueuedConnection);
    connect(m_pRecycleTableView, &TableView::pressed, this, &MainFrame::onTableItemSelected);
    connect(m_pRecycleTableView->getTableControl(), &tableDataControl::signalRedownload, this, &MainFrame::onRedownload);
    connect(m_pRecycleTableView->getTableControl(), &tableDataControl::signalAutoDownloadBt, this, &MainFrame::onClipboardDataForBt);
    connect(m_pRecycleTableView->getTableModel(), &TableModel::signalCheckChange, this, &MainFrame::onCheckChanged);
    connect(m_pRecycleTableView, &TableView::doubleClicked,[=](){
        onOpenFileActionTriggered();
    });

    connect(this, &MainFrame::switchTableSignal, m_pDownLoadingTableView, &TableView::signalClearHeaderCheck);
    connect(this, &MainFrame::switchTableSignal, m_pRecycleTableView, &TableView::signalClearHeaderCheck);
    connect(this, &MainFrame::headerViewChecked, m_pDownLoadingTableView, &TableView::signalTableViewAllChecked);
    connect(this, &MainFrame::headerViewChecked, m_pRecycleTableView, &TableView::signalTableViewAllChecked);

    connect(m_pSettingAction, &QAction::triggered, this, &MainFrame::onSettingsMenuClicked);
    connect(m_pClipboard, &ClipboardTimer::sendClipboardText, this, &MainFrame::onClipboardDataChanged);
    connect(m_pClipboard, &ClipboardTimer::sentBtText, this, &MainFrame::onClipboardDataForBt, Qt::UniqueConnection);
    connect(m_pClipboard, &ClipboardTimer::showMainWindows, this, &MainFrame::showWindowsForClipboard, Qt::UniqueConnection);
    connect(m_pLeftList, &DListView::clicked, this, &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::onPalettetypechanged);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainFrame::onPalettetypechanged);

    //connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::onPalettetypechanged);
    connect(m_pUpdateTimer, &QTimer::timeout, this, &MainFrame::updateMainUI);

    connect(m_pToolBar, &TopButton::newDownloadBtnClicked, this, &MainFrame::onNewBtnClicked);
    connect(m_pToolBar, &TopButton::getSearchEditTextChange, this, &MainFrame::onSearchEditTextChanged);
    connect(m_pToolBar, &TopButton::startDownloadBtnClicked, this, &MainFrame::onStartDownloadBtnClicked);
    connect(m_pToolBar, &TopButton::pauseDownloadBtnClicked, this, &MainFrame::onPauseDownloadBtnClicked);
    connect(m_pToolBar, &TopButton::deleteDownloadBtnClicked, this, &MainFrame::onDeleteDownloadBtnClicked);
    connect(this, &MainFrame::tableChanged, m_pToolBar, &TopButton::getTableChanged);
    connect(this, &MainFrame::signalRedownload, this, &MainFrame::onRedownload, Qt::QueuedConnection);

    connect(Settings::getInstance(), &Settings::signal_downloadSettingsChanged, this, &MainFrame::onDownloadLimitChanged);
    connect(Settings::getInstance(), &Settings::signal_poweronChanged, this, &MainFrame::onPowerOnChanged);
    connect(Settings::getInstance(), &Settings::signal_maxDownloadTaskNumberChanged, this, &MainFrame::onMaxDownloadTaskNumberChanged);
    connect(Settings::getInstance(), &Settings::signal_disckCacheChanged, this, &MainFrame::onDisckCacheChanged);
    connect(Settings::getInstance(), &Settings::signal_startAssociatedBTFileChanged, this, &MainFrame::startAssociatedBTFile);
}

void MainFrame::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::Trigger) {
        //if(m_TrayClickTimer->isActive()){
            if(this->isHidden()) {
                // 恢复窗口显示
                this->show();
                this->setWindowState(Qt::WindowActive);
                this->activateWindow();
                this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            } else {
                this->hide();
            }
        //} else {
            //m_TrayClickTimer->start(200);
            //m_TrayClickTimer->setSingleShot(true);
        //}
        return;
    }
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    if(Settings::getInstance()->getIsShowTip()) {
        MessageBox *msg = new MessageBox();
        connect(msg, &MessageBox::closeConfirmSig, this, &MainFrame::onMessageBoxConfirmClick);
        msg->setExit();
        msg->exec();
    } else {
        onMessageBoxConfirmClick();
    }
    event->ignore();
}

void MainFrame::paintEvent(QPaintEvent *event)
{
    setPaletteType();
}

void MainFrame::createNewTask(QString url)
{
    if(Settings::getInstance()->getNewTaskShowMainWindowState()) {
        activateWindow();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        show();
    }
    static newTaskWidget *pNewTaskWidget = new newTaskWidget();

    pNewTaskWidget->setUrl(url);
    connect(pNewTaskWidget, &newTaskWidget::NewDownload_sig, this, &MainFrame::getNewDownloadUrl, Qt::UniqueConnection);
    connect(pNewTaskWidget, &newTaskWidget::newDownLoadTorrent, this, &MainFrame::getNewDownloadTorrent, Qt::UniqueConnection);
    pNewTaskWidget->exec();
}

void MainFrame::onTrayQuitClick()
{
    if(!m_bShutdownOk) {
        MessageBox *pBox = new MessageBox();
        QString title = tr("Are you sure to exit? Tasks in download will be interrupted.");
        pBox->setWarings(title, tr("sure"), tr("cancel"));
        int rs = pBox->exec();
        if(rs != DDialog::Accepted) {
            return;
        }
    }
    m_pDownLoadingTableView->getTableControl()->saveDataBeforeClose();
    m_pRecycleTableView->getTableControl()->saveDataBeforeClose();
    Aria2RPCInterface::Instance()->shutdown();
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
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_RPCSuccess(QString,QJsonObject)), this,
            SLOT(onRpcSuccess(QString,QJsonObject)));
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_RPCError(QString,QString,int)), this,
            SLOT(onRpcError(QString,QString,int)));
    onDownloadLimitChanged();
    onMaxDownloadTaskNumberChanged(Settings::getInstance()->getMaxDownloadTaskNumber());
    qDebug() << "MainFrame initAria2 Finished";
}

void MainFrame::initTabledata()
{
    //m_ptableDataControl->initTabledata();

    QList<S_Task> list;
    S_Task tbTask;
    int    task_num = 0;

    DBInstance::getAllTask(list);
    for(int i = 0; i < list.length(); i++) {
        DataItem *data = new DataItem;
        initDataItem(data, list.at(i));
        if(data->status != Global::Status::Removed) {
            if(data->status == Global::Status::Lastincomplete) {
                m_pNotaskWidget->hide();
                QVariant autostart = Settings::getInstance()->getAutostartUnfinishedTaskState();
                m_pDownLoadingTableView->getTableModel()->append(data);
                if(autostart.toBool()) {
                    QString savePath = data->savePath;
                    QMap<QString, QVariant> opt;
                    opt.insert("dir", savePath.left(savePath.lastIndexOf("/")));
                    opt.insert("out", data->fileName);
                    S_Url_Info  getUrlInfo;
                    DBInstance::getUrlById(data->taskId, getUrlInfo);
                    if(getUrlInfo.m_taskId != "") {
                        if(getUrlInfo.m_downloadType == "torrent") {
                            QString selectNum = getUrlInfo.m_selectedNum;
                            opt.insert("select-file", selectNum);
                            if(!QFile(getUrlInfo.m_seedFile).exists()) {
                                showWarningMsgbox(tr("seed file not exists or broken;"));
                                qDebug() << "seed file not exists or broken;";
                            } else {
                                Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile,
                                                                          opt,
                                                                          getUrlInfo.m_taskId);
                                clearTableItemCheckStatus();
                                if(m_pUpdateTimer->isActive() == false) {
                                    m_pUpdateTimer->start(2 * 1000);
                                }
                            }
                        }
                    } else {
                        onDownloadLimitChanged();
                        Aria2RPCInterface::Instance()->addUri(data->url, opt, data->taskId);
                        clearTableItemCheckStatus();
                        if(m_pUpdateTimer->isActive() == false) {
                            m_pUpdateTimer->start(2 * 1000);
                        }
                    }
                }
            } else {
                m_pDownLoadingTableView->getTableModel()->append(data);
            }
        } else {
            DelDataItem *deldata = new DelDataItem;
            initDelDataItem(data, deldata);
            m_pRecycleTableView->getTableModel()->append(deldata);
        }
    }
    m_pDownLoadingTableView->getTableModel()->switchDownloadingMode();
    m_pDownLoadingTableView->refreshTableView(0);

    setTaskNum(m_iCurrentLab);
}

void MainFrame::setTaskNum(int currentLab)
{
    QList<DataItem *> data_list = m_pDownLoadingTableView->getTableModel()->dataList();
    QList<DataItem *> render_list = m_pDownLoadingTableView->getTableModel()->renderList();

    QList<DelDataItem *> m_recycleList = m_pRecycleTableView->getTableModel()->recyleList();
    int activeCount = 0;
    int finishCount = 0;
    int recycleCount = 0;
    QString activeNum;

    if(currentLab == 0) {
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
        activeNum = QString::number(activeCount) + tr(" item tasks");
        if(activeCount == 0) {
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
        }
    } else if(currentLab == 1) {
        int j = 0;
        for(const auto *item : render_list) {
            if(item->status == Global::Status::Complete) {
                if(!m_pDownLoadingTableView->isRowHidden(j)) {
                    ++finishCount;
                }
            }
            j++;
        }
        activeNum = QString::number(finishCount) + tr(" files");
        if(finishCount == 0) {
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
        }
    } else if(currentLab == 2){
        int k = 0;
        for(const auto *item : m_recycleList) {
            if(!m_pRecycleTableView->isRowHidden(k)) {
                ++recycleCount;
            }
            k++;
        }
        activeNum = QString::number(recycleCount) + tr(" files");
        if(recycleCount == 0) {
            m_pNotaskWidget->show();
        } else {
            m_pNotaskWidget->hide();
        }
    }
    m_pTaskNum->setText(activeNum);
}

void MainFrame::setPaletteType()
{
    m_pLeftList->setPalette(DGuiApplicationHelper::instance()->applicationPalette());

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        DPalette deeptheme_palette;
        deeptheme_palette.setBrush(DPalette::Background,
                                   DGuiApplicationHelper::instance()->applicationPalette().base());
        m_pLeftWidget->setPalette(deeptheme_palette);
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        DPalette label_palette;
        label_palette.setBrush(DPalette::Text,
                               DGuiApplicationHelper::instance()->applicationPalette().placeholderText());

        m_pNotaskTipLabel->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        DPalette tableviewPalette;

        tableviewPalette.setBrush(DPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());

        m_pDownLoadingTableView->setPalette(tableviewPalette);
        m_pRecycleTableView->setPalette(tableviewPalette);
        m_pNotaskWidget->setPalette(tableviewPalette);

        DPalette palette;
        QColor c = DGuiApplicationHelper::instance()->applicationPalette().base().color();
        c.setAlpha(70);
        palette.setColor(DPalette::Background,c);
        m_pTaskNumWidget->setPalette(palette);
        m_pdownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading_dark"));
        m_pdownloadfinishItem->setIcon(QIcon::fromTheme("dcc_print_done_dark"));
        m_precycleItem->setIcon(QIcon::fromTheme("dcc_list_delete_dark"));
        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText,
                                    DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_pTaskNum->setPalette(notaskTipLabelP);
    } else if(DGuiApplicationHelper::instance()->themeType() == 1) {
        DPalette p;
        p.setBrush(DPalette::Background,
                   DGuiApplicationHelper::instance()->applicationPalette().base());
        DPalette tableviewPalette;
        tableviewPalette.setBrush(DPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());
        m_pLeftWidget->setPalette(p);
        m_pNotaskWidget->setPalette(tableviewPalette);
        m_pDownLoadingTableView->setPalette(tableviewPalette);
        m_pRecycleTableView->setPalette(tableviewPalette);
        DPalette palette;
        QColor c = DGuiApplicationHelper::instance()->applicationPalette().base().color();
        c.setAlpha(70);
        palette.setColor(DPalette::Background,c);
        m_pTaskNumWidget->setPalette(palette);

        m_pdownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading"));
        m_pdownloadfinishItem->setIcon(QIcon::fromTheme("dcc_print_done"));
        m_precycleItem->setIcon(QIcon::fromTheme("dcc_list_delete"));
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());

        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText,DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_pTaskNum->setPalette(notaskTipLabelP);
    }
}

void MainFrame::onSettingsMenuClicked()
{
    DSettingsDialog *pSettingsDialog = new DSettingsDialog(this);

    pSettingsDialog->widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);
    pSettingsDialog->widgetFactory()->registerWidget("diskcacheInfo", Settings::createDiskCacheSettiingLabelHandle);
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
    if(!m_bIsCopyUrlFromLocal){
        createNewTask(url);
    } else{
        m_bIsCopyUrlFromLocal = false;
    }
}

void MainFrame::onClipboardDataForBt(QString url)
{
    if(url == m_curOpenBtDialogPath)
    {
        return;
    }
    else
    {
        m_curOpenBtDialogPath = url;
    }
    QString _savePath = Settings::getInstance()->getDownloadSavePath();
    BtInfoDialog *pBt = new BtInfoDialog(url, _savePath); // torrent文件路径
    QMap<QString, QVariant> opt;
    QString infoName;
    QString infoHash;
    bool isExist;
    bool _isOneClick = Settings::getInstance()->getOneClickDownloadState();
    if(_isOneClick)
    {
        pBt->slot_btnOK();
        pBt->getBtInfo(opt, infoName, infoHash);
        this->getNewDownloadTorrent(url, opt, infoName, infoHash);
        DBInstance::isExistBtInHash(infoHash, isExist);
        this->btNotificaitonSettings(tr("Download"),QString(tr("%1 downloading...")).arg(infoName),true);
        return;
    }

    int ret = pBt->exec();
    if(Settings::getInstance()->getNewTaskShowMainWindowState()) {
        activateWindow();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        show();
    }

    if(ret == QDialog::Accepted) {
        pBt->getBtInfo(opt, infoName, infoHash);
        this->getNewDownloadTorrent(url, opt, infoName, infoHash);
    }
}

void MainFrame::onListClicked(const QModelIndex &index)
{
    m_iCurrentLab = (currentLab)index.row();
    QString DownloadTask_Lable_Text;
    if((index.row() == 0) || (index.row() == 1)) {
        m_pRightStackwidget->setCurrentIndex(0);
        m_pDownLoadingTableView->refreshTableView(index.row());
        m_pDownLoadingTableView->horizontalHeader()->reset();
        bool switched = true;
        m_pDownLoadingTableView->reset(switched);
        if(index.row() == 1) {
            //m_pDownLoadingTableView->setFocus();
            //m_pDownLoadingTableView->getTableModel()->setData()
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
            m_pNotaskWidget->show();
            m_pNotaskLabel->setText(tr("No finished tasks"));
            m_pNotaskTipLabel->hide();
            m_pNoResultlabel->hide();
        } else {
            //m_pDownLoadingTableView->setFocus();
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
            m_pNotaskLabel->setText(tr("No download tasks"));
            m_pNotaskWidget->show();
            m_pNotaskTipLabel->show();
            m_pNoResultlabel->hide();
        }
    } else {
        m_pRightStackwidget->setCurrentIndex(1);
        //m_pRecycleTableView->setFocus();
        m_pNotaskWidget->show();
        m_pNotaskLabel->setText(tr("No deleted tasks"));
        m_pNotaskTipLabel->hide();
        m_pNoResultlabel->hide();
    }



    clearTableItemCheckStatus();
    emit switchTableSignal();
    setTaskNum(m_iCurrentLab);
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
    int cnt = (m_iCurrentLab == recycleLab ? m_pRecycleTableView->getTableModel()->rowCount()
               : m_pDownLoadingTableView->getTableModel()->rowCount());

    if(cnt > 0) {
        if(m_iCurrentLab == downloadingLab) {
            m_pToolBar->enableStartBtn(isChecked);
            m_pToolBar->enablePauseBtn(isChecked);
            m_pToolBar->enableDeleteBtn(isChecked);
        } else {
            m_pToolBar->enableStartBtn(false);
            m_pToolBar->enablePauseBtn(false);
            m_pToolBar->enableDeleteBtn(isChecked);
        }
        if(m_iCurrentLab == recycleLab) {
            m_pToolBar->enableStartBtn(true);
        }
    } else {
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }

    // end
    if((m_iCurrentLab == downloadingLab) || (m_iCurrentLab == finishLab)) {
        QList<DataItem *> render_list = m_pDownLoadingTableView->getTableModel()->renderList();
        if(0 == render_list.size()){
            emit headerViewChecked(false);
        }
        for(int j = 0; j < render_list.size(); j++) {
            DataItem *data = render_list.at(j);
            if(!isChecked) {
                if(m_iCurrentLab == downloadingLab) {
                    m_iDownloadingHeaderCheckStatus = 0;
                }
                if(m_iCurrentLab == finishLab) {
                    m_iFinishHeaderCheckStatus = 0;
                }
                data->Ischecked = false;
            } else {
                if(m_iCurrentLab == downloadingLab) {
                    m_iDownloadingHeaderCheckStatus = 2;
                }
                if(m_iCurrentLab == finishLab) {
                    m_iFinishHeaderCheckStatus = 2;
                }
                data->Ischecked = true;
            }

            m_pDownLoadingTableView->reset();
        }
    } else {
        QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();
        if(0 == recycle_list.size()){
            emit headerViewChecked(false);
        }
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

void MainFrame::getNewDownloadUrl(QStringList urlList, QString savePath, QString fileName)
{
    qDebug() << "getNewDownloadUrl: " << urlList << "    "  << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    bool isExitsUrl = false;
    QStringList sameUrlList;

    // 判断url是否在数据中已存在
    for(int i = 0; i < urlList.size(); i++) {
        DBInstance::isExistUrl(urlList[i], isExitsUrl);
        if(isExitsUrl) {
            sameUrlList.append(urlList[i]);
            urlList.removeAt(i);
            --i;
        }
    }
    if(!sameUrlList.isEmpty()) {
        showRedownloadMsgbox(sameUrlList);
    }
    if(urlList.isEmpty()) {
        //qDebug() << "url is NULl";
        return;
    }

    // 将url加入数据库和aria
    S_Task task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    for(int i = 0; i < urlList.size(); i++) {
        task = getUrlToName(urlList[i], savePath, fileName);
        DBInstance::addTask(task);
        Aria2RPCInterface::Instance()->addNewUri(task.m_url, savePath, task.m_downloadFilename, task.m_taskId);
        //clearTableItemCheckStatus();
        emit headerViewChecked(false);
    }

    m_pNotaskWidget->hide();

    // 定时器打开
    //if(m_pUpdateTimer->isActive() == false) {
        m_pUpdateTimer->start(2 * 1000);
   // }
}

S_Task MainFrame::getUrlToName(QString url, QString savePath, QString name)
{
    // 获取url文件名
    QString fileName;

    if(name != ""){
        fileName = name;
    } else if(url.startsWith("magnet")) {
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
    if(fileName.contains(".torrent")){
        fileName = fileName.remove(".torrent");
    }
    int count = DBInstance::getSameNameCount(fileName.mid(0, fileName.lastIndexOf(".")));
    if(count > 0){
        QString name1 = fileName.mid(0, fileName.lastIndexOf('.'));
        name1 += QString("_%1").arg(count);
        fileName = name1 + fileName.mid(fileName.lastIndexOf('.'), fileName.length());
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

void MainFrame::continueDownload(DataItem *pItem)
{
    if(pItem->status != Global::Status::Active) {
        if(pItem->status == Global::Status::Lastincomplete) {
            // QString  save_path=selectList.at(i)->savePath;
            QString filePath = pItem->savePath;
            QString fileName = pItem->fileName;
            QString defaultSavepath = getDownloadSavepathFromConfig();
            QString savePath;
            if(defaultSavepath != pItem->savePath) {
                int nameLength = fileName.size();
                int filePathLength = filePath.size();
                int folderPathLength = filePathLength - nameLength - 1;
                savePath = filePath.left(folderPathLength);
            } else {
                savePath = defaultSavepath;
            }

            QMap<QString, QVariant> opt;
            opt.insert("dir", savePath);
            opt.insert("out", fileName);
            S_Url_Info  getUrlInfo;
            DBInstance::getUrlById(pItem->taskId, getUrlInfo);
            if(getUrlInfo.m_taskId != "") {
                if(getUrlInfo.m_downloadType == "torrent") {
                    QString select_num = getUrlInfo.m_selectedNum;
                    opt.insert("select-file", select_num);
                    if(!QFile(getUrlInfo.m_seedFile).exists()) {
                        showWarningMsgbox(tr("seed file not exists or broken;"));
                        qDebug() << "seed file not exists or broken;";
                    } else {
                        Aria2RPCInterface::Instance()->addTorrent(getUrlInfo.m_seedFile,
                                                                  opt,
                                                                  getUrlInfo.m_taskId);
                        if(m_pUpdateTimer->isActive() == false) {
                            m_pUpdateTimer->start(2 * 1000);
                        }
                    }
                }
            } else {
                // deal_download_upload_limit_period();
                Aria2RPCInterface::Instance()->addUri(pItem->url, opt, pItem->taskId);
                //clearTableItemCheckStatus();
                if(m_pUpdateTimer->isActive() == false) {
                    m_pUpdateTimer->start(2 * 1000);
                }
            }
        } else {
            Aria2RPCInterface::Instance()->unpause(pItem->gid, pItem->taskId);
        }
    }
}

void MainFrame::onContextMenu(const QPoint &pos)
{
    if(m_iCurrentLab == recycleLab){
        QModelIndex index = m_pRecycleTableView->indexAt(pos);
        if(index.isValid()){
            QModelIndex realIndex =index.sibling(index.row(),0);
            QString gid = m_pRecycleTableView->getTableModel()->data(index, TableModel::GID).toString();
            m_pCheckItem = m_pRecycleTableView->getTableModel()->find(gid);
            if(!m_pRecycleTableView->getTableModel()->data(index, TableModel::Ischecked).toBool()){
                getHeaderStatechanged(false);
            }
            m_pRecycleTableView->getTableModel()->setData(realIndex, true, TableModel::Ischecked);
        }
    } else {
        QModelIndex index = m_pDownLoadingTableView->indexAt(pos);
        if(index.isValid()){
            QModelIndex realIndex =index.sibling(index.row(),0);
            QString gid = m_pDownLoadingTableView->getTableModel()->data(index, TableModel::GID).toString();
            m_pCheckItem = m_pDownLoadingTableView->getTableModel()->find(gid);
            m_CheckIndex = index;
            if(!m_pDownLoadingTableView->getTableModel()->data(index, TableModel::Ischecked).toBool()){
                getHeaderStatechanged(false);
            }
            m_pDownLoadingTableView->getTableModel()->setData(realIndex, true, TableModel::Ischecked);
        }
    }

    int chkedCnt = 0;
    DataItem *pDownloadItem = nullptr;
    DelDataItem *pDeleteItem = nullptr;
    if(m_iCurrentLab == recycleLab) {
        QList<DelDataItem *> recyleList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < recyleList.size(); i++) {
            if(recyleList.at(i)->Ischecked) {
                chkedCnt++;
                pDeleteItem = recyleList.at(i);
            }
        }
    } else {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); i++) {
            if(selectList.at(i)->Ischecked) {
                chkedCnt++;
                pDownloadItem = selectList.at(i);
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
    if(m_iCurrentLab == downloadingLab) {
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
        if((errorCount > 0) && (1 == chkedCnt)) {
            QAction *pActionredownload = new QAction();
            pActionredownload->setText(tr("Download again"));
            delmenlist->addAction(pActionredownload);
            connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        }
        delmenlist->addSeparator();
    }

    if(m_iCurrentLab == recycleLab && QFileInfo(pDeleteItem->savePath).exists()) {
        QAction *returnedToOrigin = new QAction();
        returnedToOrigin->setText(tr("Restore"));
        delmenlist->addAction(returnedToOrigin);
        connect(returnedToOrigin, &QAction::triggered, this, &MainFrame::onReturnOriginActionTriggered);
    }
    if((m_iCurrentLab == recycleLab) && (1 == chkedCnt)) {
        QAction *pActionredownload = new QAction();
        pActionredownload->setText(tr("Download again"));
        delmenlist->addAction(pActionredownload);
        connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
    }
    if((m_iCurrentLab == finishLab) || (m_iCurrentLab == recycleLab)) {
        if((1 == chkedCnt && m_iCurrentLab == finishLab && QFileInfo(pDownloadItem->savePath).exists()) ||
           (1 == chkedCnt && m_iCurrentLab == recycleLab && QFileInfo(pDeleteItem->savePath).exists())){
            QAction *pActionopenFile = new QAction();
            pActionopenFile->setText(tr("Open"));
            delmenlist->addAction(pActionopenFile);
            connect(pActionopenFile, &QAction::triggered, this, &MainFrame::onOpenFileActionTriggered);
        }
    }
    if((1 == chkedCnt && (m_iCurrentLab == finishLab) && QFileInfo(pDownloadItem->savePath).exists()) ||
       (1 == chkedCnt && m_iCurrentLab == recycleLab && QFileInfo(pDeleteItem->savePath).exists())){
        QAction *pActionopenFoler = new QAction();
        pActionopenFoler->setText(tr("Open folder"));
        delmenlist->addAction(pActionopenFoler);
        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if(m_iCurrentLab == finishLab) {
        QList<DataItem *> selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked == 1) {
                DataItem *data = selectList.at(i);
                if(data->status == Global::Status::Complete) {
                    ++renamCount;
                }
            }
        }

        if(renamCount == 1  && QFileInfo(pDownloadItem->savePath).exists()) {
            QAction *pactionRename = new QAction();
            pactionRename->setText(tr("Rename"));
            delmenlist->addAction(pactionRename);
            delmenlist->addSeparator();
            connect(pactionRename, &QAction::triggered, this, &MainFrame::onRenameActionTriggered);
        }
        if(QFileInfo(pDownloadItem->savePath).exists()){
            QAction *pAction_move = new QAction();
            pAction_move->setText(tr("Move to"));
            delmenlist->addAction(pAction_move);
            delmenlist->addSeparator();
            connect(pAction_move, &QAction::triggered, this, &MainFrame::onMoveToActionTriggered);
        }

    }

    if(1 == chkedCnt && m_iCurrentLab == recycleLab){
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if(m_iCurrentLab == downloadingLab) {
            delmenlist->addSeparator();
        }
    }

    QAction *pactiondelDownloading = new QAction();
    pactiondelDownloading->setText(tr("Delete"));
    delmenlist->addAction(pactiondelDownloading);
    connect(pactiondelDownloading, &QAction::triggered, this, &MainFrame::onDelActionTriggered);

    QAction *pactionDeletePermanently = new QAction();
    pactionDeletePermanently->setText(tr("Permanently delete"));
    delmenlist->addAction(pactionDeletePermanently);
    connect(pactionDeletePermanently, &QAction::triggered, this, &MainFrame::onDeletePermanentActionTriggered);

    if(1 == chkedCnt && m_iCurrentLab == finishLab){
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if(m_iCurrentLab == downloadingLab) {
            delmenlist->addSeparator();
        }
    }

    if(1 == chkedCnt && m_iCurrentLab == downloadingLab){
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if(m_iCurrentLab == downloadingLab) {
            delmenlist->addSeparator();
        }
        QAction *pActionopenFoler = new QAction();
        pActionopenFoler->setText(tr("Open folder"));
        delmenlist->addAction(pActionopenFoler);
        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if(m_iCurrentLab == recycleLab) {
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

    if(m_iCurrentLab == recycleLab) {
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
        if(m_iCurrentLab == downloadingLab) {
            m_pToolBar->enableStartBtn(true);
            m_pToolBar->enablePauseBtn(true);
            m_pToolBar->enableDeleteBtn(true);
        } else if(m_iCurrentLab == finishLab) {
            m_pToolBar->enableDeleteBtn(true);
            if(1 == chkedCnt){
                m_pToolBar->enableStartBtn(true);
                m_pToolBar->enablePauseBtn(true);
            } else {
                m_pToolBar->enableStartBtn(false);
                m_pToolBar->enablePauseBtn(false);
            }
        } else if(m_iCurrentLab == recycleLab) {
            m_pToolBar->enableStartBtn(true);
            m_pToolBar->enablePauseBtn(true);
            m_pToolBar->enableDeleteBtn(true);
        }
    } else {
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
        if(m_iCurrentLab == recycleLab){
            m_pToolBar->enableStartBtn(true);
        }
    }
}

void MainFrame::clearTableItemCheckStatus()
{
    if((m_iCurrentLab == downloadingLab) || (m_iCurrentLab == finishLab)) {
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
    emit headerViewChecked(false);
}

void MainFrame::onSearchEditTextChanged(QString text)
{
    if(text != ""){
        m_pNotaskLabel->hide();
        m_pNotaskTipLabel->hide();
        m_pNoResultlabel->show();
    } else {
        m_pNotaskLabel->show();
        if(m_iCurrentLab == downloadingLab){
            m_pNotaskTipLabel->show();
        }
        m_pNoResultlabel->hide();
    }
    m_SearchContent = text;
    m_pDownLoadingTableView->getTableControl()->searchEditTextChanged(text);
    m_pRecycleTableView->getTableControl()->searchEditTextChanged(text);
    setTaskNum(m_iCurrentLab);
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
    QStringList sameFileList;
    for(int i = 0; i < urlList.size(); i++){
        if((urlList[i].m_infoHash == infoHash) && (urlList[0].m_selectedNum == selectedNum)) {
            showWarningMsgbox(tr("Task exist."));
            return;
//            sameFileList.append(btPath);
//            if(!showRedownloadMsgbox(sameFileList)){
//                return;
//            }
//            int count = DBInstance::getSameNameCount(infoName);
//            if(count > 0){
//                QString name1 = infoName.mid(0, infoName.lastIndexOf('.'));
//                name1 += QString("_%1").arg(count);
//                infoName = name1 + infoName.mid(infoName.lastIndexOf('.'), infoName.length());
//            }
        }
    }


    // 将任务添加如task表中
    S_Task  task;
    QString strId = QUuid::createUuid().toString();
    task.m_taskId = strId;
    task.m_gid = "";
    task.m_gidIndex = 0;
    task.m_url = "";
    task.m_downloadPath = Settings::getInstance()->getDownloadSavePath();
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
    urlInfo.m_infoHash = infoHash;
    DBInstance::addUrl(urlInfo);

    opt.insert("out", infoName);
    // 开始下载
    Aria2RPCInterface::Instance()->addTorrent(btPath, opt, strId);
    clearTableItemCheckStatus();
    // 定时器打开
    if(m_pUpdateTimer->isActive() == false) {
        m_pUpdateTimer->start(2 * 1000);
    }
}

void MainFrame::onRedownload(QString taskId, int rd)
{
    if((rd == 0) || (rd == 1)) {
        m_pDownLoadingTableView->getTableControl()->downloadListRedownload(taskId);
    } else {
        m_pRecycleTableView->getTableControl()->recycleListRedownload(taskId);
    }
    clearTableItemCheckStatus();
    updateMainUI();
    if(m_pUpdateTimer->isActive() == false) {
        m_pUpdateTimer->start(2 * 1000);
    }
}

void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
    MessageBox *msg = new MessageBox();

    msg->setWarings(title, tr("sure"), "", sameUrlCount, sameUrlList);
    msg->exec();
}

void MainFrame::showClearMsgbox()
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::ClearrecycleSig, this, &MainFrame::getClearRecycleSlot);
    msg->setClear();
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
        emit switchTableSignal();
    }
}

void MainFrame::getClearRecycleSlot(bool ischecked)
{
    QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();

    if(ischecked) {
        for(int i = 0; i < recycle_list.size(); ++i) {
            DelDataItem *data = recycle_list.at(i);
            QString aria_temp_file = data->savePath + ".aria2";
            if(!data->savePath.isEmpty()) {
                QFile::remove(data->savePath);
                if(QFile::exists(aria_temp_file)) {
                    QFile::remove(aria_temp_file);
                }
            }
            Aria2RPCInterface::Instance()->removeDownloadResult(data->gid);
        }
    }
    for(int i = 0; i < recycle_list.size(); ++i) {
        DBInstance::delAllTask();
    }

    m_pRecycleTableView->getTableModel()->removeItems(true);
}

void MainFrame::showReloadMsgbox()
{
    MessageBox *msg = new MessageBox();

    // connect(msg,&MessageBox::ReDownload_sig,this,&MainWindow::get_Redowload_confirm_slot);
    QString title = tr("Do you continue?");

    msg->setWarings(title, tr("sure"), tr("cancel"));
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);

        // 重新下载：通知aria2移除下载项

        if((m_iCurrentLab == downloadingLab) || (m_iCurrentLab == finishLab)) {
            for(int i = 0; i < m_reloadList.size(); i++) {
                DataItem *data = m_reloadList.at(i);
                Aria2RPCInterface::Instance()->forceRemove(data->gid,"REDOWNLOAD_"+ QString::number(m_iCurrentLab)+ "_" + data->taskId);
            }
        } else {
            for(int i = 0; i < m_recycleReloadList.size(); i++) {
                DelDataItem *data = m_recycleReloadList.at(i);
                Aria2RPCInterface::Instance()->forceRemove(data->gid,"REDOWNLOAD_" + QString::number(m_iCurrentLab) + "_" + data->taskId);
            }
        }
    }
}

void MainFrame::showDeleteMsgbox(bool permanently)
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::DeletedownloadSig, this, &MainFrame::onGetDeleteConfirm);
    if(m_iCurrentLab == downloadingLab){
        msg->setDelete(permanently, true);
    } else {
        msg->setDelete(permanently);
    }
    int rs = msg->exec();
    if(rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_pToolBar->enableStartBtn(false);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
        if(m_iCurrentLab == recycleLab){
            m_pToolBar->enableStartBtn(true);
        }
    }
}

void MainFrame::showRenameMsgbox()
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::RenameSig, this, &MainFrame::getRenameConfirmSlot);
    QString title = tr("Rename");
    msg->setReName(title, tr("sure"), tr("cancel"), m_pCheckItem->fileName);
    msg->exec();
}

bool MainFrame::showRedownloadMsgbox(QList<QString> sameUrlList)
{
    MessageBox *msg = new MessageBox();

    connect(msg, &MessageBox::reDownloadSig, this, &MainFrame::getRedownloadConfirmSlot);
    QString title = tr("Redownload");
    msg->setRedownload(sameUrlList);
    int rs = msg->exec();
    if(rs == DDialog::Accepted){
        return true;
    }
    QString url;
    foreach(QString str, sameUrlList){
        url.append(str);
        url.append("/n");
    }
    createNewTask(url);
    return false;
}

void MainFrame::onAria2Remove(QString gId, QString id)
{
    Aria2RPCInterface::Instance()->remove(gId, id);
}

void MainFrame::onGetDeleteConfirm(bool ischecked, bool permanent)
{
    if(m_pUpdateTimer->isActive()) {
        m_pUpdateTimer->stop();
    }

    if(m_iCurrentLab == recycleLab) {
        m_pRecycleTableView->getTableControl()->onDeleteRecycleListConfirm(ischecked, permanent);
    } else {
        m_pDownLoadingTableView->getTableControl()->onDeleteDownloadListConfirm(ischecked, permanent, m_pRecycleTableView);
    }
    setTaskNum(m_iCurrentLab);

    if(this->m_SearchContent != "") {
        onSearchEditTextChanged(m_SearchContent);
    }
    if(m_pUpdateTimer->isActive() == false) {
        m_pUpdateTimer->start(2 * 1000);
    }
}

QString   MainFrame::getDownloadSavepathFromConfig()
{
     return Settings::getInstance()->getDownloadSavePath();
}

void MainFrame::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control) {
        m_bctrlkeyPress = true;
        qDebug() << "Key_Control";
    }
    if(event->key() == Qt::Key_A) {
        if(m_bctrlkeyPress == true){
            getHeaderStatechanged(true);
            emit headerViewChecked(true);
            qDebug() << "Key_Control + Key_A";
        }

    }
    QWidget::keyPressEvent(event);
}

void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
    if(m_bctrlkeyPress == true) {
        m_bctrlkeyPress = false;
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

    if(m_iCurrentLab == downloadingLab) {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->Ischecked && !m_pDownLoadingTableView->isRowHidden(i)) {
                ++selectedCount;
                continueDownload(selectList.at(i));
            }
        }

    } else if(m_iCurrentLab == finishLab) {
        onOpenFolderActionTriggered();
    } else {
        onClearRecyleActionTriggered();
    }
}

void MainFrame::onPauseDownloadBtnClicked()
{
    QList<DataItem *> selectList;
    int selectedCount = 0;

    if(m_iCurrentLab == downloadingLab) {
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
    } else if(m_iCurrentLab == finishLab) {
        onOpenFileActionTriggered();
    } else {
        onReturnOriginActionTriggered();
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
        m_pDownLoadingTableView->getTableControl()->aria2MethodAdd(json, m_SearchContent);
    } else if(method == ARIA2C_METHOD_TELL_STATUS) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodStatusChanged(json, m_iCurrentLab, m_SearchContent);
    } else if(method == ARIA2C_METHOD_SHUTDOWN) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodShutdown(json);
    } else if(method == ARIA2C_METHOD_GET_FILES) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodGetFiles(json, m_iCurrentLab);
    } else if(method == ARIA2C_METHOD_UNPAUSE) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodUnpause(json, m_iCurrentLab);
    } else if(method == ARIA2C_METHOD_FORCE_REMOVE) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodForceRemove(json);
    } else if(method == ARIA2C_METHOD_UNPAUSE_ALL) {
        m_pDownLoadingTableView->getTableControl()->aria2MethodUnpauseAll(json, m_iCurrentLab);
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

    if(m_bctrlkeyPress == false && selected.column() != 0) {
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
    } else if(m_bctrlkeyPress == true || selected.column() == 0) {
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
    m_bShutdownOk = true;
    for(const auto *item : dataList) {
        if((item->status == Global::Status::Active) || (item->status == Global::Status::Waiting)) {
            Aria2RPCInterface::Instance()->tellStatus(item->gid, item->taskId);
        }
        if((item->status == Global::Status::Active) || (item->status == Global::Status::Waiting) ||
           (item->status == Global::Status::Paused) || (item->status == Global::Status::Lastincomplete) ||
           (item->status == Global::Status::Error)) {
            ++activeCount;
        }
        if(item->status == Global::Status::Active){
            m_bShutdownOk = false;
        }
    }

    /*  if(this->g_search_content != "")
       {
          slot_searchEditTextChange(this->g_search_content);
       }*/
    if(activeCount == 0) {
        if(m_pUpdateTimer->isActive()) {
            m_pUpdateTimer->stop();
        }
    }
    setTaskNum(m_iCurrentLab);
}

void MainFrame::onDelActionTriggered()
{
    int selectedCount = 0;

    if(m_iCurrentLab == recycleLab) {
        selectedCount = m_pRecycleTableView->getTableControl()->onDelAction(m_iCurrentLab);
    } else {
        selectedCount = m_pDownLoadingTableView->getTableControl()->onDelAction(m_iCurrentLab);
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showDeleteMsgbox(false);
    }
}

void MainFrame::onRedownloadActionTriggered()
{
    int selectedCount = 0;

    if((m_iCurrentLab == finishLab) || (m_iCurrentLab == downloadingLab)) {
        selectedCount = m_pDownLoadingTableView->getTableControl()->RedownloadDownloadAndFinishList(m_reloadList);
    } else {
        selectedCount = m_pRecycleTableView->getTableControl()->RedownloadTrashList(m_recycleReloadList);
    }
    if(selectedCount == 0) {
        //showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showReloadMsgbox();
    }
}

void MainFrame::onReturnOriginActionTriggered()
{
    int selectedCount = 0;
    QList<DelDataItem *> recycle_list = m_pRecycleTableView->getTableModel()->recyleList();

    for(int i = 0; i < recycle_list.size(); i++) {
        DelDataItem *data = recycle_list.at(i);
        if((data->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
            DataItem *returntoData = new DataItem;
            ++selectedCount;
            if(data->completedLength == data->totalLength) {
                if(data->totalLength != "0B") {
                    returntoData->status = Global::Status::Complete;
                } else {
                    returntoData->status = Global::Status::Lastincomplete;
                }
            } else {
                long completedLength = returntoData->completedLength.toLong();


                if((returntoData->completedLength != "0B") && (returntoData->totalLength != "0B")) {
                    returntoData->percent =  returntoData->completedLength.toLong() * 100.0 /
                                             returntoData->totalLength.toLong();

                    if((returntoData->percent < 0) || (returntoData->percent > 100)) {
                        returntoData->status = Global::Status::Lastincomplete;
                        //returntoData->percent = 0;
                    }
                } else {
                    returntoData->percent = 0;
                }
            }
            returntoData->fileName = data->fileName;
            returntoData->savePath = data->savePath;
            returntoData->totalLength = data->totalLength;
            returntoData->completedLength = data->completedLength;
            returntoData->url = data->url;
            returntoData->gid = data->gid;
            returntoData->time = data->finishTime;
            returntoData->taskId = data->taskId;
            //returntoData->status = Global::Status::Lastincomplete;
            m_pDownLoadingTableView->getTableModel()->append(returntoData);
            if((data->completedLength != data->totalLength) || (data->totalLength == "0B")) {
                QMap<QString, QVariant> opt;
                returntoData->url = data->url;
                QString filePath = data->savePath;
                QString fileName = data->fileName;
                QString defaultSavepath = getDownloadSavepathFromConfig();
                QString savePath;

                if(defaultSavepath != data->savePath) {
                    int nameLength = fileName.size();
                    int filePathLength = filePath.size();
                    int folderPathLength = filePathLength - nameLength - 1;
                    savePath = filePath.left(folderPathLength);
                } else {
                    savePath = defaultSavepath;
                }


                opt.insert("dir", savePath);
                //opt.insert("out", fileName);
                S_Url_Info  getUrlInfo;
                DBInstance::getUrlById(returntoData->taskId, getUrlInfo);
                if(getUrlInfo.m_taskId != "") {
                    if(getUrlInfo.m_downloadType == "torrent") {
                        QString select_num = getUrlInfo.m_selectedNum;
                        QString seed_file_path = getUrlInfo.m_seedFile;

                        opt.insert("select-file", select_num);

                        if(!QFile(seed_file_path).exists()) {
                            showWarningMsgbox(tr("seed file not exists or broken;"));
                        } else {
                            Aria2RPCInterface::Instance()->addTorrent(seed_file_path, opt, getUrlInfo.m_taskId);
                            clearTableItemCheckStatus();
                            if(m_pUpdateTimer->isActive() == false) {
                                m_pUpdateTimer->start(2 * 1000);
                            }
                        }
                    }
                } else {
                    onDownloadLimitChanged();
                    //Aria2RPCInterface::Instance()->addUri(returntoData->url, opt, returntoData->taskId);
                    //Aria2RPCInterface::Instance()->pause(returntoData->gid, returntoData->taskId);
                    if(m_pUpdateTimer->isActive() == false) {
                        m_pUpdateTimer->start(2 * 1000);
                    }
                }
            }
            m_pRecycleTableView->getTableModel()->removeItem(data);
            setTaskNum(m_iCurrentLab);
        }
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        // ToolBar禁用按钮联动：还原后禁用按钮
        m_pToolBar->enableStartBtn(true);
        m_pToolBar->enablePauseBtn(false);
        m_pToolBar->enableDeleteBtn(false);
    }
}

void MainFrame::onOpenFileActionTriggered()
{
    QList<DataItem *> selectList;
    QList<DelDataItem *> delList;
    int selectedCount = 0;

    if(m_iCurrentLab == finishLab) {
        selectList = m_pDownLoadingTableView->getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); ++i) {
            if(selectList.at(i)->status == Complete) {
                if((selectList.at(i)->Ischecked == 1) && !m_pDownLoadingTableView->isRowHidden(i)) {
                    DataItem *data = selectList.at(i);
                    ++selectedCount;
                    QString file_path = data->savePath;
                    QString file_name = data->fileName;
                    QString path = QString("file:///") + file_path;
                    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
                }
            }
        }
    } else {
        delList = m_pRecycleTableView->getTableModel()->recyleList();
        for(int i = 0; i < delList.size(); ++i) {
            if((delList.at(i)->Ischecked == 1) && !m_pRecycleTableView->isRowHidden(i)) {
                DelDataItem *data = delList.at(i);
                ++selectedCount;
                QString file_path = data->savePath;
                QString file_name = data->fileName;
                QString path = QString("file:///") + file_path;
                QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
            }
        }
    }

//    if(selectedCount == 0) {
//        showWarningMsgbox(tr("no item is selected,please check items!"));
//    }
}

void MainFrame::onOpenFolderActionTriggered()
{
    int selectedCount = 0;

    if(m_iCurrentLab == recycleLab) {
       selectedCount = m_pRecycleTableView->getTableControl()->onOpenFolderAction(m_iCurrentLab);
    } else {
       selectedCount = m_pDownLoadingTableView->getTableControl()->onOpenFolderAction(m_iCurrentLab);
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    }
}

void MainFrame::onRenameActionTriggered()
{
    m_pDownLoadingTableView->setCurrentIndex(m_CheckIndex);
    m_pDownLoadingTableView->edit(m_CheckIndex);
}

void MainFrame::onMoveToActionTriggered()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    QStringList fileName;
    if ( fileDialog->exec() == QDialog::Accepted )
    {
        fileName = fileDialog->selectedFiles();
        QString filePath = fileName.first();
        if(filePath != "") {
            QList<DataItem *> selectList;
            selectList = m_pDownLoadingTableView->getTableModel()->renderList();
            for(int i = 0; i < selectList.size(); ++i) {
                if(selectList.at(i)->status == Complete) {
                    if(selectList.at(i)->Ischecked == 1) {
                        DataItem *data = selectList.at(i);
                        QFile::rename(data->savePath, filePath + "/" + data->fileName);
                        data->savePath = filePath + "/" + data->fileName;
                        S_Task task;
                        DBInstance::getTaskByID(data->taskId,task);
                        task.m_downloadPath = data->savePath;
                        task.m_downloadFilename = data->fileName;
                        DBInstance::updateTaskByID(task);
                    }
                }
            }
        }
    }
}

void MainFrame::onClearRecyleActionTriggered()
{
    showClearMsgbox();
}

void MainFrame::onCopyUrlActionTriggered()
{
    int selectedCount = 0;
    QString copyUrl = "";

    if(m_iCurrentLab == recycleLab) {
        selectedCount = m_pRecycleTableView->getTableControl()->onCopyUrlAction(m_iCurrentLab, copyUrl);
    } else {
        selectedCount = m_pDownLoadingTableView->getTableControl()->onCopyUrlAction(m_iCurrentLab, copyUrl);
    }


    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        m_bIsCopyUrlFromLocal = true;
        QClipboard *clipboard = DApplication::clipboard();
        clipboard->setText(copyUrl);
        m_pTaskNum->setText(tr("Copied to clipboard"));

        QString showHead(tr("Downloader"));
        QString showInfo(tr("Copied to clipboard"));
        this->btNotificaitonSettings(showHead, showInfo);
    }
}

void MainFrame::onDeletePermanentActionTriggered()
{
    int selectedCount = 0;

    if(m_iCurrentLab == recycleLab) {
        selectedCount = m_pRecycleTableView->getTableControl()->onDeletePermanentAction(m_iCurrentLab);
    } else {
        selectedCount = m_pDownLoadingTableView->getTableControl()->onDeletePermanentAction(m_iCurrentLab);
    }
    if(selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showDeleteMsgbox(true);
    }
}

void MainFrame::getRenameConfirmSlot(QString &name)
{
    if(name == m_pCheckItem->fileName){
        return;
    }

    QString FilePath;
    FilePath = m_pCheckItem->savePath.left(m_pCheckItem->savePath.length()
                                           - m_pCheckItem->fileName.length());

    if(QFileInfo::exists(FilePath + name)){
        return;
    }

    QFile::rename(FilePath + m_pCheckItem->fileName, FilePath + name);
    m_pCheckItem->fileName = name;
    m_pCheckItem->savePath = FilePath + name;
    m_pDownLoadingTableView->update();
    m_pCheckItem = nullptr;
    S_Task task;
    DBInstance::getTaskByID(m_pCheckItem->taskId,task);
    task.m_downloadPath = m_pCheckItem->savePath;
    task.m_downloadFilename = name;
    DBInstance::updateTaskByID(task);
}

void MainFrame::getRedownloadConfirmSlot(const QList<QString> &sameUrlList)
{
    if(sameUrlList.at(0).contains(".torrent")){
        return;
    }
    // 将url加入数据库和aria
    S_Task task;
    QMap<QString, QVariant> opt;
    QString savePath = Settings::getInstance()->getDownloadSavePath();
    opt.insert("dir", savePath);
    for(int i = 0; i < sameUrlList.size(); i++) {
        task = getUrlToName(sameUrlList[i], savePath, "");
        DBInstance::addTask(task);
        Aria2RPCInterface::Instance()->addNewUri(task.m_url, savePath, task.m_downloadFilename, task.m_taskId);
        clearTableItemCheckStatus();
    }

    m_pNotaskWidget->hide();

    // 定时器打开
    if(m_pUpdateTimer->isActive() == false) {
        m_pUpdateTimer->start(2 * 1000);
    }
}

void MainFrame::onDownloadLimitChanged()
{
    QTime   currentTime = QTime::currentTime();
    QTime  periodStartTime;
    QTime  periodEndTime;
    QString downloadSpeed, uploadSpeed;

    // get_limit_speed_time(period_start_time, period_end_time);
    S_DownloadSettings settings = Settings::getInstance()->getAllSpeedLimitInfo();
    if("0" == settings.m_strType){
        Aria2RPCInterface::Instance()->setDownloadUploadSpeed("0", "0");
        return;
    }

    periodStartTime.setHMS(settings.m_strStartTime.section(":", 0, 0).toInt(),
                              settings.m_strStartTime.section(":", 1, 1).toInt(),
                              settings.m_strStartTime.section(":", 2, 2).toInt());

    periodEndTime.setHMS(settings.m_strEndTime.section(":", 0, 0).toInt(),
                            settings.m_strEndTime.section(":", 1, 1).toInt(),
                            settings.m_strEndTime.section(":", 2, 2).toInt());

    downloadSpeed = settings.m_strMaxDownload;
    uploadSpeed = settings.m_strMaxUpload;

    // 判断当前时间是否在限速时间内
    bool bInPeriod = checkIfInPeriod(&currentTime, &periodStartTime, &periodEndTime);
    if(!bInPeriod) {
        Aria2RPCInterface::Instance()->setDownloadUploadSpeed("0", "0");
    } else {
        Aria2RPCInterface::Instance()->setDownloadUploadSpeed(downloadSpeed, uploadSpeed);
    }
}

void MainFrame::onPowerOnChanged(bool isPowerOn)
{
    QString autostartDesktop = "downloadmanager.desktop";
    QString defaultDesktop = "downloadmanager.desktop";
    QString userDefaultDesktopPath = QString("%1/autostart/")
                                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    if(isPowerOn == 1) {
        QString cmd = QString("cp %1 %2").arg(UOS_DOWNLOAD_MANAGER_DESKTOP_PATH + defaultDesktop).arg(
            userDefaultDesktopPath);
        char *ch;
        QByteArray ba = cmd.toLatin1();
        ch = ba.data();
        system(ch);
    } else {
        QString cmd = QString("rm -f %1").arg(userDefaultDesktopPath + defaultDesktop);
        char *ch;
        QByteArray ba = cmd.toLatin1();
        ch = ba.data();
        system(ch);
    }
}

void MainFrame::onMaxDownloadTaskNumberChanged(int nTaskNumber)
{
    QMap<QString, QVariant> opt;
    QString value = QString("max-concurrent-downloads=%1").arg(nTaskNumber);

    modifyConfigFile("max-concurrent-downloads=", value);
    opt.insert("max-concurrent-downloads", QString().number(nTaskNumber));
    Aria2RPCInterface::Instance()->changeGlobalOption(opt);

    const QList<DataItem *> dataList = m_pDownLoadingTableView->getTableModel()->dataList();
    int activeCount = 0;
    m_bShutdownOk = true;
    for(const auto *item : dataList) {
        if(item->status == Global::Status::Active) {
            activeCount ++;
            if(activeCount > nTaskNumber){
                Aria2RPCInterface::Instance()->pause(item->gid, item->taskId);
                QTimer::singleShot(500, this, [=](){
                    Aria2RPCInterface::Instance()->unpause(item->gid, item->taskId);
                });
                QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
                S_Task_Status get_status;
                S_Task_Status downloadStatus(item->taskId,
                                             Global::Status::Paused,
                                             QDateTime::currentDateTime(),
                                             item->completedLength,
                                             item->speed,
                                             item->totalLength,
                                             item->percent,
                                             item->total,
                                             finish_time);

                if(DBInstance::getTaskStatusById(item->taskId, get_status)) {
                    DBInstance::updateTaskStatusById(downloadStatus);
                } else {
                    DBInstance::addTaskStatus(downloadStatus);
                }
            }
        }

    }
}

void MainFrame::onDisckCacheChanged(int nNum)
{
    QMap<QString, QVariant> opt;
    QString cacheNum = QString().number(nNum) + "M";

    opt.insert("disk-cache", cacheNum);
    Aria2RPCInterface::Instance()->changeGlobalOption(opt);
    QString value = "disk-cache=" + cacheNum;
    modifyConfigFile("disk-cache=", value);
}

void MainFrame::modifyConfigFile(QString configItem, QString value)
{
    QString strAll;
    QStringList strList;

    QString m_aria2configPath = QString("%1/%2/%3/aria2.conf")
                                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                .arg(qApp->organizationName())
                                .arg(qApp->applicationName());

    QFile readFile(m_aria2configPath);

    if(readFile.open((QIODevice::ReadOnly | QIODevice::Text))) {
        QTextStream stream(&readFile);
        strAll = stream.readAll();
    }
    readFile.close();
    QFile writeFile(m_aria2configPath);
    if(writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&writeFile);
        strList = strAll.split("\n");
        for(int i = 0; i < strList.count(); i++) {
            if(strList.at(i).contains(configItem)) {
                QString tempStr = strList.at(i);
                tempStr.replace(0, tempStr.length(), value);
                stream << tempStr << '\n';
            } else {
                if(i == strList.count() - 1) {
                    // 最后一行不需要换行
                    stream << strList.at(i);
                } else {
                    stream << strList.at(i) << '\n';
                }
            }
        }
    }
    writeFile.close();
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

void MainFrame::initDataItem(Global::DataItem *data, const S_Task &tbTask)
{
    data->gid = tbTask.m_gid;
    data->url = tbTask.m_url;
    data->time = "0";
    data->speed = "0kb/s";
    data->taskId = tbTask.m_taskId;
    data->fileName = tbTask.m_downloadFilename;
    data->savePath = tbTask.m_downloadPath;
    S_Task_Status taskStatus;
    DBInstance::getTaskStatusById(data->taskId, taskStatus);
    if(taskStatus.m_taskId != "") {
        data->percent = taskStatus.m_percent;
        data->Ischecked = 0;
        data->totalLength = taskStatus.m_totalLength;
        data->completedLength = taskStatus.m_compeletedLength;
        if(taskStatus.m_downloadStatus == Global::Status::Active) {
            data->status = Global::Status::Lastincomplete;
        } else {
            data->status = taskStatus.m_downloadStatus;
        }
        data->total = taskStatus.m_totalFromSource;
        if(data->status == Global::Status::Complete) {
            data->time = taskStatus.m_modifyTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
}

void MainFrame::initDelDataItem(Global::DataItem* data, Global::DelDataItem *delData)
{
    S_Task_Status taskStatus;
    DBInstance::getTaskStatusById(data->taskId, taskStatus);
    delData->taskId = data->taskId;
    delData->gid = data->gid;
    delData->url = data->url;
    delData->status = data->status;
    delData->fileName = data->fileName;
    delData->savePath = data->savePath;
    delData->deleteTime = taskStatus.m_modifyTime.toString("yyyy-MM-dd hh:mm:ss");
    delData->totalLength = data->totalLength;
    delData->completedLength = data->completedLength;
    delData->finishTime = taskStatus.m_finishTime.toString("yyyy-MM-dd hh:mm:ss");
}

void MainFrame::startAssociatedBTFile(bool status)
{
    if(status)
    {
        this->startBtAssociat();
    }
    else
    {
        this->endBtAssociat();
    }
}

void MainFrame::startBtAssociat()
{
    QString _path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list";
    QFile _readFile(_path);
    if(!_readFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"error";
        return;
    }
    QTextStream data(&_readFile);
    bool isDefault;
    bool isAdded;
    QStringList _DefaultList;
    QStringList _AddedList;
    //找到 [Default Applications] 和[Added Associations] 下面中的 application/x-bittorrent=字段
    while(!data.atEnd())
    {

        QString sLine = data.readLine();
        if(sLine == "[Default Applications]")
        {
            isDefault = true;
            isAdded = false;
        }
        else if(sLine == "[Added Associations]")
        {
            isDefault = false;
            isAdded = true;
        }
        if(isDefault)
        {
            _DefaultList.append(sLine);
        }
        if(isAdded)
        {
            _AddedList.append(sLine);
        }
    }
    //将application/x-bittorrent 字段替换为 application/x-bittorrent=uos-download.desktop。 uos-download.desktop为桌面文件夹名字
    if(!_DefaultList.isEmpty())
    {
        for (int i = 0; i < _DefaultList.size(); i++)
        {
            if(_DefaultList[i].contains("application/x-bittorrent"))
            {
                _DefaultList[i] = "application/x-bittorrent=downloadmanager.desktop;";
            }
            if(i == _DefaultList.size()-1 &&
                    !(_DefaultList[i].contains("application/x-bittorrent")))
            {
                _DefaultList.append("application/x-bittorrent=downloadmanager.desktop;");
            }
        }
    }
    else
    {
        qDebug()<<"[Default Associations] is Null";
    }
    if(!_AddedList.isEmpty())
    {
        for (int i = 0; i < _AddedList.size(); i++)
        {
            if(_AddedList[i].contains("application/x-bittorrent"))
            {
                _AddedList[i] = "application/x-bittorrent=downloadmanager.desktop;";
            }
            if(i == _AddedList.size()-1 &&
                    !(_AddedList[i].contains("application/x-bittorrent")))
            {
                _AddedList.append("application/x-bittorrent=downloadmanager.desktop;");
            }
        }
    }
    else
    {
        qDebug()<<"[Default Associations] is Null";
    }
    _readFile.close();

    //将替换以后的字符串，重新写入到文件中去
    QFile _writerFile(_path);
    _DefaultList << _AddedList;
    if(_writerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {

    }
    QTextStream _writeData(&_writerFile);

    for (int i =0 ;i < _DefaultList.size(); i++)
    {
        _writeData<<_DefaultList[i] << endl;
    }
    _writeData.flush();
    _writerFile.close();
}

void MainFrame::endBtAssociat()
{
    QString _path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list";
    QFile _readFile(_path);
    if(!_readFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open file error";
        return;
    }
    QTextStream data(&_readFile);
    QStringList _list;
    while(!data.atEnd())
    {
        QString sLine = data.readLine();
        _list.append(sLine);
    }
    for (int i = 0; i < _list.size(); i++) {
        if(_list[i].contains("application/x-bittorrent"))
        {
            _list[i] = "application/x-bittorrent=";
        }
    }
    _readFile.close();

    QFile _writerFile(_path);
    if(!_writerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"writer file error";
        return;
    }
    QTextStream _writeData(&_writerFile);
    for (int i =0 ;i < _list.size(); i++)
    {
        _writeData<<_list[i] << endl;
    }
    _writeData.flush();
    _writerFile.close();
}

void MainFrame::btNotificaitonSettings(QString head,QString text,bool isBt)
{
    // 获取免打扰模式值
    QVariant undisturbed_mode_switchbutton = Settings::getInstance()->m_pSettings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    bool downloadInfoNotify = Settings::getInstance()->getDownloadInfoSystemNotifyState();
    if(downloadInfoNotify) {
        QDBusInterface  tInterNotify("com.deepin.dde.Notification",
                                           "/com/deepin/dde/Notification",
                                           "com.deepin.dde.Notification",
                                           QDBusConnection::sessionBus());
        QList<QVariant> arg;
        QString in0(tr("Downloader"));      //下载器
        uint in1 = 101;
        QString in2;
        in2 = "downloadmanager";
        QString in3(head);
        QString in4(text);

        QStringList in5;
        QVariantMap in6;
        if(isBt)
        {
            in5<<"_cancel"<<tr("Cancel")<<"_view"<<tr("View");
            in6["x-deepin-action-_view"] = "downloadmanager";
        }

        int in7 = 5000;
        arg<<in0<<in1<<in2<<in3<<in4<<in5<<in6<<in7;
        tInterNotify.callWithArgumentList(QDBus::AutoDetect,"Notify", arg);
    }
}

void MainFrame::showWindowsForClipboard()
{
    if(this->isHidden()) {
        // 恢复窗口显示
        this->show();
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
        this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    }
    m_pLeftList->setCurrentIndex(m_pLeftList->currentIndex().sibling(0,0));
}
