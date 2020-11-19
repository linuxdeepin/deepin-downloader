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
#include <DFontSizeManager>
#include <DTitlebar>

#include <QDir>
#include <QLayout>
#include <QAction>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QClipboard>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QDesktopServices>
#include <QDebug>
#include <QDBusInterface>
#include <QSharedMemory>
#include <QMimeDatabase>
#include <QUuid>

#include <unistd.h>

#include "aria2rpcinterface.h"
#include "aria2const.h"
#include "tableView.h"
#include "topButton.h"
#include "createtaskwidget.h"
#include "tableModel.h"
#include "global.h"
#include "clipboardtimer.h"
#include "messagebox.h"
#include "btinfodialog.h"
#include "../database/dbinstance.h"
#include "tabledatacontrol.h"
#include "analysisurl.h"
#include "diagnostictool.h"
#include "settings.h"
#include "headerView.h"

using namespace Global;

#define UOS_DOWNLOAD_MANAGER_DESKTOP_PATH "/usr/share/applications/"

MainFrame::MainFrame(QWidget *parent)
    : DMainWindow(parent)
    , m_TaskWidget(new CreateTaskWidget())
    , m_CurrentTab(downloadingTab)
    , m_CheckItem(nullptr)
    , m_CheckIndex(QModelIndex())
{
    init();
    initTray();
    initDbus();
    initAria2();
    updateDHTFile();
    initConnection();
    initTabledata();
    setPaletteType();
    QString clipboradStr = QApplication::clipboard()->text();
    if (Settings::getInstance()->getIsClipboradStart(clipboradStr)) {
        m_Clipboard->checkClipboardHasUrl();
    }
}

MainFrame *MainFrame::instance()
{
    static MainFrame *m = nullptr;
    if (m == nullptr) {
        m = new MainFrame;
    }
    return m;
}

MainFrame::~MainFrame()
{
}

void MainFrame::init()
{
    setObjectName("ariaMain");
    setMinimumSize(838, 636);
    setTitlebarShadowEnabled(true);
    setAcceptDrops(true);
    // 添加设置界面
    m_TaskWidget->setObjectName("newTaskWidget");
    DMenu *pSettingsMenu = new DMenu;
    pSettingsMenu->setObjectName("settingsMenu");
    m_SettingAction = new QAction(tr("Settings"), this);
    m_SettingAction->setObjectName("settingAction");
    pSettingsMenu->addAction(m_SettingAction);

    QAction *pFinishAction = new QAction(tr("When completed"), this);
    DMenu *pFinishMenu = new DMenu(tr("When completed"), this);
    m_ShutdownAct = new QAction(tr("Shut down"), this);
    m_ShutdownAct->setCheckable(true);
    m_SleepAct = new QAction(tr("Hibernate"), this);
    m_SleepAct->setCheckable(true);
    m_QuitProcessAct = new QAction(tr("Exit Downloader"), this);
    m_QuitProcessAct->setCheckable(true);

    pFinishMenu->addAction(m_ShutdownAct);
    pFinishMenu->addAction(m_SleepAct);
    pFinishMenu->addAction(m_QuitProcessAct);
    pFinishAction->setMenu(pFinishMenu);
    pSettingsMenu->addAction(pFinishAction);

    QAction *pDiagnosticAction = new QAction(tr("Diagnostic tool"), this);
    connect(pDiagnosticAction, &QAction::triggered, this, &MainFrame::showDiagnosticTool);
    pSettingsMenu->addAction(pDiagnosticAction);

    titlebar()->setMenu(pSettingsMenu);
    m_ToolBar = new TopButton(this);
    titlebar()->setCustomWidget(m_ToolBar, false);
    titlebar()->setObjectName("titlebar");

    QPalette p;
    p.setColor(QPalette::Background, QColor(255, 255, 255));

    QFrame *pMainWidget = new QFrame;
    pMainWidget->setFrameShape(QFrame::NoFrame);
    QHBoxLayout *pMainHLayout = new QHBoxLayout(pMainWidget);
    setCentralWidget(pMainWidget);
    pMainHLayout->setContentsMargins(0, 0, 0, 0);
    pMainHLayout->setSpacing(0);

    m_DownLoadingTableView = new TableView(tableviewFlag::downloading);
    m_DownLoadingTableView->setObjectName("downloadTableView");
    m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
    m_DownLoadingTableView->setColumnHidden(4, true);

    m_RecycleTableView = new TableView(tableviewFlag::recycle);
    m_RecycleTableView->setObjectName("recycleTableView");
    m_RecycleTableView->verticalHeader()->setDefaultSectionSize(48);
    m_RecycleTableView->setColumnHidden(3, true);
    m_DownLoadingTableView->getTableControl()->setRecycleTable(m_RecycleTableView);
    m_LeftWidget = new QWidget;
    m_LeftWidget->setAutoFillBackground(true);
    QVBoxLayout *pLeftLayout = new QVBoxLayout(m_LeftWidget);
    m_LeftWidget->setPalette(p);
    m_LeftWidget->setFixedWidth(155);
    pLeftLayout->setContentsMargins(10, 0, 10, 0);

    m_RightWidget = new QWidget;
    m_NotaskWidget = new QWidget;
    m_NotaskWidget->setAutoFillBackground(true);
    m_NotaskWidget->setPalette(p);

    QVBoxLayout *pnotaskWidgetlayout = new QVBoxLayout(m_NotaskWidget);
    pnotaskWidgetlayout->setContentsMargins(10, 0, 0, 0);

    QFont lableFont;
    lableFont.setPointSize(15);
    lableFont.setBold(QFont::ExtraLight);
    //lableFont.setFamily("T5");
    m_NotaskLabel = new Dtk::Widget::DLabel();
    m_NotaskLabel->setFont(lableFont);
    m_NotaskLabel->setWindowOpacity(0.2);
    m_NotaskLabel->setText(tr("No download tasks"));
    m_NotaskLabel->setAlignment(Qt::AlignHCenter);
    m_NotaskLabel->setForegroundRole(DPalette::PlaceholderText);
    pnotaskWidgetlayout->addWidget(m_NotaskLabel);

    m_NoResultlabel = new Dtk::Widget::DLabel();
    m_NoResultlabel->setFont(lableFont);
    m_NoResultlabel->setText(tr("No search result"));
    m_NoResultlabel->setWindowOpacity(0.2);
    m_NoResultlabel->setAlignment(Qt::AlignHCenter);
    m_NoResultlabel->setForegroundRole(DPalette::TextTitle);
    m_NoResultlabel->hide();
    pnotaskWidgetlayout->addWidget(m_NoResultlabel);

    m_NotaskTipLabel = new DLabel();
    QFont notaskTipLabelFont;
    notaskTipLabelFont.setPointSize(13);
    notaskTipLabelFont.setFamily("T6");
    m_NotaskTipLabel->setFont(notaskTipLabelFont);
    m_NotaskTipLabel->setText(tr("Click + to create new task"));
    m_NotaskTipLabel->setAlignment(Qt::AlignHCenter);
    m_NotaskTipLabel->setForegroundRole(DPalette::PlaceholderText);
    pnotaskWidgetlayout->addWidget(m_NotaskTipLabel);
    pnotaskWidgetlayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_RightWidget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);

    m_RightStackwidget = new QStackedWidget(this);
    m_RightStackwidget->setCurrentIndex(0);

    m_TaskNumWidget = new QWidget;
    m_TaskNumWidget->setFixedHeight(30);
    //m_pTaskNumWidget->setPalette(pa);
    QHBoxLayout *TaskNumWidgetlayout = new QHBoxLayout(m_TaskNumWidget);
    TaskNumWidgetlayout->setMargin(0);
    m_TaskNum = new QLabel(tr("0 task"));
    m_TaskNum->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_TaskNum->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
    TaskNumWidgetlayout->addWidget(m_TaskNum);
    m_RightStackwidget->addWidget(m_DownLoadingTableView);
    m_RightStackwidget->addWidget(m_RecycleTableView);
    pRightLayout->addWidget(m_RightStackwidget);
    pRightLayout->addWidget(m_NotaskWidget);
    pRightLayout->addWidget(m_TaskNumWidget);
    pRightLayout->setSpacing(0);

    pMainHLayout->addWidget(m_LeftWidget);
    pMainHLayout->addWidget(m_RightWidget);

    m_LeftList = new DListView;
    m_LeftList->setObjectName("leftList");
    m_LeftList->setItemSpacing(0);
    m_LeftList->setItemSize(QSize(112, 40));
    m_LeftList->setItemMargins(QMargins(10, 2, 5, 2));
    m_LeftList->setIconSize(QSize(14, 14));
    QFont font;
    font.setFamily("Source Han Sans");
    font.setPixelSize(14);
    m_LeftList->setFont(font);
    QStandardItemModel *pleftlistModel = new QStandardItemModel(this);

    m_DownloadingItem = new DStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("Downloading"));
    m_DownloadFinishItem = new DStandardItem(QIcon::fromTheme("dcc_print_done"), tr("Completed"));
    m_RecycleItem = new DStandardItem(QIcon::fromTheme("dcc_list_delete"), tr("Trash"));
    m_DownloadingItem->setFontSize(DFontSizeManager::T6);
    m_DownloadFinishItem->setFontSize(DFontSizeManager::T6);
    m_RecycleItem->setFontSize(DFontSizeManager::T6);
    m_DownloadingItem->setEditable(false);
    m_DownloadFinishItem->setEditable(false);
    m_RecycleItem->setEditable(false);
    pleftlistModel->appendRow(m_DownloadingItem);
    pleftlistModel->appendRow(m_DownloadFinishItem);
    pleftlistModel->appendRow(m_RecycleItem);
    m_LeftList->setModel(pleftlistModel);
    pLeftLayout->addWidget(m_LeftList, 0);

    m_LeftList->setCurrentIndex(pleftlistModel->index(0, 0));

    m_Clipboard = new ClipboardTimer; // 获取当前剪切板
    m_UpdateTimer = new QTimer(this);
    m_TrayClickTimer = new QTimer(this);
    m_CurOpenBtDialogPath = "";

    //   QApplication *pApp = dynamic_cast<QApplication *>(QApplication::instance()) ;
    //    connect(pApp, &QApplication::lastWindowClosed,
    //      this, [ & ]() {
    //          auto quit = Settings::getInstance()->getCloseMainWindowSelected();
    //          if (quit == 1) {
    //              qApp->quit();
    //          }
    //    });
}

void MainFrame::initTray()
{
    QIcon tryIcon = QIcon(":/icons/icon/downloader5.svg");

    m_SystemTray = new QSystemTrayIcon(this);
    m_SystemTray->setObjectName("systemTray");
    m_SystemTray->setIcon(tryIcon);
    m_SystemTray->setToolTip(tr("Downloader"));

    QAction *pShowMainAct = new QAction(tr("Show main window"), this);
    QAction *pNewDownloadAct = new QAction(tr("New task"), this);
    QAction *pStartAllAct = new QAction(tr("Continue all tasks"), this);
    QAction *pPauseAllAct = new QAction(tr("Pause all tasks"), this);
    QMenu *pFinishMenu = new QMenu(tr("When completed"), this);
    pFinishMenu->addAction(m_ShutdownAct);
    pFinishMenu->addAction(m_SleepAct);
    pFinishMenu->addAction(m_QuitProcessAct);
    QAction *pQuitAct = new QAction(tr("Exit"), this);

    QMenu *pTrayMenu = new QMenu(this);
    pTrayMenu->addAction(pShowMainAct);
    pTrayMenu->addAction(pNewDownloadAct);
    pTrayMenu->addAction(pStartAllAct);
    pTrayMenu->addAction(pPauseAllAct);
    pTrayMenu->addMenu(pFinishMenu);
    pTrayMenu->addAction(pQuitAct);

    // 连接信号与槽
    connect(pShowMainAct, &QAction::triggered, [=]() {
        showNormal();
        setWindowState(Qt::WindowActive);
        activateWindow();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    });
    connect(pNewDownloadAct, &QAction::triggered, [=]() {
        createNewTask("");
    });

    connect(m_ShutdownAct, &QAction::triggered, [=](bool checked) {
        if (checked) {
            m_SleepAct->setChecked(false);
            m_QuitProcessAct->setChecked(false);
        }
    });
    connect(m_SleepAct, &QAction::triggered, [=](bool checked) {
        if (checked) {
            m_ShutdownAct->setChecked(false);
            m_QuitProcessAct->setChecked(false);
        }
    });
    connect(m_QuitProcessAct, &QAction::triggered, [=](bool checked) {
        if (checked) {
            m_ShutdownAct->setChecked(false);
            m_SleepAct->setChecked(false);
        }
    });

    connect(pStartAllAct, &QAction::triggered, [=]() {
        const QList<DownloadDataItem *> selectList = m_DownLoadingTableView->getTableModel()->renderList();
        foreach (DownloadDataItem *pData, selectList) {
            if (pData->status != Global::DownloadJobStatus::Complete)
                continueDownload(pData);
        }

        Aria2RPCInterface::instance()->unpauseAll();
        if (m_UpdateTimer->isActive() == false) {
            m_UpdateTimer->start(m_timeInterval);
        }
    });
    connect(pPauseAllAct, &QAction::triggered, [=]() {
        Aria2RPCInterface::instance()->pauseAll();
        //        if(m_pUpdatetimer->isActive()) {
        //            m_pUpdatetimer->stop();
        //        }
    });
    connect(pQuitAct, &QAction::triggered, this, &MainFrame::onTrayQuitClick);
    connect(m_SystemTray, &QSystemTrayIcon::activated, this, &MainFrame::onActivated);
    m_SystemTray->setContextMenu(pTrayMenu);
    m_SystemTray->show();
}

void MainFrame::updateDHTFile()
{
    QFileInfo f(QDir::homePath() + "/.config/uos/downloader/dht.dat");
    QDateTime t = f.fileTime(QFileDevice::FileModificationTime);

    QFileInfo f2(QDir::homePath() + "/.config/uos/downloader/dht6.dat");
    QDateTime t2 = f2.fileTime(QFileDevice::FileModificationTime);
    if (t.date() == QDate::currentDate() && t2.date() == QDate::currentDate()) {
        return;
    }
    QFile::remove(QDir::homePath() + "/.config/uos/downloader/dht.dat");
    QFile::remove(QDir::homePath() + "/.config/uos/downloader/dht6.dat");
    //    QMap<QString, QVariant> opt;
    //    opt.insert("dir", QString(QDir::homePath() + "/.config/uos/downloader"));
    //    opt.insert("out", "dht.dat");
    //    Aria2RPCInterface::instance()->addUri("https://github.com/P3TERX/aria2.conf/raw/master/dht.dat",
    //                                          opt, "dht.dat");

    //    QMap<QString, QVariant> opt2;
    //    opt2.insert("dir", QString(QDir::homePath() + "/.config/uos/downloader"));
    //    opt2.insert("out", "dht6.dat");
    //    Aria2RPCInterface::instance()->addUri("https://github.com/P3TERX/aria2.conf/raw/master/dht6.dat",
    //                                          opt2, "dht6.dat");

    QString dhtpah = QDir::homePath() + "/.config/uos/downloader/";
    QProcess p;
    p.startDetached("curl https://github.com/P3TERX/aria2.conf/raw/master/dht6.dat -o" + dhtpah + "dht6.dat -O");

    QProcess p2;
    p2.startDetached("curl https://github.com/P3TERX/aria2.conf/raw/master/dht.dat -o" + dhtpah + "dht.dat -O");
}

void MainFrame::initConnection()
{
    connect(m_DownLoadingTableView, &TableView::HeaderStatechanged, this, &MainFrame::onHeaderStatechanged);
    connect(m_DownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::onContextMenu, Qt::QueuedConnection);
    connect(m_DownLoadingTableView, &TableView::pressed, this, &MainFrame::onTableItemSelected);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::RedownloadJob, this, &MainFrame::onRedownload);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::AutoDownloadBt, this, &MainFrame::OpenBt);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::removeFinished, this, &MainFrame::onRemoveFinished);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::whenDownloadFinish, this, &MainFrame::onDownloadFinish);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::addMaxDownloadTask, this, [=](int num) {
        onMaxDownloadTaskNumberChanged(num);
    });
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::DownloadUnusuaHttpJob, this, &MainFrame::onDownloadNewUrl);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::DownloadUnusuaBtJob, this, &MainFrame::onDownloadNewTorrent);
    connect(m_DownLoadingTableView->getTableModel(), &TableModel::CheckChange, this, &MainFrame::onCheckChanged);
    connect(m_DownLoadingTableView, &TableView::doubleClicked, this, &MainFrame::onTableViewItemDoubleClicked);

    connect(m_RecycleTableView, &TableView::HeaderStatechanged, this, &MainFrame::onHeaderStatechanged);
    connect(m_RecycleTableView, &TableView::customContextMenuRequested, this, &MainFrame::onContextMenu, Qt::QueuedConnection);
    connect(m_RecycleTableView, &TableView::pressed, this, &MainFrame::onTableItemSelected);
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::RedownloadJob, this, &MainFrame::onRedownload);
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::AutoDownloadBt, this, &MainFrame::OpenBt);
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::removeFinished, this, &MainFrame::onRemoveFinished);
    connect(m_RecycleTableView->getTableModel(), &TableModel::CheckChange, this, &MainFrame::onCheckChanged);
    connect(m_RecycleTableView, &TableView::doubleClicked, this, &MainFrame::onTableViewItemDoubleClicked);

    connect(this, &MainFrame::isHeaderChecked, m_DownLoadingTableView, &TableView::isCheckHeader);
    connect(this, &MainFrame::saveNameBeforChangeList, m_DownLoadingTableView, &TableView::onListchanged);
    connect(this, &MainFrame::isHeaderChecked, m_RecycleTableView, &TableView::isCheckHeader);

    connect(m_SettingAction, &QAction::triggered, this, &MainFrame::onSettingsMenuClicked);
    connect(m_Clipboard, &ClipboardTimer::sendClipboardTextChange, this, &MainFrame::onClipboardDataChanged);
    connect(m_LeftList, &DListView::pressed, this, &MainFrame::onListClicked);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::onPalettetypechanged);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MainFrame::onPalettetypechanged);

    //connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &MainFrame::onPalettetypechanged);
    connect(m_UpdateTimer, &QTimer::timeout, this, &MainFrame::onUpdateMainUI);

    connect(m_ToolBar, &TopButton::newDownloadBtnClicked, this, &MainFrame::onNewBtnClicked);
    connect(m_ToolBar, &TopButton::SearchEditTextChange, this, &MainFrame::onSearchEditTextChanged);
    connect(m_ToolBar, &TopButton::startDownloadBtnClicked, this, &MainFrame::onStartDownloadBtnClicked);
    connect(m_ToolBar, &TopButton::pauseDownloadBtnClicked, this, &MainFrame::onPauseDownloadBtnClicked);
    connect(m_ToolBar, &TopButton::deleteDownloadBtnClicked, this, &MainFrame::onDeleteDownloadBtnClicked);
    connect(this, &MainFrame::tableChanged, m_ToolBar, &TopButton::onTableChanged);
    connect(this, &MainFrame::redownload, this, &MainFrame::onRedownload, Qt::QueuedConnection);

    connect(Settings::getInstance(), &Settings::downloadSettingsChanged, this, &MainFrame::onDownloadLimitChanged);
    connect(Settings::getInstance(), &Settings::poweronChanged, this, &MainFrame::onPowerOnChanged);
    connect(Settings::getInstance(), &Settings::maxDownloadTaskNumberChanged, this, [=](int num) {
        onMaxDownloadTaskNumberChanged(num);
    });
    connect(Settings::getInstance(), &Settings::disckCacheChanged, this, &MainFrame::onDisckCacheChanged);
    connect(Settings::getInstance(), &Settings::startAssociatedBTFileChanged, this, &MainFrame::onIsStartAssociatedBTFile);
    connect(Settings::getInstance(), &Settings::autoDownloadBySpeedChanged,
            this, &MainFrame::onAutoDownloadBySpeed);
    connect(Settings::getInstance(), &Settings::autoSortBySpeedChanged,
            this, [=](bool state) {
                if (state) {
                    m_DownLoadingTableView->getTableHeader()->setSortIndicator(5, Qt::AscendingOrder);
                } else {
                    m_DownLoadingTableView->getTableHeader()->setSortIndicator(6, Qt::AscendingOrder);
                }
            });

    connect(m_TaskWidget, &CreateTaskWidget::downloadWidgetCreate, this, &MainFrame::onParseUrlList);
    connect(m_TaskWidget, &CreateTaskWidget::downLoadTorrentCreate, this, &MainFrame::onDownloadNewTorrent, Qt::UniqueConnection);
}

void MainFrame::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (QSystemTrayIcon::Trigger == reason) {
        if (isVisible()) {
            if (isMinimized()) {
                setWindowState(Qt::WindowActive);
                activateWindow();
                showNormal();
                //hide();
                //show();
            } else {
                showMinimized();
            }
        } else {
            showNormal();
        }
    }

    //    if(reason == QSystemTrayIcon::ActivationReason::Trigger) {
    //        //if(m_TrayClickTimer->isActive()){
    //            if(isHidden()) {
    //                // 恢复窗口显示
    //                show();
    //                setWindowState(Qt::WindowActive);
    //                activateWindow();
    //                setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    //            } else {
    //                hide();
    //            }
    //        //} else {
    //            //m_TrayClickTimer->start(200);
    //            //m_TrayClickTimer->setSingleShot(true);
    //        //}
    //        return;
    //    }
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    m_SystemTray->show();
    if (Settings::getInstance()->getIsShowTip()) {
        MessageBox msg;
        connect(&msg, &MessageBox::closeConfirm, this, &MainFrame::onMessageBoxConfirmClick);
        msg.setExit();
        msg.exec();
    } else {
        onMessageBoxConfirmClick();
    }
    // setWindowFlags(Qt::Tool);
    event->ignore();
    // DMainWindow::closeEvent(event);
}

void MainFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    setPaletteType();
}

void MainFrame::createNewTask(QString url)
{
    if (Settings::getInstance()->getNewTaskShowMainWindowState()) {
        activateWindow();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

        if (isHidden()) {
            QDesktopWidget *desktop = QApplication::desktop();
            move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
            show();
        }
    }
    m_TaskWidget->setUrl(url);
    if (isNetConnect()) {
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    if (m_TaskWidget->isHidden()) {
        m_TaskWidget->move(pos().x() + this->width() / 2 - m_TaskWidget->width() / 2,
                           pos().y() + this->height() / 2 - m_TaskWidget->height() / 2);
    }
    m_TaskWidget->exec();
}

void MainFrame::onTrayQuitClick(bool force)
{
    if (!m_ShutdownOk && !force) {
        static bool msgBoxFlg = true;
        static MessageBox msgBox;
        if (msgBox.isVisible()) {
            return;
        }
        if (msgBoxFlg) {
            QString title = tr("Are you sure you want to exit? \nDownloading tasks will be interrupted.");
            msgBox.setWarings(title, tr("sure"), tr("cancel"));
            msgBoxFlg = false;
        }
        int rs = msgBox.exec();
        if (rs != DDialog::Accepted) {
            return;
        }
    }
    m_DownLoadingTableView->getTableControl()->saveDataBeforeClose();
    m_RecycleTableView->getTableControl()->saveDataBeforeClose();
    Aria2RPCInterface::instance()->shutdown();
    // qApp->quit();
}

void MainFrame::onMessageBoxConfirmClick()
{
    if (Settings::getInstance()->getCloseMainWindowSelected()) {
        onTrayQuitClick(false);
    } else {
        //showMinimized();
        hide();
    }
}

void MainFrame::initAria2()
{
    Aria2RPCInterface::instance()->init(); // 启动Aria2RPCInterface::instance()
    connect(Aria2RPCInterface::instance(), SIGNAL(RPCSuccess(QString, QJsonObject)), this,
            SLOT(onRpcSuccess(QString, QJsonObject)));
    connect(Aria2RPCInterface::instance(), SIGNAL(RPCError(QString, QString, int, QJsonObject)), this,
            SLOT(onRpcError(QString, QString, int, QJsonObject)));
    onDownloadLimitChanged();
    onMaxDownloadTaskNumberChanged(Settings::getInstance()->getMaxDownloadTaskNumber());
    qDebug() << "MainFrame initAria2 Finished";
}

void MainFrame::initTabledata()
{
    //m_ptableDataControl->initTabledata();
    QList<TaskInfo> list;
    TaskInfo tbTask;

    DBInstance::getAllTask(list);
    for (int i = 0; i < list.length(); i++) {
        DownloadDataItem *data = new DownloadDataItem;
        initDataItem(data, list.at(i));
        if (data->status != Global::DownloadJobStatus::Removed) {
            if (data->status == Global::DownloadJobStatus::Lastincomplete) {
                m_NotaskWidget->hide();
                if (Settings::getInstance()->getAutostartUnfinishedTaskState()) { //启动后自动下载未完成的任务
                    startDownloadTask(data);
                }
            }
            m_DownLoadingTableView->getTableModel()->append(data);
        } else {
            DeleteDataItem *deldata = new DeleteDataItem;
            initDelDataItem(data, deldata);
            m_RecycleTableView->getTableModel()->append(deldata);
        }
    }
    m_DownLoadingTableView->getTableModel()->switchDownloadingMode();
    m_DownLoadingTableView->refreshTableView(0);
    if (Settings::getInstance()->getAutoSortBySpeed()) {
        m_DownLoadingTableView->getTableHeader()->setSortIndicator(5, Qt::AscendingOrder);
    } else {
        m_DownLoadingTableView->getTableHeader()->setSortIndicator(6, Qt::AscendingOrder);
    }
    setTaskNum();
}

void MainFrame::setTaskNum()
{
    const QList<DownloadDataItem *> &renderList = m_DownLoadingTableView->getTableModel()->renderList();
    const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
    QString activeNum;

    if (m_CurrentTab == downloadingTab) {
        int i = 0;
        int activeCount = 0;
        for (const auto *item : renderList) {
            if ((item->status == Global::DownloadJobStatus::Active) || (item->status == Global::DownloadJobStatus::Paused) || (item->status == Global::DownloadJobStatus::Lastincomplete) || (item->status == Global::DownloadJobStatus::Error) || (item->status == Global::Waiting)) {
                if (!m_DownLoadingTableView->isRowHidden(i)) {
                    ++activeCount;
                }
            }
            i++;
        }
        activeNum = QString::number(activeCount) + tr(" item tasks");
        if (activeCount == 0) {
            m_NotaskWidget->show();
        } else {
            m_NotaskWidget->hide();
        }
    } else if (m_CurrentTab == finishTab) {
        int j = 0;
        int finishCount = 0;
        for (const auto *item : renderList) {
            if (item->status == Global::DownloadJobStatus::Complete) {
                if (!m_DownLoadingTableView->isRowHidden(j)) {
                    ++finishCount;
                }
            }
            j++;
        }
        activeNum = QString::number(finishCount) + tr(" files");
        if (finishCount == 0) {
            m_NotaskWidget->show();
        } else {
            m_NotaskWidget->hide();
        }
    } else if (m_CurrentTab == recycleTab) {
        int k = 0;
        int recycleCount = 0;
        for (const auto *item : recycleList) {
            Q_UNUSED(item);
            if (!m_RecycleTableView->isRowHidden(k)) {
                ++recycleCount;
            }
            k++;
        }
        activeNum = QString::number(recycleCount) + tr(" files");
        if (recycleCount == 0) {
            m_NotaskWidget->show();
        } else {
            m_NotaskWidget->hide();
        }
    }
    m_TaskNum->setText(activeNum);
}

void MainFrame::setPaletteType()
{
    m_LeftList->setPalette(DGuiApplicationHelper::instance()->applicationPalette());

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        DPalette deepthemePalette;
        deepthemePalette.setBrush(DPalette::Background,
                                  DGuiApplicationHelper::instance()->applicationPalette().base());
        m_LeftWidget->setPalette(deepthemePalette);
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        DPalette label_palette;
        label_palette.setBrush(DPalette::Text,
                               DGuiApplicationHelper::instance()->applicationPalette().placeholderText());

        m_NotaskTipLabel->setPalette(DGuiApplicationHelper::instance()->applicationPalette());
        DPalette tableviewPalette;

        tableviewPalette.setBrush(DPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());

        m_DownLoadingTableView->setPalette(tableviewPalette);
        m_RecycleTableView->setPalette(tableviewPalette);
        m_NotaskWidget->setPalette(tableviewPalette);

        DPalette palette;
        QColor c = DGuiApplicationHelper::instance()->applicationPalette().base().color();
        c.setAlpha(70);
        palette.setColor(DPalette::Background, c);
        m_TaskNumWidget->setPalette(palette);
        m_NotaskLabel->setWindowOpacity(0.2);
        m_DownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading_dark"));
        m_DownloadFinishItem->setIcon(QIcon::fromTheme("dcc_print_done_dark"));
        m_RecycleItem->setIcon(QIcon::fromTheme("dcc_list_delete_dark"));
        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText,
                                 DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_TaskNum->setPalette(notaskTipLabelP);
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        DPalette p;
        p.setBrush(DPalette::Background,
                   DGuiApplicationHelper::instance()->applicationPalette().base());
        DPalette tableviewPalette;
        tableviewPalette.setBrush(DPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().window());
        m_LeftWidget->setPalette(p);
        m_NotaskWidget->setPalette(tableviewPalette);
        m_DownLoadingTableView->setPalette(tableviewPalette);
        m_RecycleTableView->setPalette(tableviewPalette);
        DPalette palette;
        QColor c = DGuiApplicationHelper::instance()->applicationPalette().base().color();
        c.setAlpha(70);
        palette.setColor(DPalette::Background, c);
        m_TaskNumWidget->setPalette(palette);
        m_NotaskLabel->setWindowOpacity(0.2);
        m_DownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading"));
        m_DownloadFinishItem->setIcon(QIcon::fromTheme("dcc_print_done"));
        m_RecycleItem->setIcon(QIcon::fromTheme("dcc_list_delete"));
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());

        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText, DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_TaskNum->setPalette(notaskTipLabelP);
    }
}

void MainFrame::onSettingsMenuClicked()
{
    DSettingsDialog SettingsDialog;

    SettingsDialog.widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    SettingsDialog.widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    SettingsDialog.widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    SettingsDialog.widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);
    SettingsDialog.widgetFactory()->registerWidget("diskcacheInfo", Settings::createDiskCacheSettiingLabelHandle);
    SettingsDialog.widgetFactory()->registerWidget("downloadspeedlimitsetting",
                                                   Settings::createDownloadSpeedLimitSettiingHandle);
    SettingsDialog.widgetFactory()->registerWidget("notificationsSettiing",
                                                   Settings::createNotificationsSettiingHandle);
    SettingsDialog.widgetFactory()->registerWidget("autodownloadbyspeed",
                                                   Settings::createAutoDownloadBySpeedHandle);
    SettingsDialog.widgetFactory()->registerWidget("prioritydownloadbysize",
                                                   Settings::createPriorityDownloadBySizeHandle);
    SettingsDialog.updateSettings("Settings", Settings::getInstance()->m_settings);

    Settings::getInstance()->setAutoStart(isAutoStart());

    SettingsDialog.exec();
    Settings::getInstance()->m_settings->sync();
}

void MainFrame::onClipboardDataChanged(QString url)
{
    if (!m_CopyUrlFromLocal) {
        createNewTask(url);
    } else {
        m_CopyUrlFromLocal = false;
    }
}

void MainFrame::OpenBt(QString url)
{
    if (isNetConnect()) {
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    bool bIsBt = Settings::getInstance()->getStartAssociatedBTFileState();
    if (!bIsBt) {
        return;
    }
    QString savePath = Settings::getInstance()->getDownloadSavePath();
    qDebug() << "url: " << url << "exists:  " << QFile::exists(url);

    qDebug() << "savePath: " << savePath;
    BtInfoDialog btDiag(url, savePath); // torrent文件路径
    QMap<QString, QVariant> opt;
    QString infoName;
    QString infoHash;
    bool isOneClick = Settings::getInstance()->getOneClickDownloadState();
    if (isOneClick) {
        if (!btDiag.onBtnOK()) {
            return;
        }
        btDiag.getBtInfo(opt, infoName, infoHash);
        bool ret = onDownloadNewTorrent(url, opt, infoName, infoHash);
        if (ret) {
            btNotificaitonSettings(tr("Download"), QString(tr("%1 downloading...")).arg(infoName), true);
        }
        return;
    }
    if (Settings::getInstance()->getNewTaskShowMainWindowState()) {
        Raise();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        btDiag.move(pos().x() + this->width() / 2 - btDiag.width() / 2, pos().y() + this->height() / 2 - btDiag.height() / 2);
    } else {
        if (windowState() == Qt::WindowMinimized) {
            hide();
        }
    }
    int ret = btDiag.exec();

    //    if (!Settings::getInstance()->getNewTaskShowMainWindowState()) {
    //        setWindowState(windowState() & Qt::WindowMinimized);
    //        showMinimized();
    //    }
    if (ret == QDialog::Accepted) {
        btDiag.getBtInfo(opt, infoName, infoHash);
        onDownloadNewTorrent(url, opt, infoName, infoHash);
    }
    //clearSharedMemory();
}

void MainFrame::onListClicked(const QModelIndex &index)
{
    QObject *obj = QObject::sender();
    //QString str = obj->objectName();
    emit saveNameBeforChangeList();
    m_ToolBar->enablePauseBtn(false);
    m_ToolBar->enableStartBtn(false);
    m_ToolBar->enableDeleteBtn(false);
    m_CurrentTab = static_cast<CurrentTab>(index.row());
    if ((index.row() == 0) || (index.row() == 1)) {
        m_RightStackwidget->setCurrentIndex(0);
        m_DownLoadingTableView->refreshTableView(index.row());
        m_DownLoadingTableView->horizontalHeader()->reset();
        m_DownLoadingTableView->reset(true);
        if (index.row() == 1) {
            //m_pDownLoadingTableView->setFocus();
            //m_pDownLoadingTableView->getTableModel()->setData()
            //m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
            m_NotaskWidget->show();
            m_NotaskLabel->setText(tr("No finished tasks"));
            m_NotaskTipLabel->hide();
            m_NoResultlabel->hide();
            m_DownLoadingTableView->getTableHeader()->setSortIndicator(4, Qt::AscendingOrder);
        } else {
            //m_pDownLoadingTableView->setFocus();
            //m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(48);
            m_NotaskLabel->setText(tr("No download tasks"));
            m_NotaskWidget->show();
            m_NotaskTipLabel->show();
            m_NoResultlabel->hide();
            if (Settings::getInstance()->getAutoSortBySpeed()) {
                m_DownLoadingTableView->getTableHeader()->setSortIndicator(5, Qt::AscendingOrder);
            } else {
                m_DownLoadingTableView->getTableHeader()->setSortIndicator(6, Qt::AscendingOrder);
            }
        }
    } else {
        if (obj != nullptr) {
            QTimer::singleShot(100, [=]() { // 其他列表为空的时候切换到回收站列表，会显示不全，刷新两次就可以
                onListClicked(index);
            });
        }

        m_RightStackwidget->setCurrentIndex(1);
        //m_pRecycleTableView->setFocus();
        if (m_RecycleTableView->getTableModel()->recyleList().size() <= 0) {
            m_NotaskWidget->show();
        }
        m_NotaskLabel->setText(tr("No deleted tasks"));
        m_NotaskTipLabel->hide();
        m_NoResultlabel->hide();
        m_RecycleTableView->getTableHeader()->setSortIndicator(4, Qt::AscendingOrder);

        m_RecycleTableView->resize(m_RecycleTableView->width() - 1, m_RecycleTableView->height() - 1);
        m_RecycleTableView->resize(m_RecycleTableView->width() + 1, m_RecycleTableView->height() + 1);
        m_RecycleTableView->update();
        m_RecycleTableView->horizontalHeader()->reset();
        m_RecycleTableView->reset(true);
        m_RecycleTableView->repaint();
    }
    clearTableItemCheckStatus();
    emit isHeaderChecked(false);
    setTaskNum();
    onSearchEditTextChanged(m_ToolBar->getSearchText());
    emit tableChanged(index.row());
}

void MainFrame::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    Q_UNUSED(type);
    setPaletteType();
}

void MainFrame::onHeaderStatechanged(bool isChecked)
{
    // ToolBar禁用按钮联动：表头全选复选框状体变化 begin
    int cnt = (m_CurrentTab == recycleTab ? m_RecycleTableView->getTableModel()->rowCount()
                                          : m_DownLoadingTableView->getTableModel()->rowCount());

    int activeCount = 0;
    int pauseCount = 0;
    int errorCount = 0;
    int hasFileCount = 0;
    if (m_CurrentTab == downloadingTab && isChecked) {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); i++) {
            DownloadDataItem *pDownloadItem = selectList.at(i);
            if (pDownloadItem->status == Global::DownloadJobStatus::Active) {
                ++activeCount;
            }
            if ((pDownloadItem->status == Global::DownloadJobStatus::Paused) || (pDownloadItem->status == Global::DownloadJobStatus::Lastincomplete)) {
                ++pauseCount;
            }
            if (pDownloadItem->status == Global::DownloadJobStatus::Error) {
                ++errorCount;
            }
        }
    } else if (m_CurrentTab == recycleTab && isChecked) {
        const QList<DeleteDataItem *> &selectList = m_RecycleTableView->getTableModel()->recyleList();
        for (int i = 0; i < selectList.size(); i++) {
            DeleteDataItem *pItem = selectList.at(i);
            if (QFile::exists(pItem->savePath)) {
                ++hasFileCount;
            }
        }
    }
    if (cnt > 0) {
        if (m_CurrentTab == downloadingTab) {
            if (isChecked) {
                if (activeCount) {
                    m_ToolBar->enablePauseBtn(isChecked);
                }
                if (pauseCount || errorCount) {
                    m_ToolBar->enableStartBtn(isChecked);
                }
            } else {
                m_ToolBar->enablePauseBtn(isChecked);
                m_ToolBar->enableStartBtn(isChecked);
            }
            m_ToolBar->enableDeleteBtn(isChecked);
        } else {
            m_ToolBar->enableStartBtn(false);
            m_ToolBar->enablePauseBtn(false);
            m_ToolBar->enableDeleteBtn(isChecked);
        }
        if (m_CurrentTab == recycleTab) {
            if (hasFileCount && isChecked) {
                m_ToolBar->enablePauseBtn(true);
            } else {
                m_ToolBar->enablePauseBtn(false);
            }
            m_ToolBar->enableStartBtn(isChecked);
        }
    } else {
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
    }

    // end
    if ((m_CurrentTab == downloadingTab) || (m_CurrentTab == finishTab)) {
        const QList<DownloadDataItem *> &renderList = m_DownLoadingTableView->getTableModel()->renderList();
        if (0 == renderList.size()) {
            emit isHeaderChecked(false);
        }
        for (int j = 0; j < renderList.size(); j++) {
            DownloadDataItem *data = renderList.at(j);
            data->Ischecked = isChecked;
            m_DownLoadingTableView->reset();
        }
    } else {
        const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
        if (0 == recycleList.size()) {
            emit isHeaderChecked(false);
        }
        for (int j = 0; j < recycleList.size(); j++) {
            DeleteDataItem *data = recycleList.at(j);
            if (!isChecked) {
                data->Ischecked = false;
            } else {
                data->Ischecked = true;
            }
        }
        m_RecycleTableView->reset();
    }
}

void MainFrame::onDownloadNewUrl(QString url, QString savePath, QString fileName, QString type)
{
    if (url.startsWith("magnet")) {
        QString left = url.split('&').at(0);
        QString infoHash = left.right(left.length() - left.lastIndexOf(':') - 1);
        if (!checkIsHasSameTask(infoHash.toLower())) {
            return;
        }
    }
    qDebug() << "getNewDownloadUrl: " << url << "    " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    bool isExitsUrl = false;

    // 判断url是否在数据中已存在
    DBInstance::isExistUrl(url, isExitsUrl);
    if (isExitsUrl) {
        if (showRedownloadMsgbox(url)) {
            deleteTaskByUrl(url);
        } else {
            return;
        }
    }

    // 将url加入数据库和aria
    TaskInfo task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    getUrlToName(task, url, savePath, fileName, type);
    DBInstance::addTask(task);
    qDebug() << task.gid << "   " << task.url;
    Aria2RPCInterface::instance()->addNewUri(task.url, savePath, task.downloadFilename, task.taskId);
    //clearTableItemCheckStatus();
    emit isHeaderChecked(false);

    m_NotaskWidget->hide();

    // 定时器打开
    //if(m_pUpdateTimer->isActive() == false) {
    m_UpdateTimer->start(m_timeInterval);
    // }
}

void MainFrame::getUrlToName(TaskInfo &task, QString url, QString savePath, QString name, QString type)
{
    // 获取url文件名
    QString fileName;

    if (!name.isEmpty()) {
        fileName = name;
    } else if (url.startsWith("magnet")) {
        fileName = url.split("&")[0];
        if (fileName.contains("btih:")) {
            fileName = fileName.split("btih:")[1] + ".torrent";
        } else {
            fileName = url.right(40);
        }
    } else {
        fileName = QString(url).right(url.length() - url.lastIndexOf('/') - 1);
    }

    // 对url进行转码
    if (!fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        const QByteArray byte = fileName.toLatin1();
        QString decode = QUrl::fromPercentEncoding(byte);
        if (decode.contains("?")) {
            decode = decode.split("?")[0];
        }
        fileName = decode;
    }
    if (fileName.contains(".torrent")) {
        if (!fileName.endsWith(".torrent")) {
            fileName = fileName.remove(".torrent");
        }
    }
    int count = DBInstance::getSameNameCount(fileName, type);
    if (count > 0) {
        fileName += QString("-%1").arg(count);
        int count1 = DBInstance::getSameNameCount(fileName, type);
        if (count1 > 0) {
            fileName += QString("-%1").arg(count1);
        }
    }
    if ((!type.isEmpty())) {
        fileName = fileName + "." + type;
    }
    task.taskId = QUuid::createUuid().toString();
    task.gid = "";
    task.gidIndex = 0;
    task.url = url;
    QString urlDecode = QUrl::fromPercentEncoding(fileName.toUtf8());
    task.downloadPath = savePath + "/" + urlDecode;
    task.downloadFilename = urlDecode;
    task.createTime = QDateTime::currentDateTime();
    return;
}

void MainFrame::continueDownload(DownloadDataItem *pItem)
{
    m_ToolBar->enablePauseBtn(true);
    m_ToolBar->enableStartBtn(false);
    if (pItem->status != Global::DownloadJobStatus::Active) {
        if (pItem->status == Global::DownloadJobStatus::Lastincomplete || pItem->status == Global::DownloadJobStatus::Error) {
            startDownloadTask(pItem);
        } else {
            Aria2RPCInterface::instance()->unpause(pItem->gid, pItem->taskId);
            if (m_UpdateTimer->isActive() == false) {
                m_UpdateTimer->start(m_timeInterval);
            }
        }
    }
}

void MainFrame::onContextMenu(const QPoint &pos)
{
    if (m_CurrentTab == recycleTab) {
        QModelIndex index = m_RecycleTableView->indexAt(pos);
        if (index.isValid()) {
            QModelIndex realIndex = index.sibling(index.row(), 0);
            QString gid = m_RecycleTableView->getTableModel()->data(index, TableModel::taskId).toString();
            m_DelCheckItem = m_RecycleTableView->getTableModel()->find(gid, 0);
            if (!m_RecycleTableView->getTableModel()->data(index, TableModel::Ischecked).toBool()) {
                onHeaderStatechanged(false);
            }
            m_RecycleTableView->getTableModel()->setData(realIndex, true, TableModel::Ischecked);
        }
    } else {
        QModelIndex index = m_DownLoadingTableView->indexAt(pos);
        if (index.isValid()) {
            QModelIndex realIndex = index.sibling(index.row(), 0);
            QString gid = m_DownLoadingTableView->getTableModel()->data(index, TableModel::taskId).toString();
            m_CheckItem = m_DownLoadingTableView->getTableModel()->find(gid);
            m_CheckIndex = index;
            if (!m_DownLoadingTableView->getTableModel()->data(index, TableModel::Ischecked).toBool()) {
                onHeaderStatechanged(false);
            }
            m_DownLoadingTableView->getTableModel()->setData(realIndex, true, TableModel::Ischecked);
        }
    }

    int chkedCnt = 0;
    int activeCount = 0;
    int pauseCount = 0;
    int errorCount = 0;
    int existFileCount = 0;
    DownloadDataItem *pDownloadItem = nullptr;
    DeleteDataItem *pDeleteItem = nullptr;
    if (m_CurrentTab == recycleTab) {
        const QList<DeleteDataItem *> &recyleList = m_RecycleTableView->getTableModel()->recyleList();
        for (int i = 0; i < recyleList.size(); i++) {
            if (recyleList.at(i)->Ischecked) {
                chkedCnt++;
                pDeleteItem = recyleList.at(i);
                if (QFileInfo(recyleList.at(i)->savePath).exists()) {
                    existFileCount++;
                }
            }
        }
    } else {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.at(i)->Ischecked) {
                chkedCnt++;
                pDownloadItem = selectList.at(i);
                if (pDownloadItem->status == Global::DownloadJobStatus::Active) {
                    ++activeCount;
                }
                if ((pDownloadItem->status == Global::DownloadJobStatus::Paused) || (pDownloadItem->status == Global::DownloadJobStatus::Lastincomplete)) {
                    ++pauseCount;
                }
                if (pDownloadItem->status == Global::DownloadJobStatus::Error) {
                    ++errorCount;
                }
            }
        }
    }
    if (chkedCnt == 0) {
        return;
    }

    QMenu *delmenlist = new QMenu(this);
    delmenlist->setObjectName("tableMenu");
    if (m_CurrentTab == downloadingTab) {
        if (pauseCount > 0 || errorCount > 0) {
            QAction *pActionStart = new QAction();
            pActionStart->setText(tr("Resume"));
            delmenlist->addAction(pActionStart);
            connect(pActionStart, &QAction::triggered, this, &MainFrame::onStartDownloadBtnClicked);
        }
        if (activeCount > 0) {
            QAction *pActionPause = new QAction();
            pActionPause->setText(tr("Pause"));
            delmenlist->addAction(pActionPause);
            connect(pActionPause, &QAction::triggered, this, &MainFrame::onPauseDownloadBtnClicked);
        }
        if ((errorCount > 0) && (1 == chkedCnt)) {
            QAction *pActionredownload = new QAction();
            pActionredownload->setText(tr("Download Again"));
            delmenlist->addAction(pActionredownload);
            connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        }
        delmenlist->addSeparator();
    }

    if (m_CurrentTab == recycleTab && existFileCount > 0) {
        QAction *returnedToOrigin = new QAction();
        returnedToOrigin->setText(tr("Restore"));
        delmenlist->addAction(returnedToOrigin);
        connect(returnedToOrigin, &QAction::triggered, this, &MainFrame::onReturnOriginActionTriggered);
    }
    if ((m_CurrentTab == recycleTab || m_CurrentTab == finishTab) && (1 == chkedCnt)) {
        QAction *pActionredownload = new QAction();
        pActionredownload->setText(tr("Download Again"));
        delmenlist->addAction(pActionredownload);
        connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        if ((m_CurrentTab == finishTab && QFileInfo(pDownloadItem->savePath).exists()) || (m_CurrentTab == recycleTab && QFileInfo(pDeleteItem->savePath).exists())) {
            QAction *pActionopenFile = new QAction();
            pActionopenFile->setText(tr("Open"));
            delmenlist->addAction(pActionopenFile);
            connect(pActionopenFile, &QAction::triggered, this, &MainFrame::onOpenFileActionTriggered);
        }
    }
    if ((1 == chkedCnt && (m_CurrentTab == finishTab) && QFileInfo(pDownloadItem->savePath).exists())
        || (1 == chkedCnt && m_CurrentTab == recycleTab && QFileInfo(pDeleteItem->savePath).exists())) {
        QAction *pActionopenFoler = new QAction();
        pActionopenFoler->setText(tr("Open folder"));
        delmenlist->addAction(pActionopenFoler);
        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if (m_CurrentTab == finishTab) {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        int noFileCount = 0;
        for (int i = 0; i < selectList.size(); ++i) {
            if (selectList.at(i)->Ischecked == 1) {
                DownloadDataItem *data = selectList.at(i);
                if (!QFileInfo(data->savePath).exists()) {
                    noFileCount++;
                }
            }
        }

        if (1 == chkedCnt && QFileInfo(pDownloadItem->savePath).exists()) {
            QAction *pactionRename = new QAction();
            pactionRename->setObjectName("rename");
            pactionRename->setText(tr("Rename"));
            delmenlist->addAction(pactionRename);
            delmenlist->addSeparator();
            connect(pactionRename, &QAction::triggered, this, &MainFrame::onRenameActionTriggered);
        }
        if (!noFileCount) {
            QAction *pActionMove = new QAction();
            pActionMove->setText(tr("Move to"));
            delmenlist->addAction(pActionMove);
            delmenlist->addSeparator();
            connect(pActionMove, &QAction::triggered, this, &MainFrame::onMoveToActionTriggered);
        }
    }

    if (1 == chkedCnt && m_CurrentTab == recycleTab) {
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == downloadingTab) {
            delmenlist->addSeparator();
        }
    }

    QAction *pactiondelDownloading = new QAction();
    pactiondelDownloading->setText(tr("Delete"));
    delmenlist->addAction(pactiondelDownloading);
    connect(pactiondelDownloading, &QAction::triggered, this, &MainFrame::onDeleteActionTriggered);

    QAction *pactionDeletePermanently = new QAction();
    pactionDeletePermanently->setText(tr("Permanently Delete"));
    delmenlist->addAction(pactionDeletePermanently);
    connect(pactionDeletePermanently, &QAction::triggered, this, &MainFrame::onDeletePermanentActionTriggered);

    if (1 == chkedCnt && m_CurrentTab == finishTab) {
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == downloadingTab) {
            delmenlist->addSeparator();
        }
    }

    if (1 == chkedCnt && m_CurrentTab == downloadingTab) {
        QAction *pactionCopyDownloadUrl = new QAction();
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == downloadingTab) {
            delmenlist->addSeparator();
        }
        QAction *pActionopenFoler = new QAction();
        pActionopenFoler->setText(tr("Open folder"));
        delmenlist->addAction(pActionopenFoler);
        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if (m_CurrentTab == recycleTab) {
        QAction *pActionClearRecycle = new QAction();
        pActionClearRecycle->setText(tr("Delete All"));
        delmenlist->addAction(pActionClearRecycle);
        connect(pActionClearRecycle, &QAction::triggered, this, &MainFrame::onClearRecyleActionTriggered);
    }

    delmenlist->exec(QCursor::pos());
    delete delmenlist;
}

void MainFrame::onCheckChanged(bool checked, int flag)
{
    Q_UNUSED(checked);
    Q_UNUSED(flag);
    // ToolBar禁用按钮联动：列表内复选框状态变化 begin
    int chkedCnt = 0;
    int fileExistCnt = 0;

    if (m_CurrentTab == recycleTab) {
        const QList<DeleteDataItem *> &recyleList = m_RecycleTableView->getTableModel()->recyleList();
        for (int i = 0; i < recyleList.size(); i++) {
            if (recyleList.at(i)->Ischecked) {
                m_DelCheckItem = recyleList.at(i);
                chkedCnt++;
                if (QFileInfo::exists(recyleList.at(i)->savePath)) {
                    fileExistCnt++;
                }
            }
        }
    } else {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.at(i)->Ischecked) {
                m_CheckItem = selectList.at(i);
                chkedCnt++;

                if (QFileInfo::exists(selectList.at(i)->savePath)) {
                    fileExistCnt++;
                }
            }
        }
    }

    if (chkedCnt > 0) {
        if (m_CurrentTab == downloadingTab) {
            m_ToolBar->enableStartBtn(true);
            m_ToolBar->enablePauseBtn(true);
            m_ToolBar->enableDeleteBtn(true);
            if (m_CheckItem->status == DownloadJobStatus::Paused || m_CheckItem->status == DownloadJobStatus::Lastincomplete) {
                m_ToolBar->enablePauseBtn(false);
            } else {
                m_ToolBar->enablePauseBtn(true);
            }
            if (m_CheckItem->status == DownloadJobStatus::Active) {
                m_ToolBar->enableStartBtn(false);
            } else {
                m_ToolBar->enableStartBtn(true);
            }

        } else if (m_CurrentTab == finishTab) {
            m_ToolBar->enableDeleteBtn(true);
            if (1 == chkedCnt && fileExistCnt > 0) {
                m_ToolBar->enableStartBtn(true);
                m_ToolBar->enablePauseBtn(true);
            } else {
                m_ToolBar->enableStartBtn(false);
                m_ToolBar->enablePauseBtn(false);
            }
        } else if (m_CurrentTab == recycleTab) {
            m_ToolBar->enableStartBtn(true);
            if (fileExistCnt > 0) {
                m_ToolBar->enablePauseBtn(true);
            } else {
                m_ToolBar->enablePauseBtn(false);
            }
            m_ToolBar->enableDeleteBtn(true);
        }
    } else {
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
        if (m_CurrentTab == recycleTab && m_RecycleTableView->getTableModel()->recyleList().count() > 0) {
            m_ToolBar->enableStartBtn(true);
        }
    }
}

void MainFrame::clearTableItemCheckStatus()
{
    if ((m_CurrentTab == downloadingTab) || (m_CurrentTab == finishTab)) {
        const QList<DownloadDataItem *> &renderList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int j = 0; j < renderList.size(); j++) {
            DownloadDataItem *data = renderList.at(j);
            data->Ischecked = false;
        }
        m_DownLoadingTableView->reset();
    } else {
        const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
        for (int j = 0; j < recycleList.size(); j++) {
            DeleteDataItem *data = recycleList.at(j);
            data->Ischecked = false;
        }
        m_RecycleTableView->reset();
    }
    emit isHeaderChecked(false);
}

void MainFrame::onSearchEditTextChanged(QString text)
{
    if (!text.isEmpty()) {
        m_NotaskLabel->hide();
        m_NotaskTipLabel->hide();
        m_NoResultlabel->show();
    } else {
        m_NotaskLabel->show();
        if (m_CurrentTab == downloadingTab) {
            m_NotaskTipLabel->show();
        }
        m_NoResultlabel->hide();
    }
    m_SearchContent = text;
    m_DownLoadingTableView->getTableControl()->searchEditTextChanged(text);
    m_RecycleTableView->getTableControl()->searchEditTextChanged(text);
    setTaskNum();
}

bool MainFrame::onDownloadNewTorrent(QString btPath, QMap<QString, QVariant> &opt, QString infoName, QString infoHash)
{
    QString selectedNum = opt.value("select-file").toString();

    if (selectedNum.isNull()) {
        qDebug() << "select is null";
        return false;
    }

    // 数据库是否已存在相同的地址
    if (!checkIsHasSameTask(infoHash.toLower())) {
        return false;
    }

    // 将任务添加如task表中
    TaskInfo task;
    QString strId = QUuid::createUuid().toString();
    task.taskId = strId;
    task.gid = "";
    task.gidIndex = 0;
    task.url = "";
    task.downloadPath = Settings::getInstance()->getCustomFilePath() + "/" + infoName;
    task.downloadFilename = infoName;
    task.createTime = QDateTime::currentDateTime();
    DBInstance::addTask(task);

    // 将任务添加如UrlInfo表中
    BtTaskInfo urlInfo;
    urlInfo.taskId = strId;
    urlInfo.url = "";
    urlInfo.downloadType = "torrent";
    urlInfo.seedFile = btPath;
    urlInfo.selectedNum = selectedNum;
    urlInfo.infoHash = infoHash;
    DBInstance::addBtTask(urlInfo);

    //opt.insert("out", infoName);
    // 开始下载
    Aria2RPCInterface::instance()->addTorrent(btPath, opt, strId);
    clearTableItemCheckStatus();

    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    foreach (DownloadDataItem *pItem, dataList) {
        QString str = "magnet:?xt=urn:btih:" + infoHash.toLower();
        QString url = pItem->url.toLower();
        if (url.startsWith(str)) {
            Aria2RPCInterface::instance()->forcePause(pItem->gid, pItem->taskId);
            Aria2RPCInterface::instance()->remove(pItem->gid, pItem->taskId);
            DBInstance::delTask(pItem->taskId);
            m_DownLoadingTableView->getTableModel()->removeItem(pItem);
            break;
        }
    }

    // 定时器打开
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
    return true;
}

void MainFrame::onRedownload(QString taskId, int rd)
{
    if ((rd == 0) || (rd == 1)) {
        m_DownLoadingTableView->getTableControl()->downloadListRedownload(taskId);
    } else {
        m_RecycleTableView->getTableControl()->recycleListRedownload(taskId);
    }
    clearTableItemCheckStatus();
    onUpdateMainUI();
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
}

void MainFrame::onRemoveFinished()
{
    m_ToolBar->enableCreateTaskBtn(true);
}

void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
    MessageBox msg;

    msg.setWarings(title, tr("sure"), "", sameUrlCount, sameUrlList);
    msg.exec();
}

void MainFrame::showClearMsgbox()
{
    MessageBox msg;

    connect(&msg, &MessageBox::Clearrecycle, this, &MainFrame::onClearRecycle);
    msg.setClear();
    int rs = msg.exec();
    if (rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
        emit isHeaderChecked(false);
    }
}

void MainFrame::onClearRecycle(bool ischecked)
{
    const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();

    if (ischecked) {
        for (int i = 0; i < recycleList.size(); ++i) {
            DeleteDataItem *data = recycleList.at(i);
            QString ariaTempFile = data->savePath + ".aria2";
            if (!data->savePath.isEmpty()) {
                QFile::remove(data->savePath);
                if (QFile::exists(ariaTempFile)) {
                    QFile::remove(ariaTempFile);
                }
            }
            Aria2RPCInterface::instance()->removeDownloadResult(data->gid);
        }
    }
    for (int i = 0; i < recycleList.size(); ++i) {
        DBInstance::delTask(recycleList.at(i)->taskId);
    }

    m_RecycleTableView->getTableModel()->removeRecycleItems();
    if (m_RecycleTableView->getTableModel()->recyleList().isEmpty()) {
        emit isHeaderChecked(false);
    }
}

void MainFrame::showReloadMsgbox()
{
    MessageBox msg;
    QString title = tr("Do you continue?");

    msg.setWarings(title, tr("sure"), tr("cancel"));
    int rs = msg.exec();
    if (rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);

        // 重新下载：通知aria2移除下载项

        if ((m_CurrentTab == downloadingTab) || (m_CurrentTab == finishTab)) {
            for (int i = 0; i < m_ReloadList.size(); i++) {
                DownloadDataItem *data = m_ReloadList.at(i);
                Aria2RPCInterface::instance()->forceRemove(data->gid, "REDOWNLOAD_" + QString::number(m_CurrentTab) + "_" + data->taskId);
            }
        } else {
            for (int i = 0; i < m_RecycleReloadList.size(); i++) {
                DeleteDataItem *data = m_RecycleReloadList.at(i);
                Aria2RPCInterface::instance()->forceRemove(data->gid, "REDOWNLOAD_" + QString::number(m_CurrentTab) + "_" + data->taskId);
            }
        }
    }
}

void MainFrame::showDeleteMsgbox(bool permanently)
{
    MessageBox msg;
    connect(&msg, &MessageBox::Deletedownload, this, &MainFrame::onDeleteConfirm);
    if (m_CurrentTab == downloadingTab) {
        msg.setDelete(permanently, true);
    } else {
        msg.setDelete(permanently);
    }
    int rs = msg.exec();
    if (rs == DDialog::Accepted) {
        // ToolBar禁用按钮联动：确认后禁用按钮
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
        m_DownLoadingTableView->getTableHeader()->onHeaderChecked(false);
        //        if(m_CurrentTab == recycleTab){
        //            m_ToolBar->enableStartBtn(true);
        //        }
    }
}

void MainFrame::showRenameMsgbox()
{
    MessageBox msg;

    connect(&msg, &MessageBox::Rename, this, &MainFrame::onRenameConfirmSlot);
    QString title = tr("Rename");
    msg.setReName(title, tr("sure"), tr("cancel"), m_CheckItem->fileName);
    msg.exec();
}

bool MainFrame::showRedownloadMsgbox(const QString sameUrl)
{
    MessageBox msg;

    connect(&msg, &MessageBox::reDownload, this, &MainFrame::onRedownloadConfirmSlot);
    msg.setRedownload(sameUrl);
    int rs = msg.exec();
    if (rs == DDialog::Accepted) {
        return true;
    }
    return false;
}

void MainFrame::showDiagnosticTool()
{
    DiagnosticTool control;
    connect(this, &MainFrame::ariaOption, &control, &DiagnosticTool::onAriaOption);
    control.exec();
}

void MainFrame::onAria2Remove(QString gId, QString id)
{
    Aria2RPCInterface::instance()->remove(gId, id);
}

void MainFrame::onDeleteConfirm(bool ischecked, bool permanent)
{
    if (m_UpdateTimer->isActive()) {
        m_UpdateTimer->stop();
    }

    if (ischecked || permanent) {
        m_ToolBar->enableCreateTaskBtn(false);
    }

    if (m_CurrentTab == recycleTab) {
        m_RecycleTableView->getTableControl()->onDeleteRecycleListConfirm(ischecked, permanent);
    } else {
        m_DownLoadingTableView->getTableControl()->onDeleteDownloadListConfirm(ischecked, permanent, m_RecycleTableView);
    }
    setTaskNum();

    if (!m_SearchContent.isEmpty()) {
        onSearchEditTextChanged(m_SearchContent);
    }
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
}

QString MainFrame::getDownloadSavepathFromConfig()
{
    return Settings::getInstance()->getDownloadSavePath();
}

void MainFrame::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        m_CtrlkeyPress = true;
        qDebug() << "Key_Control";
    }
    if (event->key() == Qt::Key_A) {
        if (m_CtrlkeyPress == true) {
            onHeaderStatechanged(true);
            emit isHeaderChecked(true);
            qDebug() << "Key_Control + Key_A";
        }
    }
    QWidget::keyPressEvent(event);
}

void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
    if (m_CtrlkeyPress == true) {
        m_CtrlkeyPress = false;
    }
    QWidget::keyReleaseEvent(event);
}

void MainFrame::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    //    m_DownLoadingTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //    m_DownLoadingTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    //    m_RecycleTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //    m_RecycleTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
}

void MainFrame::onNewBtnClicked()
{
    createNewTask("");
}

void MainFrame::onStartDownloadBtnClicked()
{
    if (isNetConnect()) {
        m_TaskWidget->showNetErrorMsg();
        return;
    }

    if (m_CurrentTab == downloadingTab) {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); ++i) {
            if (selectList.at(i)->Ischecked && !m_DownLoadingTableView->isRowHidden(i)) {
                continueDownload(selectList.at(i));
            }
        }

    } else if (m_CurrentTab == finishTab) {
        onOpenFolderActionTriggered();
    } else {
        onClearRecyleActionTriggered();
    }
}

void MainFrame::onPauseDownloadBtnClicked()
{
    if (isNetConnect()) {
        m_TaskWidget->showNetErrorMsg();
        return;
    }

    if (m_CurrentTab == downloadingTab) {
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableStartBtn(true);
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); ++i) {
            if (selectList.at(i)->Ischecked && !m_DownLoadingTableView->isRowHidden(i)) {
                if (selectList.at(i)->status != Global::DownloadJobStatus::Paused) {
                    BtTaskInfo info;
                    DBInstance::getBtTaskById(selectList.at(i)->taskId, info);
                    if (info.downloadType == "torrent" || selectList.at(i)->savePath.contains("[METADATA]")) {
                        Aria2RPCInterface::instance()->forcePause(selectList.at(i)->gid, selectList.at(i)->taskId);
                    } else {
                        Aria2RPCInterface::instance()->pause(selectList.at(i)->gid, selectList.at(i)->taskId);
                    }
                    QDateTime finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");

                    TaskStatus getStatus;
                    TaskStatus downloadStatus(selectList.at(i)->taskId,
                                              Global::DownloadJobStatus::Paused,
                                              QDateTime::currentDateTime(),
                                              selectList.at(
                                                            i)
                                                  ->completedLength,
                                              selectList.at(i)->speed,
                                              selectList.at(
                                                            i)
                                                  ->totalLength,
                                              selectList.at(i)->percent,
                                              selectList.at(i)->total,
                                              finishTime);

                    if (DBInstance::getTaskStatusById(selectList.at(i)->taskId, getStatus)) {
                        DBInstance::updateTaskStatusById(downloadStatus);
                    } else {
                        DBInstance::addTaskStatus(downloadStatus);
                    }
                }
            }
        }
    } else if (m_CurrentTab == finishTab) {
        onOpenFileActionTriggered();
    } else {
        onReturnOriginActionTriggered();
    }
}

void MainFrame::onDeleteDownloadBtnClicked()
{
    onDeleteActionTriggered();
}

void MainFrame::onRpcSuccess(QString method, QJsonObject json)
{
    if ((method == ARIA2C_METHOD_ADD_URI)
        || (method == ARIA2C_METHOD_ADD_TORRENT)
        || (method == ARIA2C_METHOD_ADD_METALINK)) {
        m_DownLoadingTableView->getTableControl()->aria2MethodAdd(json, m_SearchContent);
    } else if (method == ARIA2C_METHOD_TELL_STATUS) {
        m_DownLoadingTableView->getTableControl()->aria2MethodStatusChanged(json, m_CurrentTab, m_SearchContent);
    } else if (method == ARIA2C_METHOD_SHUTDOWN) {
        m_DownLoadingTableView->getTableControl()->aria2MethodShutdown(json);
    } else if (method == ARIA2C_METHOD_GET_FILES) {
        m_DownLoadingTableView->getTableControl()->aria2MethodGetFiles(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_UNPAUSE) {
        m_DownLoadingTableView->getTableControl()->aria2MethodUnpause(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_FORCE_REMOVE) {
        m_DownLoadingTableView->getTableControl()->aria2MethodForceRemove(json);
    } else if (method == ARIA2C_METHOD_REMOVE) {
        if (m_CurrentTab == CurrentTab::recycleTab) {
            QString id = json.value("id").toString();
            DeleteDataItem *pItem = m_RecycleTableView->getTableModel()->find(id, 0);
            if (pItem != nullptr) {
                QString ariaTempFile = pItem->savePath + ".aria2";
                if (QFile::exists(ariaTempFile)) {
                    QFile::remove(ariaTempFile);
                }
                m_RecycleTableView->getTableModel()->removeItem(pItem);
            }
        } else {
            QString id = json.value("id").toString();
            DownloadDataItem *pItem = m_DownLoadingTableView->getTableModel()->find(id);
            if (pItem != nullptr) {
                QString ariaTempFile = pItem->savePath + ".aria2";
                if (QFile::exists(ariaTempFile)) {
                    QFile::remove(ariaTempFile);
                }
                m_DownLoadingTableView->getTableModel()->removeItem(pItem);
            }
        }
    } else if (method == ARIA2C_METHOD_UNPAUSE_ALL) {
        m_DownLoadingTableView->getTableControl()->aria2MethodUnpauseAll(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_GET_GLOBAL_STAT) {
        m_DownLoadingTableView->getTableControl()->aria2GetGlobalStatus(json);
    } else if (method == ARIA2C_METHOD_GET_GLOBAL_OPTION) {
        QJsonObject obj = json.value("result").toObject();
        QString tracker = obj.value("bt-tracker").toString();
        bool isHasDHT = false;
        if (obj.value("enable-dht").toString().contains("true")) {
            QString dhtfile = obj.value("dht-file-path").toString();
            if (QFileInfo::exists(dhtfile)) {
                isHasDHT = true;
            }
        }
        emit ariaOption(!tracker.isEmpty(), isHasDHT);
    }
}

void MainFrame::onRpcError(QString method, QString id, int error, QJsonObject obj)
{
    QJsonObject result = obj.value("error").toObject();
    int errNo = result.value("code").toInt();
    QString message = result.value("message").toString();
    qDebug() << "slot rpc error method is:" << method << error << message;

    if (1 == errNo) {
        if (message.contains("cannot be paused now")) { //暂停失败，采用强制暂停
            DownloadDataItem *item = m_DownLoadingTableView->getTableModel()->find(id);
            if (nullptr != item) {
                Aria2RPCInterface::instance()->forcePause(item->gid, "");
            }
        } else if (message.contains("No URI to download.")) { //url错误，弹窗提示
            DBInstance::delTask(id);
            static bool isMsg = true;
            static MessageBox msg;
            if (msg.isVisible()) {
                return;
            }
            if (isMsg) {
                msg.setWarings(tr("Unable to parse the URL, please check"), tr("Ok"));
                isMsg = false;
            }
            msg.exec();
        }
    }
    // save_data_before_close();
    if (error == 400) {
        if ((method == ARIA2C_METHOD_FORCE_REMOVE) && id.startsWith("REDOWNLOAD_")) {
            // 当重新下载400移除失败时直接重新下载
            QStringList sp = id.split("_");
            QString taskId = sp.at(2);
            int rd = sp.at(1).toInt();
            emit redownload(taskId, rd);
        }
    }
}

void MainFrame::onTableItemSelected(const QModelIndex &selected)
{
    bool chked = selected.model()->data(selected, TableModel::DataRole::Ischecked).toBool();

    if (m_CtrlkeyPress == false && selected.column() != 0) {
        if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
            const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->renderList();
            for (int i = 0; i < dataList.size(); i++) {
                dataList.at(i)->Ischecked = false;
            }
            m_DownLoadingTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                             true,
                                                             TableModel::Ischecked);
            m_DownLoadingTableView->reset();
        } else {
            const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
            for (int i = 0; i < recycleList.size(); i++) {
                recycleList.at(i)->Ischecked = false;
            }
            m_RecycleTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                         true,
                                                         TableModel::Ischecked);
            m_RecycleTableView->reset();
        }
    } else if (m_CtrlkeyPress == true || selected.column() == 0) {
        m_DownLoadingTableView->reset();
        m_RecycleTableView->reset();
        if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
            m_DownLoadingTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                             !chked,
                                                             TableModel::Ischecked);
            m_DownLoadingTableView->reset();
        } else {
            m_RecycleTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                         !chked,
                                                         TableModel::Ischecked);
            m_RecycleTableView->reset();
        }
    }
}

void MainFrame::onUpdateMainUI()
{
    static int flag = 0;
    flag++;
    if (flag >= 5) {
        m_DownLoadingTableView->getTableControl()->updateDb();
        flag = 0;
    }
    Aria2RPCInterface::instance()->getGlobalSatat();
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();

    int activeCount = 0;
    m_ShutdownOk = true;
    for (const auto *item : dataList) {
        if ((item->status == Global::DownloadJobStatus::Active) || (item->status == Global::DownloadJobStatus::Waiting)) {
            Aria2RPCInterface::instance()->tellStatus(item->gid, item->taskId);
        }
        if ((item->status == Global::DownloadJobStatus::Active) || (item->status == Global::DownloadJobStatus::Waiting)
            || (item->status == Global::DownloadJobStatus::Lastincomplete)) {
            ++activeCount;
        }
        if (item->status == Global::DownloadJobStatus::Active) {
            m_ShutdownOk = false;
        }
    }

    if (activeCount == 0) {
        if (m_UpdateTimer->isActive()) {
            m_UpdateTimer->stop();
            m_NotaskLabel->show();
            if (m_CurrentTab == downloadingTab) {
                m_NotaskTipLabel->show();
            }

            m_DownLoadingTableView->getTableControl()->updateDb();
        }
    }
    setTaskNum();
}

void MainFrame::onDeleteActionTriggered()
{
    int selectedCount = 0;

    if (m_CurrentTab == recycleTab) {
        selectedCount = m_RecycleTableView->getTableControl()->onDelAction(m_CurrentTab);
    } else {
        selectedCount = m_DownLoadingTableView->getTableControl()->onDelAction(m_CurrentTab);
    }
    if (selectedCount == 0) {
        showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        showDeleteMsgbox(false);
    }
}

void MainFrame::onRedownloadActionTriggered()
{
    if (isNetConnect()) {
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    if (m_CurrentTab == CurrentTab::recycleTab) {
        if (QFileInfo::exists(m_DelCheckItem->savePath)) {
            MessageBox msg;
            msg.setRedownload(m_DelCheckItem->fileName, true);
            int rs = msg.exec();
            if (rs != DDialog::Accepted) {
                return;
            }
        }
    } else if (m_CurrentTab == CurrentTab::finishTab) {
        if (QFileInfo::exists(m_CheckItem->savePath)) {
            MessageBox msg;
            msg.setRedownload(m_CheckItem->fileName, true);
            int rs = msg.exec();
            if (rs != DDialog::Accepted) {
                return;
            }
        }
    }

    // ToolBar禁用按钮联动：确认后禁用按钮
    m_ToolBar->enableStartBtn(false);
    m_ToolBar->enablePauseBtn(false);
    m_ToolBar->enableDeleteBtn(false);

    // 重新下载：通知aria2移除下载项
    QString savePath;
    QString fileName;
    QString url;
    QString taskId;
    if ((m_CurrentTab == downloadingTab) || (m_CurrentTab == finishTab)) {
        savePath = m_CheckItem->savePath;
        fileName = m_CheckItem->fileName;
        url = m_CheckItem->url;
        taskId = m_CheckItem->taskId;
    } else {
        savePath = m_DelCheckItem->savePath;
        fileName = m_DelCheckItem->fileName;
        url = m_DelCheckItem->url;
        taskId = m_DelCheckItem->taskId;
    }

    if (url.isEmpty()) { //bt任务
        TaskInfo task;
        QMap<QString, QVariant> opt;

        QString filePath = QString(savePath).left(savePath.lastIndexOf('/'));
        BtTaskInfo info;
        DBInstance::getBtTaskById(taskId, info);

        QString strId = QUuid::createUuid().toString();
        task.taskId = strId;
        task.gid = "";
        task.gidIndex = 0;
        task.url = "";
        task.downloadPath = Settings::getInstance()->getCustomFilePath();
        task.downloadFilename = fileName;
        task.createTime = QDateTime::currentDateTime();
        DBInstance::addTask(task);

        BtTaskInfo urlInfo;
        urlInfo.taskId = strId;
        urlInfo.url = "";
        urlInfo.downloadType = "torrent";
        urlInfo.seedFile = info.seedFile;
        urlInfo.selectedNum = info.selectedNum;
        urlInfo.infoHash = info.infoHash;
        DBInstance::addBtTask(urlInfo);

        deleteTaskByTaskID(taskId);
        QString selectNum = info.selectedNum;
        opt.insert("select-file", selectNum);
        opt.insert("dir", task.downloadPath);
        Aria2RPCInterface::instance()->addTorrent(info.seedFile, opt, strId);
    } else { // 非bt任务
        TaskInfo task;
        QMap<QString, QVariant> opt;
        opt.insert("dir", savePath);
        QString filePath = QString(savePath).left(savePath.lastIndexOf('/'));
        deleteTaskByUrl(url);
        getUrlToName(task, url, filePath, fileName, "");

        DBInstance::addTask(task);
        qDebug() << task.gid << "   " << task.url;
        Aria2RPCInterface::instance()->addNewUri(task.url, filePath, task.downloadFilename, task.taskId);
    }
    emit isHeaderChecked(false);
    m_NotaskWidget->hide();
    m_UpdateTimer->start(m_timeInterval);
}

void MainFrame::onReturnOriginActionTriggered()
{
    int selectedCount = 0;
    const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
    int size = recycleList.size();
    for (int i = size - 1; i >= 0; i--) {
        DeleteDataItem *data = recycleList.at(i);
        TaskStatus getStatus;
        DBInstance::getTaskStatusById(data->taskId, getStatus);
        if ((data->Ischecked == 1) && !m_RecycleTableView->isRowHidden(i) && QFileInfo(data->savePath).exists()) {
            DownloadDataItem *returntoData = new DownloadDataItem;
            ++selectedCount;
            if (data->completedLength == data->totalLength) {
                if (data->totalLength != "0B") {
                    returntoData->status = Global::DownloadJobStatus::Complete;
                    getStatus.downloadStatus = Global::DownloadJobStatus::Complete;
                } else {
                    returntoData->status = Global::DownloadJobStatus::Lastincomplete;
                    getStatus.downloadStatus = Global::DownloadJobStatus::Lastincomplete;
                }
            } else {
                if ((data->completedLength != "0B") && (data->totalLength != "0B")) {
                    double d = returntoData->completedLength.toLong() * 100.0 / returntoData->totalLength.toLong();
                    returntoData->percent = static_cast<int>(d);

                    if ((returntoData->percent < 0) || (returntoData->percent > 100)) {
                        returntoData->status = Global::DownloadJobStatus::Lastincomplete;
                        getStatus.downloadStatus = Global::DownloadJobStatus::Lastincomplete;
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
            if (returntoData->status == Global::DownloadJobStatus::Removed || getStatus.downloadStatus == Global::DownloadJobStatus::Removed) {
                returntoData->status = Global::DownloadJobStatus::Lastincomplete;
                getStatus.downloadStatus = Global::DownloadJobStatus::Lastincomplete;
            }
            //returntoData->status = Global::Status::Lastincomplete;
            m_DownLoadingTableView->getTableModel()->append(returntoData);
            if ((data->completedLength != data->totalLength) || (data->totalLength == "0B")) {
                returntoData->url = data->url;
                QString savePath;
                int folderPathLength = data->savePath.size() - data->fileName.size() - 1;
                savePath = data->savePath.left(folderPathLength);
                //opt.insert("dir", savePath);
                //opt.insert("out", fileName);
                BtTaskInfo taskInfo;
                DBInstance::getBtTaskById(returntoData->taskId, taskInfo);
                if (!taskInfo.taskId.isEmpty()) {
                    if (taskInfo.downloadType == "torrent") {
                        //opt.insert("select-file", taskInfo.selectedNum);
                        if (m_UpdateTimer->isActive() == false) {
                            m_UpdateTimer->start(m_timeInterval);
                        }
                    }
                } else {
                    onDownloadLimitChanged();
                    //Aria2RPCInterface::instance()->addUri(returntoData->url, opt, returntoData->taskId);
                    //Aria2RPCInterface::instance()->pause(returntoData->gid, returntoData->taskId);
                    if (m_UpdateTimer->isActive() == false) {
                        m_UpdateTimer->start(m_timeInterval);
                    }
                }
            }
            m_RecycleTableView->getTableModel()->removeItem(data);
            setTaskNum();
        }
        DBInstance::updateTaskStatusById(getStatus);
    }
    if (selectedCount == 0) {
        //showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        // ToolBar禁用按钮联动：还原后禁用按钮
        if (m_RecycleTableView->getTableModel()->recyleList().size() > 0) {
            m_ToolBar->enableStartBtn(true);
        } else {
            m_ToolBar->enableStartBtn(false);
        }
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
    }
    if (m_RecycleTableView->getTableModel()->recyleList().isEmpty()) {
        emit isHeaderChecked(false);
    }
}

void MainFrame::onOpenFileActionTriggered()
{
    if (m_CurrentTab == finishTab) {
        QString path = QString("file:///") + m_CheckItem->savePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    } else if (m_CurrentTab == recycleTab) {
        QString path = QString("file:///") + m_DelCheckItem->savePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
}

void MainFrame::onOpenFolderActionTriggered()
{
    if (m_CurrentTab == recycleTab) {
        QString filePath = m_DelCheckItem->savePath.left(m_DelCheckItem->savePath.length()
                                                         - m_DelCheckItem->savePath.split('/').last().length());
        QString path = QString("file:///") + filePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    } else {
        QString filePath = m_CheckItem->savePath.left(m_CheckItem->savePath.length()
                                                      - m_CheckItem->savePath.split('/').last().length());
        QString path = QString("file:///") + filePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
}

void MainFrame::onRenameActionTriggered()
{
    m_DownLoadingTableView->setCurrentIndex(m_CheckIndex);
    m_DownLoadingTableView->edit(m_CheckIndex.sibling(m_CheckIndex.row(), 1));
}

void MainFrame::onMoveToActionTriggered()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec() != QDialog::Accepted) {
        return;
    }

    QStringList fileName = fileDialog.selectedFiles();
    QString filePath = fileName.first();
    if (!filePath.isEmpty()) {
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); ++i) {
            if ((selectList.at(i)->status == DownloadJobStatus::Complete) && (selectList.at(i)->Ischecked == true)) {
                DownloadDataItem *data = selectList.at(i);
                QFile::rename(data->savePath, filePath + "/" + data->fileName);
                data->savePath = filePath + "/" + data->fileName;
                TaskInfo task;
                DBInstance::getTaskByID(data->taskId, task);
                task.downloadPath = data->savePath;
                task.downloadFilename = data->fileName;
                DBInstance::updateTaskInfoByID(task);
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
    BtTaskInfo getUrlInfo;
    QString url;
    if (m_CurrentTab == downloadingTab || m_CurrentTab == finishTab) {
        DBInstance::getBtTaskById(m_CheckItem->taskId, getUrlInfo);
        if (!getUrlInfo.taskId.isEmpty()) {
            if (getUrlInfo.downloadType == "torrent") {
                url = "magnet:?xt=urn:btih:" + getUrlInfo.infoHash;
            }
        } else {
            url = m_CheckItem->url;
        }
    } else if (m_CurrentTab == recycleTab) {
        DBInstance::getBtTaskById(m_DelCheckItem->taskId, getUrlInfo);
        if (!getUrlInfo.taskId.isEmpty()) {
            if (getUrlInfo.downloadType == "torrent") {
                url = "magnet:?xt=urn:btih:" + getUrlInfo.infoHash;
            }
        } else {
            url = m_DelCheckItem->url;
        }
    }

    m_CopyUrlFromLocal = true;
    QClipboard *clipboard = DApplication::clipboard();
    clipboard->setText(url);
    //m_TaskNum->setText(tr("Copied to clipboard"));

    QString showHead(tr("Downloader"));
    QString showInfo(tr("Copied to clipboard"));
    btNotificaitonSettings(showHead, showInfo);
}

void MainFrame::onDeletePermanentActionTriggered()
{
    if (m_CurrentTab == recycleTab) {
        m_RecycleTableView->getTableControl()->onDeletePermanentAction(m_CurrentTab);
    } else {
        m_DownLoadingTableView->getTableControl()->onDeletePermanentAction(m_CurrentTab);
    }

    showDeleteMsgbox(true);
}

void MainFrame::onRenameConfirmSlot(QString &name)
{
    if (name == m_CheckItem->fileName) {
        return;
    }

    QString FilePath;
    FilePath = m_CheckItem->savePath.left(m_CheckItem->savePath.length()
                                          - m_CheckItem->fileName.length());

    if (QFileInfo::exists(FilePath + name)) {
        return;
    }

    QFile::rename(FilePath + m_CheckItem->fileName, FilePath + name);
    m_CheckItem->fileName = name;
    m_CheckItem->savePath = FilePath + name;
    m_DownLoadingTableView->update();
    m_CheckItem = nullptr;

    TaskInfo task;
    DBInstance::getTaskByID(m_CheckItem->taskId, task);
    task.downloadPath = m_CheckItem->savePath;
    task.downloadFilename = name;
    DBInstance::updateTaskInfoByID(task);
}

void MainFrame::onRedownloadConfirmSlot(const QString sameUrl, QString fileName, QString type)
{
    if (sameUrl.contains(".torrent")) {
        return;
    }
    // 将url加入数据库和aria
    TaskInfo task;
    TaskInfo tempTask;
    QMap<QString, QVariant> opt;
    QString savePath = Settings::getInstance()->getDownloadSavePath();
    opt.insert("dir", savePath);
    getUrlToName(task, sameUrl, savePath, fileName, type);
    DBInstance::getTaskForUrl(sameUrl, tempTask);
    QStringList tempPathList = tempTask.downloadPath.split("/");
    savePath.clear();
    for (int i = 0; i < tempPathList.size() - 1; i++) {
        savePath += tempPathList[i] + "/";
    }
    DBInstance::addTask(task);
    Aria2RPCInterface::instance()->addNewUri(task.url, savePath, task.downloadFilename, task.taskId);
    clearTableItemCheckStatus();

    m_NotaskWidget->hide();

    // 定时器打开
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
}

void MainFrame::onTableViewItemDoubleClicked(QModelIndex index)
{
    if (m_CurrentTab == finishTab) {
        QString taskId = m_DownLoadingTableView->getTableModel()->data(index, TableModel::taskId).toString();
        m_CheckItem = m_DownLoadingTableView->getTableModel()->find(taskId);
        onOpenFileActionTriggered();
    }
}

void MainFrame::onDownloadLimitChanged()
{
    QTime currentTime = QTime::currentTime();
    QTime periodStartTime;
    QTime periodEndTime;
    QString downloadSpeed, uploadSpeed;

    // get_limit_speed_time(period_start_time, period_end_time);
    DownloadSettings settings = Settings::getInstance()->getAllSpeedLimitInfo();
    if ("0" == settings.m_type) {
        Aria2RPCInterface::instance()->setDownloadUploadSpeed("0", "0");
        return;
    }

    periodStartTime.setHMS(settings.m_startTime.section(":", 0, 0).toInt(),
                           settings.m_startTime.section(":", 1, 1).toInt(),
                           settings.m_startTime.section(":", 2, 2).toInt());

    periodEndTime.setHMS(settings.m_endTime.section(":", 0, 0).toInt(),
                         settings.m_endTime.section(":", 1, 1).toInt(),
                         settings.m_endTime.section(":", 2, 2).toInt());

    downloadSpeed = settings.m_maxDownload;
    uploadSpeed = settings.m_maxUpload;

    // 判断当前时间是否在限速时间内
    bool bInPeriod = checkIfInPeriod(&currentTime, &periodStartTime, &periodEndTime);
    if (!bInPeriod) {
        Aria2RPCInterface::instance()->setDownloadUploadSpeed("0", "0");
    } else {
        Aria2RPCInterface::instance()->setDownloadUploadSpeed(downloadSpeed, uploadSpeed);
    }
}

void MainFrame::onPowerOnChanged(bool isPowerOn)
{
    // setAutoStart(isPowerOn);
    QString autostartDesktop = "downloader.desktop";
    QString defaultDesktop = "downloader.desktop";
    QString userDefaultDesktopPath = QString("%1/autostart/")
                                         .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    if (isPowerOn == 1) {
        QString cmd = QString("cp %1 %2").arg(UOS_DOWNLOAD_MANAGER_DESKTOP_PATH + defaultDesktop).arg(userDefaultDesktopPath);
        qDebug() << cmd;
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

void MainFrame::onMaxDownloadTaskNumberChanged(int nTaskNumber, bool isStopTask)
{
    static int maxDownloadTaskCount = 0;
    if (nTaskNumber != 1) {
        maxDownloadTaskCount = nTaskNumber;
    } else {
        if (maxDownloadTaskCount < 20) {
            maxDownloadTaskCount += 1;
        } else {
            return;
        }
    }
    QMap<QString, QVariant> opt;
    QString value = QString("max-concurrent-downloads=%1").arg(maxDownloadTaskCount);

    modifyConfigFile("max-concurrent-downloads=", value);
    opt.insert("max-concurrent-downloads", QString().number(maxDownloadTaskCount));
    Aria2RPCInterface::instance()->changeGlobalOption(opt);

    if (nTaskNumber == 1) {
        return;
    }
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    int activeCount = 0;
    m_ShutdownOk = true;
    if (isStopTask) {
        for (const auto *item : dataList) { //暂停掉之前已经开始的多余下载总数的任务
            if (item->status == Global::DownloadJobStatus::Active) {
                activeCount++;
                if (activeCount > maxDownloadTaskCount) {
                    Aria2RPCInterface::instance()->pause(item->gid, item->taskId);
                    QTimer::singleShot(500, this, [=]() {
                        Aria2RPCInterface::instance()->unpause(item->gid, item->taskId);
                    });
                    QDateTime finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
                    TaskStatus getStatus;
                    TaskStatus downloadStatus(item->taskId,
                                              Global::DownloadJobStatus::Paused,
                                              QDateTime::currentDateTime(),
                                              item->completedLength,
                                              item->speed,
                                              item->totalLength,
                                              item->percent,
                                              item->total,
                                              finishTime);

                    if (DBInstance::getTaskStatusById(item->taskId, getStatus)) {
                        DBInstance::updateTaskStatusById(downloadStatus);
                    } else {
                        DBInstance::addTaskStatus(downloadStatus);
                    }
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
    Aria2RPCInterface::instance()->changeGlobalOption(opt);
    QString value = "disk-cache=" + cacheNum;
    modifyConfigFile("disk-cache=", value);
}

void MainFrame::modifyConfigFile(QString configItem, QString value)
{
    QString strAll;
    QStringList strList;

    QString aria2configPath = QString("%1/%2/%3/aria2.conf")
                                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                  .arg(qApp->organizationName())
                                  .arg(qApp->applicationName());

    QFile readFile(aria2configPath);

    if (readFile.open((QIODevice::ReadOnly | QIODevice::Text))) {
        QTextStream stream(&readFile);
        strAll = stream.readAll();
    }
    readFile.close();
    QFile writeFile(aria2configPath);
    if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&writeFile);
        strList = strAll.split("\n");
        for (int i = 0; i < strList.count(); i++) {
            if (strList.at(i).contains(configItem)) {
                QString tempStr = strList.at(i);
                tempStr.replace(0, tempStr.length(), value);
                stream << tempStr << '\n';
            } else {
                if (i == strList.count() - 1) {
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
    if (periodResult == 0) {
        // 当前时间和限速时间相同，在限速时间内
        if (cmpBeginResult == 0) {
            return true;
        }
    }
    // 开始时间，小于结束时间
    else if (periodResult == 1) {
        // 当前时间大于开始时间，并且小于结束时间
        if ((cmpBeginResult == -1) && (cmpEndResult == 1)) {
            return true;
        }
    }
    // 开始时间，大于结束时间，即限时为开始时间到24点，0点到结束时间
    else {
        if ((cmpBeginResult == -1) || (cmpEndResult == 1)) {
            return true;
        }
    }

    return false;
}

int MainFrame::checkTime(QTime *startTime, QTime *endTime)
{
    if (startTime->hour() == endTime->hour()) {
        if (startTime->minute() == endTime->minute()) {
            return 0;
        } else if (startTime->minute() < endTime->minute()) {
            return 1;
        } else {
            return -1;
        }
    } else if (startTime->hour() < endTime->hour()) {
        return 1;
    } else {
        return -1;
    }
}

void MainFrame::initDataItem(Global::DownloadDataItem *data, const TaskInfo &tbTask)
{
    data->gid = tbTask.gid;
    data->url = tbTask.url;
    data->time = "0";
    data->speed = "0KB/s";
    data->taskId = tbTask.taskId;
    data->fileName = tbTask.downloadFilename;
    data->savePath = tbTask.downloadPath;
    data->createTime = tbTask.createTime.toString("yyyy-MM-dd hh:mm:ss");
    TaskStatus taskStatus;
    DBInstance::getTaskStatusById(data->taskId, taskStatus);
    if (!taskStatus.taskId.isEmpty()) {
        data->percent = taskStatus.percent;
        data->Ischecked = 0;
        data->totalLength = taskStatus.totalLength;
        data->completedLength = taskStatus.compeletedLength;
        if (data->url.toLower().contains("magnet:?xt=urn:btih")) {
            data->completedLength = "0KB";
        }
        if (taskStatus.downloadStatus == Global::DownloadJobStatus::Active
            || taskStatus.downloadStatus == Global::DownloadJobStatus::Paused
            || taskStatus.downloadStatus == Global::DownloadJobStatus::Waiting) {
            data->status = Global::DownloadJobStatus::Lastincomplete;
        } else {
            data->status = taskStatus.downloadStatus;
        }
        data->total = taskStatus.totalFromSource;
        if (data->status == Global::DownloadJobStatus::Complete) {
            data->time = taskStatus.modifyTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
}

void MainFrame::initDelDataItem(Global::DownloadDataItem *data, Global::DeleteDataItem *delData)
{
    TaskStatus taskStatus;
    DBInstance::getTaskStatusById(data->taskId, taskStatus);
    delData->taskId = data->taskId;
    delData->gid = data->gid;
    delData->url = data->url;
    delData->status = data->status;
    delData->fileName = data->fileName;
    delData->savePath = data->savePath;
    delData->deleteTime = taskStatus.modifyTime.toString("yyyy-MM-dd hh:mm:ss");
    delData->totalLength = data->totalLength;
    delData->completedLength = data->completedLength;
    delData->finishTime = taskStatus.finishTime.toString("yyyy-MM-dd hh:mm:ss");
}

void MainFrame::onIsStartAssociatedBTFile(bool status)
{
    if (status) {
        startBtAssociat();
    } else {
        endBtAssociat();
    }
}

void MainFrame::onAutoDownloadBySpeed(bool status)
{
    if (!status) {
        onMaxDownloadTaskNumberChanged(Settings::getInstance()->getMaxDownloadTaskNumber(), false);
    }
}

void MainFrame::startBtAssociat()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list";
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error";
        return;
    }
    QTextStream data(&readFile);
    bool isDefault = false;
    bool isAdded = false;
    QStringList DefaultList;
    QStringList AddedList;
    //找到 [Default Applications] 和[Added Associations] 下面中的 application/x-bittorrent=字段
    while (!data.atEnd()) {
        QString sLine = data.readLine();
        if (sLine == "[Default Applications]") {
            isDefault = true;
            isAdded = false;
        } else if (sLine == "[Added Associations]") {
            isDefault = false;
            isAdded = true;
        }
        if (isDefault) {
            DefaultList.append(sLine);
        }
        if (isAdded) {
            AddedList.append(sLine);
        }
    }
    //将application/x-bittorrent 字段替换为 application/x-bittorrent=uos-download.desktop。 uos-download.desktop为桌面文件夹名字
    if (!DefaultList.isEmpty()) {
        for (int i = 0; i < DefaultList.size(); i++) {
            if (DefaultList[i].contains("application/x-bittorrent")) {
                DefaultList[i] = "application/x-bittorrent=downloader.desktop;";
            }
            if (i == DefaultList.size() - 1 && !(DefaultList[i].contains("application/x-bittorrent"))) {
                DefaultList.append("application/x-bittorrent=downloader.desktop;");
            }
        }
    } else {
        qDebug() << "[Default Associations] is Null";
    }
    if (!AddedList.isEmpty()) {
        for (int i = 0; i < AddedList.size(); i++) {
            if (AddedList[i].contains("application/x-bittorrent")) {
                AddedList[i] = "application/x-bittorrent=downloader.desktop;";
            }
            if (i == AddedList.size() - 1 && !(AddedList[i].contains("application/x-bittorrent"))) {
                AddedList.append("application/x-bittorrent=downloader.desktop;");
            }
        }
    } else {
        qDebug() << "[Default Associations] is Null";
    }
    readFile.close();

    //将替换以后的字符串，重新写入到文件中去
    QFile writerFile(path);
    DefaultList << AddedList;
    if (writerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    }
    QTextStream writeData(&writerFile);

    for (int i = 0; i < DefaultList.size(); i++) {
        writeData << DefaultList[i] << endl;
    }
    writeData.flush();
    writerFile.close();
}

void MainFrame::endBtAssociat()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list";
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly)) {
        qDebug() << "open file error";
        return;
    }
    QTextStream data(&readFile);
    QStringList list;
    while (!data.atEnd()) {
        QString sLine = data.readLine();
        list.append(sLine);
    }
    for (int i = 0; i < list.size(); i++) {
        if (list[i].contains("application/x-bittorrent")) {
            list[i] = "application/x-bittorrent=";
        }
    }
    readFile.close();

    QFile writerFile(path);
    if (!writerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "writer file error";
        return;
    }
    QTextStream writeData(&writerFile);
    for (int i = 0; i < list.size(); i++) {
        writeData << list[i] << endl;
    }
    writeData.flush();
    writerFile.close();
}

void MainFrame::btNotificaitonSettings(QString head, QString text, bool isBt)
{
    // 获取免打扰模式值
    QVariant switchButton = Settings::getInstance()->m_settings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    bool downloadInfoNotify = Settings::getInstance()->getDownloadInfoSystemNotifyState();
    if (downloadInfoNotify) {
        QDBusInterface tInterNotify("com.deepin.dde.Notification",
                                    "/com/deepin/dde/Notification",
                                    "com.deepin.dde.Notification",
                                    QDBusConnection::sessionBus());
        QList<QVariant> arg;
        QString in0("downloader"); //下载器
        uint in1 = 101;
        QString in2;
        in2 = "downloader";
        QString in3(head);
        QString in4(text);

        QStringList in5;
        QVariantMap in6;
        if (isBt) {
            in5 << "_view" << tr("View");
            in6["x-deepin-action-_view"] = "downloader";
        }

        int in7 = 5000;
        arg << in0 << in1 << in2 << in3 << in4 << in5 << in6 << in7;
        tInterNotify.callWithArgumentList(QDBus::AutoDetect, "Notify", arg);
    }
}

void MainFrame::startDownloadTask(DownloadDataItem *pItem)
{
    QString savePath = pItem->savePath;

    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath.left(savePath.lastIndexOf("/")));
    opt.insert("out", pItem->fileName);
    BtTaskInfo getUrlInfo;
    DBInstance::getBtTaskById(pItem->taskId, getUrlInfo);
    if (!getUrlInfo.taskId.isEmpty()) {
        if (getUrlInfo.downloadType == "torrent") {
            QString selectNum = getUrlInfo.selectedNum;
            opt.insert("select-file", selectNum);
            if (!QFile(getUrlInfo.seedFile).exists()) {
                showWarningMsgbox(tr("seed file not exists or broken;"));
                qDebug() << "seed file not exists or broken;";
            } else {
                Aria2RPCInterface::instance()->addTorrent(getUrlInfo.seedFile,
                                                          opt,
                                                          getUrlInfo.taskId);
                if (m_UpdateTimer->isActive() == false) {
                    m_UpdateTimer->start(m_timeInterval);
                }
            }
        }
    } else {
        // deal_download_upload_limit_period();
        Aria2RPCInterface::instance()->addUri(pItem->url, opt, pItem->taskId);
        //clearTableItemCheckStatus();
        if (m_UpdateTimer->isActive() == false) {
            m_UpdateTimer->start(m_timeInterval);
        }
    }
}

void MainFrame::Raise()
{
    // 恢复窗口显示
    QDesktopWidget *desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
    show();
    setWindowState(Qt::WindowActive);
    activateWindow();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    //m_LeftList->setCurrentIndex(m_LeftList->currentIndex().sibling(0, 0));
}

void MainFrame::onParseUrlList(QVector<LinkInfo> &urlList, QString path)
{
    QString size;
    if (Settings::getInstance()->getPriorityDownloadBySize(size)) { //优先下载大小小于多少的任务

        for (QVector<LinkInfo>::iterator it = urlList.begin(); it != urlList.end();) {
            if (it->length < (size.toInt() * 1024 * 1024)) {
                LinkInfo link = *it;
                it = urlList.erase(it);
                urlList.prepend(link);
                it++;
            } else
                it++;
        }
    }

    foreach (LinkInfo info, urlList) {
        QString url = info.urlTrueLink.isEmpty() ? info.url : info.urlTrueLink;
        onDownloadNewUrl(url, path, info.urlName, info.type);
        QTime time;
        time.start();
        while (time.elapsed() < 500) {
            QCoreApplication::processEvents();
        }
    }
}

void MainFrame::onDownloadFinish()
{
    //m_UpdateTimer->stop();
    m_ShutdownOk = true;
    if (m_ShutdownAct->isChecked()) {
        m_ShutdownAct->setChecked(false);
        m_DownLoadingTableView->getTableControl()->saveDataBeforeClose();
        m_RecycleTableView->getTableControl()->saveDataBeforeClose();
        QTimer::singleShot(5000, this, [=]() {
            Aria2RPCInterface::instance()->shutdown();
            QProcess p;
            p.start("shutdown -h now");
            p.waitForFinished();
        });

    } else if (m_SleepAct->isChecked()) {
        m_SleepAct->setChecked(false);
        m_DownLoadingTableView->getTableControl()->saveDataBeforeClose();
        m_RecycleTableView->getTableControl()->saveDataBeforeClose();
        //Aria2RPCInterface::instance()->shutdown();
        QProcess p;
        p.start("systemctl suspend");
        p.waitForFinished();

    } else if (m_QuitProcessAct->isChecked()) {
        m_QuitProcessAct->setChecked(false);
        onTrayQuitClick(true);
    }
}

void MainFrame::clearSharedMemory()
{
    QSharedMemory sharedMemory("downloader");
    if (sharedMemory.attach()) {
        if (sharedMemory.isAttached()) {
            sharedMemory.lock();
            char *to = static_cast<char *>(sharedMemory.data());
            size_t num = strlen(to);
            memset(to, 0, num);
            sharedMemory.unlock();
        }
    }
}

bool MainFrame::isNetConnect()
{
    QProcess *process = new QProcess(this);
    QStringList list;
    list << "-i"
         << "www.baidu.com";
    process->start("curl", list);
    process->waitForFinished();
    int ret = process->exitCode();
    return ret ? true : false;
}

bool MainFrame::isAutoStart()
{
    QString path = QString("%1/autostart/downloader.desktop").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error";
        return false;
    }
    QTextStream data(&readFile);
    QString str;
    while (!data.atEnd()) {
        str = data.readLine();
        if (str.contains("Hidden=")) {
            QStringList list = str.split('=');
            readFile.close();
            if (list[1] == "false") {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

void MainFrame::setAutoStart(bool ret)
{
    QString path = QString("%1/autostart/downloader.desktop").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly)) {
        qDebug() << "error";
    }
    QTextStream data(&readFile);
    QStringList list;
    while (!data.atEnd()) {
        list.append(data.readLine());
    }
    readFile.close();

    for (int i = 0; i < list.size(); i++) {
        if (list[i].contains("Hidden=")) {
            if (ret) {
                list[i] = "Hidden=false";
            } else {
                list[i] = "Hidden=true";
            }
        }
    }

    QString cmd = QString("rm -f %1").arg(path);
    char *ch;
    QByteArray ba = cmd.toLatin1();
    ch = ba.data();
    system(ch);
    //将替换以后的字符串，重新写入到文件中去
    QFile writerFile(path);
    if (writerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "open error";
    }
    QTextStream writeData(&writerFile);
    for (int i = 0; i < list.size(); i++) {
        writeData << list[i] << endl;
    }
    writeData.flush();
    writerFile.close();
}

void MainFrame::initDbus()
{
    QDBusConnection::sessionBus().unregisterService("com.downloader.service");
    QDBusConnection::sessionBus().registerService("com.downloader.service");
    QDBusConnection::sessionBus().registerObject("/downloader/path", this, QDBusConnection ::ExportAllSlots | QDBusConnection ::ExportAllSignals);
}

bool MainFrame::deleteDirectory(const QString &path)
{
    QFileInfo info(path);
    if (info.isFile()) {
        QFile::remove(path);
        return true;
    }
    if (path.isEmpty()) {
        return false;
    }

    QDir dir(path);
    if (!dir.exists()) {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

void MainFrame::deleteTaskByUrl(QString url)
{
    bool isExist = false;
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    foreach (DownloadDataItem *pItem, dataList) {
        if (pItem->url == url) {
            isExist = true;
            deleteTask(pItem);
            break;
        }
    }
    if (!isExist) {
        const QList<DeleteDataItem *> &dataList = m_RecycleTableView->getTableModel()->recyleList();
        foreach (DeleteDataItem *pItem, dataList) {
            if (pItem->url == url) {
                isExist = true;
                deleteTask(pItem);
                break;
            }
        }
    }
    QThread::usleep(100);
}

void MainFrame::deleteTaskByTaskID(QString taskID)
{
    bool isExist = false;
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    foreach (DownloadDataItem *pItem, dataList) {
        if (pItem->taskId == taskID) {
            isExist = true;
            deleteTask(pItem);
            break;
        }
    }
    if (!isExist) {
        const QList<DeleteDataItem *> &dataList = m_RecycleTableView->getTableModel()->recyleList();
        foreach (DeleteDataItem *pItem, dataList) {
            if (pItem->taskId == taskID) {
                isExist = true;
                deleteTask(pItem);
            }
        }
    }
    QThread::usleep(200);
}

void MainFrame::deleteTask(DeleteDataItem *pItem)
{
    if (pItem == nullptr) {
        return;
    }
    Aria2RPCInterface::instance()->forcePause(pItem->gid, pItem->taskId);
    Aria2RPCInterface::instance()->forceRemove(pItem->gid, pItem->taskId);
    QString filePath = pItem->savePath;
    if (!pItem->savePath.isEmpty()) {
        if (pItem->url.isEmpty()) { //bt任务
            BtTaskInfo info;
            DBInstance::getBtTaskById(pItem->taskId, info);
            QString torrentPath = info.seedFile;
            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
            QString mode = btInfo.mode;
            if (pItem->savePath.contains(btInfo.name)) {
                deleteDirectory(pItem->savePath);
            }
        } else {
            deleteDirectory(pItem->savePath);
        }
        if (QFile::exists(filePath + ".aria2")) {
            QFile::remove(filePath + ".aria2");
            QTimer::singleShot(3000, [=]() {
                QString path = Settings::getInstance()->getDownloadSavePath();
                if (!filePath.contains(path)) {
                    QFile::remove(filePath);
                    QFile::remove(filePath + ".aria2");
                }
            });
        }
    }
    DBInstance::delTask(pItem->taskId);
    m_RecycleTableView->getTableModel()->removeItem(pItem);
}

void MainFrame::deleteTask(DownloadDataItem *pItem)
{
    Aria2RPCInterface::instance()->forcePause(pItem->gid, pItem->taskId);
    Aria2RPCInterface::instance()->remove(pItem->gid, pItem->taskId);
    QString filePath = pItem->savePath;
    if (!pItem->savePath.isEmpty()) {
        if (pItem->url.isEmpty()) { //bt任务
            BtTaskInfo info;
            DBInstance::getBtTaskById(pItem->taskId, info);
            QString torrentPath = info.seedFile;
            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
            QString mode = btInfo.mode;
            if (pItem->savePath.contains(btInfo.name)) {
                deleteDirectory(pItem->savePath);
            }
        } else {
            deleteDirectory(pItem->savePath);
        }

        if (QFile::exists(filePath + ".aria2")) {
            QFile::remove(filePath + ".aria2");
            QTimer::singleShot(3000, [=]() {
                QString path = Settings::getInstance()->getDownloadSavePath();
                if (!filePath.contains(path)) {
                    QFile::remove(filePath);
                    QFile::remove(filePath + ".aria2");
                }
            });
        }
    }
    DBInstance::delTask(pItem->taskId);
    if (pItem->status == DownloadJobStatus::Active || pItem->status == DownloadJobStatus::Waiting) { //正在下载的任务，等删除返回成功在删除任务记录
        return;
    }
    m_DownLoadingTableView->getTableModel()->removeItem(pItem);
    return;
}

bool MainFrame::checkIsHasSameTask(QString infoHash)
{
    QList<BtTaskInfo> urlList;
    DBInstance::getAllBtTask(urlList);
    for (int i = 0; i < urlList.size(); i++) {
        DownloadDataItem *pItem = m_DownLoadingTableView->getTableModel()->find(urlList[i].taskId);
        DeleteDataItem *pDeleteItem = nullptr;
        if (pItem == nullptr) {
            pDeleteItem = m_RecycleTableView->getTableModel()->find(urlList[i].taskId, 0);
        }
        if (urlList[i].infoHash.toLower() == infoHash) {
            MessageBox msg;
            //msg.setWarings(tr("Task exist, Downloading again will delete the downloaded content!"), tr("View"), tr("Redownload"), 0, QList<QString>());
            msg.setRedownload(urlList[i].seedFile);
            int ret = msg.exec();
            if (!ret) {
                return false;
            } else {
                if (pItem != nullptr) {
                    deleteTask(pItem);
                } else {
                    deleteTask(pDeleteItem);
                }
            }
            break;
        }
    }
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    const QList<DeleteDataItem *> &recyleList = m_DownLoadingTableView->getTableModel()->recyleList();
    for (DownloadDataItem *item : dataList) {
        QString url = item->url.toUpper();
        if (url.contains(infoHash.toUpper()) && item->status != DownloadJobStatus::Complete) {
            if (showRedownloadMsgbox(item->url)) {
                deleteTaskByUrl(item->url);
                break;
            } else {
                return false;
            }
        }
    }

    for (DeleteDataItem *item : recyleList) {
        QString url = item->url.toUpper();
        if (url.contains(infoHash.toUpper()) && item->status != DownloadJobStatus::Complete) {
            if (showRedownloadMsgbox(item->url)) {
                deleteTaskByUrl(item->url);
                break;
            } else {
                return false;
            }
        }
    }
    QThread::usleep(200);
    return true;
}
