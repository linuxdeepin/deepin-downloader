// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mainframe.h"
#include <DMenu>
#include <DLabel>
#include <DFontSizeManager>
#include <DTitlebar>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
#include <DFontSizeManager>
#include <DWidgetUtil>

#include <QDir>
#include <QLayout>
#include <QAction>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#if QT_VERSION_MAJOR <= 5
#include <QDesktopWidget>
#endif
#include <QClipboard>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QDesktopServices>
#include <QDebug>
#include <QDBusInterface>
#include <QMimeDatabase>
#include <QUuid>
#include <QTableWidgetItem>

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
#include "func.h"
#include "headerView.h"
#include "httpadvancedsettingwidget.h"

#include "searchresoultwidget.h"

#define UOS_DOWNLOAD_MANAGER_DESKTOP_PATH "/usr/share/applications/"

MainFrame::MainFrame(QWidget *parent)
    : DMainWindow(parent)
    , m_CurrentTab(Global::downloadingTab)
    , m_CheckItem(nullptr)
    , m_CheckIndex(QModelIndex())
{
    qDebug() << "MainFrame constructor started";
    init();
    initTab();
    initSetting();
    m_TaskWidget = new CreateTaskWidget(this);
    initTray();
    initDbus();
    initAria2();
    updateDHTFile();
    initConnection();
    initTabledata();
    setPaletteType();
    QString clipboradStr = QApplication::clipboard()->text();
    if (!clipboradStr.isEmpty()) {
        qDebug() << "[MainFrame] Clipboard is not empty, checking for URL";
        if (Settings::getInstance()->getIsClipboradStart(clipboradStr)) {
            qDebug() << "Clipboard contains URL, starting download check";
            m_Clipboard->checkClipboardHasUrl();
        }
    }
    qDebug() << "MainFrame initialization completed";
}

MainFrame *MainFrame::instance()
{
    // qDebug() << "[MainFrame] instance function started";
    static MainFrame *m = nullptr;
    if (m == nullptr) {
        // qDebug() << "[MainFrame] Creating new MainFrame instance";
        m = new MainFrame;
    }
    // qDebug() << "[MainFrame] instance function ended";
    return m;
}

MainFrame::~MainFrame()
{
    // qDebug() << "MainFrame destructor started";
    delete (m_Clipboard);
    DataBase::Instance().destory();
}

void MainFrame::init()
{
    qDebug() << "[MainFrame] init function started";
    setObjectName("ariaMain");
    setMinimumSize(838, 636);
    setTitlebarShadowEnabled(true);
    setAcceptDrops(true);
    // 添加设置界面
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
    m_QuitProcessAct = new QAction(tr("Exit"), this);
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
    p.setColor(QPalette::Window, QColor(255, 255, 255));

    QFrame *pMainWidget = new QFrame;
    pMainWidget->setFrameShape(QFrame::NoFrame);
    QHBoxLayout *pMainHLayout = new QHBoxLayout(pMainWidget);
    setCentralWidget(pMainWidget);
    pMainHLayout->setContentsMargins(0, 0, 0, 0);
    pMainHLayout->setSpacing(0);

    m_DownLoadingTableView = new TableView(tableviewFlag::downloading);
    m_DownLoadingTableView->setObjectName("downloadTableView");
    m_DownLoadingTableView->setColumnHidden(4, true);

    m_RecycleTableView = new TableView(tableviewFlag::recycle);
    m_RecycleTableView->setObjectName("recycleTableView");
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

    QVBoxLayout *pNotaskWidgetlayout = new QVBoxLayout(m_NotaskWidget);
    pNotaskWidgetlayout->setContentsMargins(10, 0, 0, 0);

    QFont lableFont;
    lableFont.setWeight(QFont::DemiBold);

    m_NotaskLabel = new Dtk::Widget::DLabel();
    m_NotaskLabel->setFont(lableFont);
    m_NotaskLabel->setWindowOpacity(0.9);
    m_NotaskLabel->setText(tr("No download tasks"));
    m_NotaskLabel->setAlignment(Qt::AlignHCenter);
    m_NotaskLabel->setForegroundRole(DPalette::PlaceholderText);
    pNotaskWidgetlayout->addWidget(m_NotaskLabel);

    //DFontSizeManager::instance()->bind(m_NotaskLabel, DFontSizeManager::T8, QFont::DemiBold);

    m_NotaskTipLabel = new DLabel();
    QFont notaskTipLabelFont;
    m_NotaskTipLabel->setFont(notaskTipLabelFont);
    m_NotaskTipLabel->setText(tr("Click + to create new task"));
    m_NotaskTipLabel->setAlignment(Qt::AlignHCenter);
    m_NotaskTipLabel->setForegroundRole(DPalette::PlaceholderText);
    pNotaskWidgetlayout->addWidget(m_NotaskTipLabel);
    pNotaskWidgetlayout->addStretch(5);
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_RightWidget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);

    m_RightStackwidget = new QStackedWidget(this);
    m_RightStackwidget->setCurrentIndex(0);

    m_TaskNumWidget = new QWidget;
    m_TaskNumWidget->setFixedHeight(30);
    //m_pTaskNumWidget->setPalette(pa);
    QHBoxLayout *TaskNumWidgetlayout = new QHBoxLayout(m_TaskNumWidget);
#if QT_VERSION_MAJOR > 5
    TaskNumWidgetlayout->setContentsMargins(0, 0, 0, 0);
#else
    TaskNumWidgetlayout->setMargin(0);
#endif
    m_TaskNum = new QLabel(tr("0 tasks"));
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

    m_LeftList = new LeftListView;
    m_LeftList->setObjectName("leftList");
    m_LeftList->setItemSpacing(0);
    m_LeftList->setItemSize(QSize(112, 40));
    m_LeftList->setItemMargins(QMargins(10, 2, 5, 2));
    m_LeftList->setIconSize(QSize(14, 14));
    m_LeftList->setTabKeyNavigation(true);
    //m_LeftList->setBackgroundRole(QPalette::NoRole);
    //m_LeftList->setFrameShape(QFrame::Shape::NoFrame);
    QFont font;
    font.setFamily("Source Han Sans");
    QSize itemsize = m_LeftList->itemSize();
#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::NormalMode) {
        font.setPixelSize(15);
        m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
        m_RecycleTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
        itemsize.setHeight(40);
        m_LeftList->setItemSize(itemsize);
    } else {
        font.setPixelSize(14);
        m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_CompactMode_Width);
        m_RecycleTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_CompactMode_Width);
        itemsize.setHeight(40 - 4);
        m_LeftList->setItemSize(itemsize);
    }
#else
    font.setPixelSize(15);
    m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
    m_RecycleTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
    itemsize.setHeight(40);
    m_LeftList->setItemSize(itemsize);
#endif
    m_LeftList->setFont(font);
    QPalette pal = m_LeftList->palette();
    pal.setColor(QPalette::Window, Qt::white);
    m_LeftList->setPalette(pal);
    QStandardItemModel *pleftlistModel = new QStandardItemModel(this);

    m_DownloadingItem = new DStandardItem(QIcon::fromTheme("dcc_list_downloading"), tr("Downloading"));
    m_DownloadingItem->setAccessibleText("Downloading");
    m_DownloadingItem->setFont(font);
    m_DownloadFinishItem = new DStandardItem(QIcon::fromTheme("dcc_print_done"), tr("Completed"));
    m_DownloadFinishItem->setAccessibleText("Completed");
    m_DownloadFinishItem->setFont(font);
    m_RecycleItem = new DStandardItem(QIcon::fromTheme("dcc_list_delete"), tr("Trash"));
    m_RecycleItem->setAccessibleText("Trash");
    m_RecycleItem->setFont(font);
#ifdef DTKWIDGET_CLASS_DSizeMode
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        QSize itemsize = m_LeftList->itemSize();
        QFont font;
        font.setFamily("Source Han Sans");
        if (sizeMode == DGuiApplicationHelper::NormalMode) {
                font.setPixelSize(15);
                m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
                m_RecycleTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_NormalMode_Width);
                m_LeftList->setItemSize(QSize(Global::tableView_NormalMode_Width, Global::tableView_NormalMode_Width));
                itemsize.setHeight(40);
                m_LeftList->setItemSize(itemsize);
            } else {
                font.setPixelSize(14);
                m_DownLoadingTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_CompactMode_Width);
                m_RecycleTableView->verticalHeader()->setDefaultSectionSize(Global::tableView_CompactMode_Width);
                m_LeftList->setItemSize(QSize(Global::tableView_CompactMode_Width, Global::tableView_CompactMode_Width));
                itemsize.setHeight(40 - 4);
                m_LeftList->setItemSize(itemsize);
            }
        m_DownloadingItem->setFont(font);
        m_DownloadFinishItem->setFont(font);
        m_RecycleItem->setFont(font);
    });
#endif
    m_DownloadingItem->setEditable(false);
    m_DownloadFinishItem->setEditable(false);
    m_RecycleItem->setEditable(false);
    pleftlistModel->appendRow(m_DownloadingItem);
    pleftlistModel->appendRow(m_DownloadFinishItem);
    pleftlistModel->appendRow(m_RecycleItem);
    m_LeftList->setModel(pleftlistModel);
    pLeftLayout->addWidget(m_LeftList, 0);
    m_LeftList->setCurrentIndex(pleftlistModel->index(0, 0));
    //m_LeftList->viewport()->setAutoFillBackground(true);
    m_Clipboard = new ClipboardTimer; // 获取当前剪切板
    m_UpdateTimer = new QTimer(this);
    m_TrayClickTimer = new QTimer(this);
    m_CurOpenBtDialogPath = "";
    m_LeftList->installEventFilter(this);
    m_DownLoadingTableView->installEventFilter(this);
    m_RecycleTableView->installEventFilter(this);
    qDebug() << "[MainFrame] init function ended";
}

void MainFrame::initTab()
{
    // qDebug() << "[MainFrame] initTab function started";
    setTabOrder(m_ToolBar, m_LeftWidget);
    // qDebug() << "[MainFrame] initTab function ended";
}

void MainFrame::initSetting()
{
    // qDebug() << "[MainFrame] initSetting function started";
    Settings::getInstance()->init();
    // qDebug() << "[MainFrame] initSetting function ended";
}

void MainFrame::initTray()
{
    qDebug() << "Initializing system tray icon";
    QIcon tryIcon = QIcon(":/icons/icon/downloader5.svg");

    m_SystemTray = new QSystemTrayIcon(this);
    m_SystemTray->setObjectName("systemTray");
    m_SystemTray->setIcon(tryIcon);
    m_SystemTray->setToolTip(tr("Downloader"));

    QAction *pShowMainAct = new QAction(tr("Show main window"), this);
    pShowMainAct->setObjectName("Showmainwindow");
    QAction *pNewDownloadAct = new QAction(tr("New task"), this);
    pNewDownloadAct->setObjectName("Newtask");
    QAction *pStartAllAct = new QAction(tr("Resume all"), this);
    pStartAllAct->setObjectName("Resumeall");
    QAction *pPauseAllAct = new QAction(tr("Pause all"), this);
    pPauseAllAct->setObjectName("Pauseall");
    QMenu *pFinishMenu = new QMenu(tr("When completed"), this);
    pFinishMenu->setObjectName("Whencompleted");
    pFinishMenu->addAction(m_ShutdownAct);
    pFinishMenu->addAction(m_SleepAct);
    pFinishMenu->addAction(m_QuitProcessAct);
    m_ShutdownAct->setObjectName("ShutdownAction");
    m_SleepAct->setObjectName("SleepAction");
    m_QuitProcessAct->setObjectName("QuitProcessAction");
    QAction *pQuitAct = new QAction(tr("Exit"), this);
    pQuitAct->setObjectName("QuitAction");

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
        for (auto pData : selectList) {
            if (pData->status != Global::DownloadTaskStatus::Complete)
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
    qDebug() << "[MainFrame] initTray function ended";
}

void MainFrame::updateDHTFile()
{
    qDebug() << "[MainFrame] updateDHTFile function started";
    QFileInfo f(QDir::homePath() + "/.config/uos/downloader/dht.dat");
    QDateTime t = f.fileTime(QFileDevice::FileModificationTime);

    QFileInfo f2(QDir::homePath() + "/.config/uos/downloader/dht6.dat");
    QDateTime t2 = f2.fileTime(QFileDevice::FileModificationTime);
    if (t.date() == QDate::currentDate() && t2.date() == QDate::currentDate()) {
        qDebug() << "[MainFrame] DHT files are up to date, returning";
        return;
    }
    QFile::remove(QDir::homePath() + "/.config/uos/downloader/dht.dat");
    QFile::remove(QDir::homePath() + "/.config/uos/downloader/dht6.dat");
}

void MainFrame::initConnection()
{
    qDebug() << "Setting up signal connections";
    connect(m_DownLoadingTableView, &TableView::HeaderStatechanged, this, &MainFrame::onHeaderStatechanged);
    connect(m_DownLoadingTableView, &TableView::customContextMenuRequested, this, &MainFrame::onContextMenu, Qt::QueuedConnection);
    connect(m_DownLoadingTableView, &TableView::pressed, this, &MainFrame::onTableItemSelected);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::RedownloadJob, this, &MainFrame::onRedownload);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::AutoDownloadBt, this, &MainFrame::OpenFile);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::unPauseTask, this, &MainFrame::continueDownload);
    connect(m_DownLoadingTableView->getTableControl(), &TableDataControl::AutoDownloadMetalink, this, &MainFrame::OpenFile);
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
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::AutoDownloadBt, this, &MainFrame::OpenFile);
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::AutoDownloadMetalink, this, &MainFrame::OpenFile);
    connect(m_RecycleTableView->getTableControl(), &TableDataControl::removeFinished, this, &MainFrame::onRemoveFinished);
    connect(m_RecycleTableView->getTableModel(), &TableModel::CheckChange, this, &MainFrame::onCheckChanged);
    connect(m_RecycleTableView, &TableView::doubleClicked, this, &MainFrame::onTableViewItemDoubleClicked);

    connect(this, &MainFrame::isHeaderChecked, m_DownLoadingTableView, &TableView::isCheckHeader);
    connect(this, &MainFrame::saveNameBeforChangeList, m_DownLoadingTableView, &TableView::onListchanged);
    connect(this, &MainFrame::isHeaderChecked, m_RecycleTableView, &TableView::isCheckHeader);

    connect(m_SettingAction, &QAction::triggered, this, &MainFrame::onSettingsMenuClicked);
    connect(m_Clipboard, &ClipboardTimer::sendClipboardTextChange, this, &MainFrame::onClipboardDataChanged);
    //connect(m_LeftList, &LeftListView::pressed, this, &MainFrame::onListClicked);
    connect(m_LeftList, &LeftListView::currentIndexChanged, this, &MainFrame::onListClicked);
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
    connect(Settings::getInstance(), &Settings::maxDownloadTaskNumberChanged, this, &MainFrame::onMaxDownloadTaskNumberChanged);
    connect(Settings::getInstance(), &Settings::disckCacheChanged, this, &MainFrame::onDisckCacheChanged);
    connect(Settings::getInstance(), &Settings::startAssociatedBTFileChanged, this, &MainFrame::onIsStartAssociatedBTFile);
    connect(Settings::getInstance(), &Settings::controlBrowserChanged, this, &MainFrame::onIsControlBrowser);
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
    connect(m_TaskWidget, &CreateTaskWidget::downLoadMetaLinkCreate, this, &MainFrame::onDownloadNewMetalink, Qt::UniqueConnection);
    qDebug() << "[MainFrame] initConnection function ended";
}

void MainFrame::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "[MainFrame] onActivated function started";
    if (QSystemTrayIcon::Trigger == reason) {
        qDebug() << "reason is Trigger";
        if (isVisible()) {
            qDebug() << "isVisible is true";
            if (isMinimized() || !isActiveWindow()) {
                if (isFullScreen()) {
                    qDebug() << "isFullScreen is true";
                    hide();
                    showFullScreen();
                } else {
                    qDebug() << "isFullScreen is false";
                    this->titlebar()->setFocus();

                    auto e = QProcessEnvironment::systemEnvironment();
                    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
                    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

                    if (XDG_SESSION_TYPE != QLatin1String("wayland") && !WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
                        qDebug() << "XDG_SESSION_TYPE is not wayland and WAYLAND_DISPLAY is not wayland";
                        showNormal();
                        activateWindow();
                    } else {
                        qDebug() << "XDG_SESSION_TYPE is wayland or WAYLAND_DISPLAY is wayland";
                        showNormal();
                        activateWindow();
                    }
                }
            } else {
                qDebug() << "isMinimized is true or isActiveWindow is false";
                showMinimized();
                hide();
            }
        } else {
            qDebug() << "isVisible is false";
            this->titlebar()->setFocus();
            showNormal();
            activateWindow();
        }
    }
    qDebug() << "[MainFrame] onActivated function ended";
}

void MainFrame::closeEvent(QCloseEvent *event)
{
    // qDebug() << "[MainFrame] closeEvent function started";
    m_SystemTray->show();
    if (Settings::getInstance()->getIsShowTip() || (Settings::getInstance()->getCloseMainWindowSelected() == 2)) {
        // qDebug() << "[MainFrame] Showing exit confirmation dialog";
        MessageBox msg(this);
        msg.setAccessibleName("closeMainwindow");
        connect(&msg, &MessageBox::closeConfirm, this, &MainFrame::onMessageBoxConfirmClick);
        msg.setExit();
        msg.exec();
    } else {
        // qDebug() << "[MainFrame] No confirmation needed, proceeding with close action";
        onMessageBoxConfirmClick(Settings::getInstance()->getCloseMainWindowSelected());
    }
    // setWindowFlags(Qt::Tool);
    event->ignore();
    // DMainWindow::closeEvent(event);
    // qDebug() << "[MainFrame] closeEvent function ended";
}

void MainFrame::paintEvent(QPaintEvent *event)
{
    // qDebug() << "[MainFrame] paintEvent function started";
    Q_UNUSED(event);
    setPaletteType();
    // qDebug() << "[MainFrame] paintEvent function ended";
}

void MainFrame::createNewTask(QString url)
{
    qDebug() << "[MainFrame] createNewTask function started with URL:" << url;
    if (Settings::getInstance()->getNewTaskShowMainWindowState()) {
        qDebug() << "[MainFrame] Showing main window for new task";
        activateWindow();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

        if (isHidden()) {
            Dtk::Widget::moveToCenter(this);
            show();
        }
    }
    m_TaskWidget->setUrl(url);
    if (!Func::isNetConnect()) {
        qDebug() << "[MainFrame] No network connection, showing network error message";
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    if (m_TaskWidget->isHidden()) {
        qDebug() << "[MainFrame] Task widget is hidden, moving to center";
        m_TaskWidget->move(pos().x() + this->width() / 2 - m_TaskWidget->width() / 2,
                           pos().y() + this->height() / 2 - 575 / 2);
    }
    m_TaskWidget->exec();
    qDebug() << "[MainFrame] createNewTask function ended";
}

void MainFrame::onReceiveExtentionUrl(QString url)
{
    qDebug() << "[MainFrame] onReceiveExtentionUrl function started with URL:" << url;
    QTimer::singleShot(50, this, [=]() {
        createNewTask(url);
    });
    qDebug() << "[MainFrame] onReceiveExtentionUrl function ended";
}

void MainFrame::onTrayQuitClick(bool force)
{
    qDebug() << "[MainFrame] onTrayQuitClick function started with force:" << force;
    if (!m_ShutdownOk && !force) {
        qDebug() << "[MainFrame] Showing quit confirmation dialog";
        static bool msgBoxFlg = true;
        static MessageBox msgBox(this);
        if (msgBox.isVisible()) {
            qDebug() << "[MainFrame] Message box already visible, returning";
            return;
        }
        if (msgBoxFlg) {
            qDebug() << "[MainFrame] Setting up quit confirmation dialog";
            QString title = tr("Are you sure you want to exit? \nDownloading tasks will be interrupted.");
            msgBox.setWarings(title, tr("Exit"), tr("Cancel"));
            msgBoxFlg = false;
        }
        int rs = msgBox.exec();
        if (rs != DDialog::Accepted) {
            qDebug() << "[MainFrame] User chose to cancel, returning";
            return;
        }
        msgBox.setParent(NULL);
    }
    m_DownLoadingTableView->getTableControl()->saveDataBeforeClose();
    m_RecycleTableView->getTableControl()->saveDataBeforeClose();
    Aria2RPCInterface::instance()->shutdown();
    // qApp->quit();
    QTimer::singleShot(3000, this, [&]() {
        qApp->exit();
    });
    qDebug() << "[MainFrame] onTrayQuitClick function ended";
}

void MainFrame::onMessageBoxConfirmClick(int index)
{
    qDebug() << "[MainFrame] onMessageBoxConfirmClick function started with index:" << index;
    if (index) {
        qDebug() << "[MainFrame] User chose to quit, calling onTrayQuitClick";
        onTrayQuitClick(false);
    } else {
        qDebug() << "[MainFrame] User chose to hide window";
        //showMinimized();
        hide();
    }
    qDebug() << "[MainFrame] onMessageBoxConfirmClick function ended";
}

void MainFrame::initAria2()
{
    qDebug() << "[MainFrame] initAria2 function started";
    Aria2RPCInterface::instance()->init(); // 启动Aria2RPCInterface::instance()
    connect(Aria2RPCInterface::instance(), SIGNAL(RPCSuccess(QString, QJsonObject)), this,
            SLOT(onRpcSuccess(QString, QJsonObject)));
    connect(Aria2RPCInterface::instance(), SIGNAL(RPCError(QString, QString, int, QJsonObject)), this,
            SLOT(onRpcError(QString, QString, int, QJsonObject)));
    onDownloadLimitChanged();
    int maxCount = Settings::getInstance()->getMaxDownloadTaskNumber();
    int threadTask = Settings::getInstance()->getOriginalAddressThreadsNumber();
    int maxResource = Settings::getInstance()->getMaxDownloadResourcesNumber();
    int count = 0;
    if (maxResource > 0) {
        count = maxCount > maxResource / threadTask ? maxResource / threadTask : maxCount;
    } else {
        count = maxCount;
    }
    onMaxDownloadTaskNumberChanged(count);
    qDebug() << "MainFrame initAria2 Finished";
}

void MainFrame::initDbus()
{
    qDebug() << "[MainFrame] initDbus function started";
    QDBusConnection::sessionBus().unregisterService("com.downloader.service");
    QDBusConnection::sessionBus().registerService("com.downloader.service");
    QDBusConnection::sessionBus().registerObject("/downloader/path", this, QDBusConnection ::ExportAllSlots);
    qDebug() << "[MainFrame] initDbus function ended";
}

void MainFrame::initTabledata()
{
    qDebug() << "[MainFrame] initTabledata function started";
    //m_ptableDataControl->initTabledata();
    QList<TaskInfo> list;
    TaskInfo tbTask;

    DBInstance::getAllTask(list);
    for (TaskInfo info : list) {
        DownloadDataItem *data = new DownloadDataItem;
        initDataItem(data, info);
        if (data->status != Global::DownloadTaskStatus::Removed) {
            if (data->status == Global::DownloadTaskStatus::Lastincomplete) {
                if (Settings::getInstance()->getAutostartUnfinishedTaskState()) { //启动后自动下载未完成的任务
                    startDownloadTask(data);
                }
            }
            m_DownLoadingTableView->getTableModel()->append(data);
        } else {
            DeleteDataItem *deldata = new DeleteDataItem;
            initDelDataItem(data, deldata);
            delete data;
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
    qDebug() << "[MainFrame] initTabledata function ended";
}

void MainFrame::setTaskNum()
{
    qDebug() << "[MainFrame] setTaskNum function started";
    int activeNum = 0;
    if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
        activeNum = m_DownLoadingTableView->getTableModel()->renderList().count();
    } else {
        activeNum = m_RecycleTableView->getTableModel()->recyleList().count();
    }

    if (activeNum == 0) {
        qDebug() << "[MainFrame] No active tasks, showing no task widget";
        m_NotaskWidget->show();
        if (m_CurrentTab == CurrentTab::downloadingTab) {
            m_NotaskTipLabel->show();
        } else {
            m_NotaskTipLabel->hide();
        }
    } else {
        qDebug() << "[MainFrame] Active tasks found, hiding no task widget";
        m_NotaskWidget->hide();
        m_NotaskTipLabel->hide();
    }
    m_TaskNum->setText(QString::number(activeNum) + tr(" files"));
    qDebug() << "[MainFrame] setTaskNum function ended";
}

void MainFrame::setPaletteType()
{
    qDebug() << "[MainFrame] setPaletteType function started";
#if QT_VERSION_MAJOR > 5
    DPalette pb = m_LeftList->viewport()->palette();
#else
    DPalette pb = DApplicationHelper::instance()->palette(m_LeftList->viewport());
#endif
    pb.setBrush(DPalette::Base, QColor(0, 0, 0, 0));
    m_LeftList->setPalette(pb);

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        qDebug() << "[MainFrame] setPaletteType function started with DarkType";
        DPalette deepthemePalette;
        deepthemePalette.setBrush(DPalette::Window,
                                  DGuiApplicationHelper::instance()->applicationPalette().base());
        m_LeftWidget->setPalette(deepthemePalette);
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        DPalette labelPalette;
        labelPalette.setBrush(DPalette::Text,
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
        palette.setColor(DPalette::Window, c);
        m_TaskNumWidget->setPalette(palette);
        m_NotaskLabel->setWindowOpacity(0.2);
        m_DownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading_dark", QIcon(":/icons/deepin/builtin/dark/actions/dcc_list_downloading_dark_11px.svg")));
        m_DownloadFinishItem->setIcon(QIcon::fromTheme("dcc_print_done_dark", QIcon(":/icons/deepin/builtin/dark/actions/dcc_print_done_dark_11px.svg")));
        m_RecycleItem->setIcon(QIcon::fromTheme("dcc_list_delete_dark", QIcon(":/icons/deepin/builtin/dark/actions/dcc_list_delete_dark_11px.svg")));
        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText,
                                 DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_TaskNum->setPalette(notaskTipLabelP);
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        qDebug() << "[MainFrame] setPaletteType function started with LightType";
        DPalette p;
        p.setBrush(DPalette::Window,
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
        palette.setColor(DPalette::Window, c);
        m_TaskNumWidget->setPalette(palette);
        m_NotaskLabel->setWindowOpacity(0.2);
        m_DownloadingItem->setIcon(QIcon::fromTheme("dcc_list_downloading", QIcon(":/icons/deepin/builtin/dark/actions/dcc_list_downloading_dark_11px.svg")));
        m_DownloadFinishItem->setIcon(QIcon::fromTheme("dcc_print_done", QIcon(":/icons/deepin/builtin/light/actions/dcc_print_done_11px.svg")));
        m_RecycleItem->setIcon(QIcon::fromTheme("dcc_list_delete", QIcon(":/icons/deepin/builtin/light/actions/dcc_list_delete_11px.svg")));
        //m_pdownloadingItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_pdownloadfinishItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());
        //m_precycleItem->setBackground(DGuiApplicationHelper::instance()->applicationPalette().base());

        DPalette notaskTipLabelP;
        notaskTipLabelP.setBrush(DPalette::WindowText, DGuiApplicationHelper::instance()->applicationPalette().textTips());
        m_TaskNum->setPalette(notaskTipLabelP);
    }
    qDebug() << "[MainFrame] setPaletteType function ended";
}

void MainFrame::onSettingsMenuClicked()
{
    qDebug() << "[MainFrame] onSettingsMenuClicked function started";
    DSettingsDialog settingsDialog(this);
    settingsDialog.setFixedWidth(795);
    settingsDialog.setAccessibleName("DSettingsDialog");
    settingsDialog.setObjectName("DSettingsDialog");

    settingsDialog.widgetFactory()->registerWidget("filechooseredit", Settings::createFileChooserEditHandle);
    settingsDialog.widgetFactory()->registerWidget("httpdownload", Settings::createHttpDownloadEditHandle);
    settingsDialog.widgetFactory()->registerWidget("btdownload", Settings::createBTDownloadEditHandle);
    settingsDialog.widgetFactory()->registerWidget("metalinkdownload", Settings::createMetalinkdownloadEditHandle);
    settingsDialog.widgetFactory()->registerWidget("magneticdownload", Settings::createMagneticDownloadEditHandle);
    settingsDialog.widgetFactory()->registerWidget("diskcacheInfo", Settings::createDiskCacheSettiingLabelHandle);
    settingsDialog.widgetFactory()->registerWidget("downloadspeedlimitsetting", Settings::createDownloadSpeedLimitSettiingHandle);
    settingsDialog.widgetFactory()->registerWidget("notificationsSettiing",
                                                   Settings::createNotificationsSettiingHandle);
    settingsDialog.widgetFactory()->registerWidget("autodownloadbyspeed",
                                                   Settings::createAutoDownloadBySpeedHandle);
    settingsDialog.widgetFactory()->registerWidget("prioritydownloadbysize",
                                                   Settings::createPriorityDownloadBySizeHandle);
    settingsDialog.widgetFactory()->registerWidget("limitmaxnumber",
                                                   Settings::createLimitMaxNumberHandle);
    settingsDialog.widgetFactory()->registerWidget("customcombobox",
                                                   Settings::createAddressThreadHandle);
    settingsDialog.widgetFactory()->registerWidget("maxdownloadtask",
                                                   Settings::createMaxDownloadTaskHandle);
    settingsDialog.widgetFactory()->registerWidget("autoopenplugin",
                                                   Settings::createAutoOpenHandle);
    settingsDialog.widgetFactory()->registerWidget("autodeleteplugin",
                                                   Settings::createAutoDeleteHandle);
    settingsDialog.widgetFactory()->registerWidget("autosortbySpeedplugin",
                                                   Settings::createAutoSortBySpeedHandle);

    settingsDialog.updateSettings("Settings", Settings::getInstance()->m_settings);

    Settings::getInstance()->setAutoStart(isAutoStart());
    onIsMetalinkDownload(Settings::getInstance()->getMLDownloadState());
    onIsBtDownload(Settings::getInstance()->getBtDownloadState());

    for (int i = 0; i < settingsDialog.children().count(); i++) {
        for (int j = 0; j < settingsDialog.children().at(i)->children().count(); j++) {
            DPushButton *p = settingsDialog.children().at(i)->children().at(j)->findChild<DPushButton *>("SettingsContentReset");
            connect(p, &QPushButton::released,
                    this, [=]() {
                        HttpAdvancedSettingWidget h;
                        h.reset();
                    });
        }
    }
    settingsDialog.exec();
    Settings::getInstance()->m_settings->sync();
    qDebug() << "[MainFrame] onSettingsMenuClicked function ended";
}

void MainFrame::onClipboardDataChanged(QString url)
{
    qDebug() << "[MainFrame] onClipboardDataChanged function started with URL:" << url;
    if (!m_CopyUrlFromLocal) {
        qDebug() << "[MainFrame] URL not from local, creating new task";
        createNewTask(url);
    } else {
        qDebug() << "[MainFrame] URL from local, resetting flag";
        m_CopyUrlFromLocal = false;
    }
    qDebug() << "[MainFrame] onClipboardDataChanged function ended";
}

void MainFrame::OpenFile(const QString &url)
{
    qDebug() << "[MainFrame] OpenFile function started with URL:" << url;
    if (!Func::isNetConnect()) {
        qDebug() << "[MainFrame] No network connection, showing network error message";
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    bool bIsBt = Settings::getInstance()->getStartAssociatedBTFileState();
    bool bIsMl = Settings::getInstance()->getStartAssociatedMetaLinkFileState();
    if ((!bIsBt && url.endsWith(".torrent")) || (!bIsMl && url.endsWith(".metalink"))) {
        qDebug() << "[MainFrame] URL does not end with .torrent or .metalink, returning";
        return;
    }
    QString savePath = Settings::getInstance()->getDownloadSavePath();
    if (!QFile::exists(url)) {
        qDebug() << "[MainFrame] URL does not exist, returning";
        return;
    }
    BtInfoDialog btDiag(url, savePath, this); // torrent文件路径
    QMap<QString, QVariant> opt;
    QString infoName;
    QString infoHash;
    bool isOneClick = Settings::getInstance()->getOneClickDownloadState();
    if (isOneClick) {
        if (!btDiag.onBtnOK()) {
            qDebug() << "[MainFrame] Button OK not clicked, returning";
            return;
        }
        btDiag.getBtInfo(opt, infoName, infoHash);
        bool ret;
        if (url.endsWith(".metalink")) {
            qDebug() << "[MainFrame] Downloading new metalink";
            ret = onDownloadNewMetalink(url, opt, infoName);
        } else {
            qDebug() << "[MainFrame] Downloading new torrent";
            ret = onDownloadNewTorrent(url, opt, infoName, infoHash);
        }
        if (ret) {
            qDebug() << "[MainFrame] Downloading, showing notification";
            btNotificaitonSettings(tr("Downloading"), QString(tr("Downloading %1...")).arg(infoName), true);
        }
        return;
    }
    if (Settings::getInstance()->getNewTaskShowMainWindowState()) {
        qDebug() << "[MainFrame] Showing main window";
        Raise();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        //btDiag.move(pos().x() + this->width() / 2 - btDiag.width() / 2, pos().y() + this->height() / 2 - btDiag.height() / 2);
        Dtk::Widget::moveToCenter(this);
    } else {
        qDebug() << "[MainFrame] Window not showing main window";
        if (windowState() == Qt::WindowMinimized) {
            qDebug() << "[MainFrame] Window minimized, hiding";
            hide();
        }
    }
    int ret = btDiag.exec();

    //    if (!Settings::getInstance()->getNewTaskShowMainWindowState()) {
    //        setWindowState(windowState() & Qt::WindowMinimized);
    //        showMinimized();
    //    }
    if (ret == QDialog::Accepted) {
        qDebug() << "[MainFrame] Dialog accepted, downloading";
        btDiag.getBtInfo(opt, infoName, infoHash);
        if (url.endsWith(".metalink")) {
            qDebug() << "[MainFrame] Downloading new metalink";
            ret = onDownloadNewMetalink(url, opt, infoName);
        } else {
            qDebug() << "[MainFrame] Downloading new torrent";
            ret = onDownloadNewTorrent(url, opt, infoName, infoHash);
        }
        if (ret) {
            qDebug() << "[MainFrame] Downloading, showing notification";
            btNotificaitonSettings(tr("Downloading"), QString(tr("Downloading %1...")).arg(infoName), true);
        }
    }
    qDebug() << "[MainFrame] OpenFile function ended";
}

void MainFrame::onListClicked(const QModelIndex &index)
{
    qDebug() << "[MainFrame] onListClicked function started";
    if (m_CurrentTab == index.row()) {
        clearTableItemCheckStatus();
        qDebug() << "[MainFrame] Current tab matches index, returning";
        return;
    }
    m_CurrentTab = static_cast<CurrentTab>(index.row());
    clearTableItemCheckStatus();
    emit saveNameBeforChangeList();
    m_ToolBar->enablePauseBtn(false);
    m_ToolBar->enableStartBtn(false);
    m_ToolBar->enableDeleteBtn(false);

    if ((index.row() == 0) || (index.row() == 1)) {
        qDebug() << "[MainFrame] Refreshing table view";
        m_DownLoadingTableView->refreshTableView(index.row());
        m_DownLoadingTableView->horizontalHeader()->reset();
        m_DownLoadingTableView->reset(true);
        setTaskNum();
        if (index.row() == 1) {
            m_NotaskLabel->setText(tr("No finished tasks"));
            m_DownLoadingTableView->getTableHeader()->setSortIndicator(4, Qt::AscendingOrder);
        } else {
            m_NotaskLabel->setText(tr("No download tasks"));
            if (Settings::getInstance()->getAutoSortBySpeed()) {
                m_DownLoadingTableView->getTableHeader()->setSortIndicator(5, Qt::AscendingOrder);
            } else {
                m_DownLoadingTableView->getTableHeader()->setSortIndicator(6, Qt::AscendingOrder);
            }
        }
        m_RightStackwidget->setCurrentIndex(0);
    } else {
        qDebug() << "[MainFrame] Setting task number and refreshing table view";
        setTaskNum();
        m_NotaskLabel->setText(tr("No deleted tasks"));
        m_RecycleTableView->getTableHeader()->setSortIndicator(4, Qt::AscendingOrder);
        m_RightStackwidget->setCurrentIndex(1);
    }
    emit tableChanged(index.row());
    qDebug() << "[MainFrame] onListClicked function ended";
}

void MainFrame::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    qDebug() << "[MainFrame] onPalettetypechanged function started";
    Q_UNUSED(type);
    setPaletteType();
    qDebug() << "[MainFrame] onPalettetypechanged function ended";
}

void MainFrame::onHeaderStatechanged(bool isChecked)
{
    qDebug() << "[MainFrame] onHeaderStatechanged function started";
    // ToolBar禁用按钮联动：表头全选复选框状体变化 begin
    int cnt = (m_CurrentTab == recycleTab ? m_RecycleTableView->getTableModel()->rowCount()
                                          : m_DownLoadingTableView->getTableModel()->rowCount());
    if (0 == cnt) {
        qDebug() << "[MainFrame] No items in table, returning";
        emit isHeaderChecked(false);
        return;
    }

    int activeCount = 0;
    int pauseCount = 0;
    int errorCount = 0;
    int hasFileCount = 0;
    if (m_CurrentTab == CurrentTab::downloadingTab && isChecked) {
        qDebug() << "[MainFrame] Downloading tab, getting select list";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto pDownloadItem : selectList) {
            if (pDownloadItem->status == Global::DownloadTaskStatus::Active) {
                ++activeCount;
            }
            if ((pDownloadItem->status == Global::DownloadTaskStatus::Paused) || (pDownloadItem->status == Global::DownloadTaskStatus::Lastincomplete)) {
                ++pauseCount;
            }
            if (pDownloadItem->status == Global::DownloadTaskStatus::Error) {
                ++errorCount;
            }
        }
    } else if (m_CurrentTab == recycleTab && isChecked) {
        qDebug() << "[MainFrame] Recycle tab, getting select list";
        const QList<DeleteDataItem *> &selectList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto pItem : selectList) {
            if (QFile::exists(pItem->savePath)) {
                ++hasFileCount;
            }
        }
    }
    if (m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "[MainFrame] Downloading tab, enabling buttons";
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
        qDebug() << "[MainFrame] Recycle tab, enabling buttons";
        if (hasFileCount && isChecked) {
            m_ToolBar->enablePauseBtn(true);
        } else {
            m_ToolBar->enablePauseBtn(false);
        }
        m_ToolBar->enableStartBtn(isChecked);
    }

    if ((m_CurrentTab == CurrentTab::downloadingTab) || (m_CurrentTab == CurrentTab::finishTab)) {
        qDebug() << "[MainFrame] Downloading or finish tab, setting item check status";
        const QList<DownloadDataItem *> &renderList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto data : renderList) {
            data->isChecked = isChecked;
        }
        m_DownLoadingTableView->reset();
    } else {
        qDebug() << "[MainFrame] Recycle tab, setting item check status";
        const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto data : recycleList) {
            data->isChecked = isChecked;
        }
        m_RecycleTableView->reset();
    }
    qDebug() << "[MainFrame] onHeaderStatechanged function ended";
}

void MainFrame::onDownloadNewUrl(QString url, QString savePath, QString fileName, QString type, QString leng)
{
    qDebug() << "[MainFrame] onDownloadNewUrl function started with URL:" << url;
    // 将url加入数据库和aria
    TaskInfo task;
    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath);
    getNameFromUrl(task, url, savePath, fileName, leng, type);
    DBInstance::addTask(task);
    qDebug() << task.gid << "   "; //<< task.url;
    Aria2RPCInterface::instance()->addNewUri(task.url, savePath, task.downloadFilename, task.taskId);
    //clearTableItemCheckStatus();
    emit isHeaderChecked(false);
    if (m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "[MainFrame] Downloading tab, hiding no task widget";
        m_NotaskWidget->hide();
    }
    m_UpdateTimer->start(m_timeInterval);
    qDebug() << "[MainFrame] onDownloadNewUrl function ended";
}

bool MainFrame::onDownloadNewTorrent(QString btPath, QMap<QString, QVariant> &opt, QString infoName, QString infoHash)
{
    qDebug() << "[MainFrame] onDownloadNewTorrent function started with btPath:" << btPath;
    Aria2cBtInfo info = Aria2RPCInterface::instance()->getBtInfo(btPath);
    if (info.infoHash.isEmpty()) {
        qDebug() << "[MainFrame] Torrent file not exist or broken";
        MessageBox msg;
        msg.setWarings(tr("Torrent file not exist or broken"), tr("OK"));
        msg.exec();
        return false;
    }
    QString selectedNum = opt.value("select-file").toString();

    if (selectedNum.isEmpty()) {
        qDebug() << "select is null";
        return false;
    }

    // 数据库是否已存在相同的地址
    if (!checkIsHasSameTask(infoHash.toLower())) {
        qDebug() << "[MainFrame] Same task already exists, returning";
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
    TaskInfoHash urlInfo;
    urlInfo.taskId = strId;
    urlInfo.url = "";
    urlInfo.downloadType = "torrent";
    urlInfo.filePath = btPath;
    urlInfo.selectedNum = selectedNum;
    urlInfo.infoHash = infoHash;
    DBInstance::addBtTask(urlInfo);

    //等待500毫秒，aria2删除bt任务有延时
    //    QTime time;
    //    time.start();
    //    while (time.elapsed() < 300) {
    //        QCoreApplication::processEvents();
    //    }
    //opt.insert("out", infoName);
    // 开始下载
    Aria2RPCInterface::instance()->addTorrent(btPath, opt, strId);
    clearTableItemCheckStatus();

    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    for (auto pItem : dataList) {
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
    m_NotaskWidget->hide();
    qDebug() << "[MainFrame] onDownloadNewTorrent function ended";
    return true;
}

bool MainFrame::onDownloadNewMetalink(QString linkPath, QMap<QString, QVariant> &opt, QString infoName)
{
    qDebug() << "[MainFrame] onDownloadNewMetalink function started with linkPath:" << linkPath;
    QString selectedNum = opt.value("select-file").toString();

    // 数据库是否已存在相同的地址
    if (!checkIsHasSameTask(Func::pathToMD5(linkPath).toLower())) {
        return false;
    }

    QString strId = QUuid::createUuid().toString();
    // 将任务添加如UrlInfo表中
    TaskInfoHash urlInfo;
    urlInfo.taskId = strId;
    urlInfo.url = "";
    urlInfo.downloadType = "metalink";
    urlInfo.filePath = linkPath;
    urlInfo.selectedNum = selectedNum;
    urlInfo.infoHash = Func::pathToMD5(linkPath);
    DBInstance::addBtTask(urlInfo);

    // 将任务添加如task表中
    TaskInfo task;

    task.taskId = strId;
    task.gid = "";
    task.gidIndex = 0;
    task.url = "";
    task.downloadPath = Settings::getInstance()->getCustomFilePath() + "/" + infoName;
    task.downloadFilename = infoName;
    task.createTime = QDateTime::currentDateTime();
    DBInstance::addTask(task);

    // 开始下载
    Aria2RPCInterface::instance()->addMetalink(linkPath, opt, strId);
    clearTableItemCheckStatus();

    // 定时器打开
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
    m_NotaskWidget->hide();
    qDebug() << "[MainFrame] onDownloadNewMetalink function ended";
    return true;
}

void MainFrame::getNameFromUrl(TaskInfo &task, QString url, QString savePath, QString name, QString fileLength, QString type)
{
    qDebug() << "[MainFrame] getNameFromUrl function started with url:" << url;
    // 获取url文件名
    QString fileName;

    if (!name.isEmpty()) {
        qDebug() << "[MainFrame] getNameFromUrl function started with name:" << name;
        fileName = name;
    } else if (url.startsWith("magnet")) {
        qDebug() << "[MainFrame] getNameFromUrl function started with magnet";
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
    //    if (!fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
    //        const QByteArray byte = fileName.toLatin1();
    //        QString decode = QUrl::fromPercentEncoding(byte);
    //        if (decode.contains("?")) {
    //            decode = decode.split("?")[0];
    //        }
    //        fileName = decode;
    //    }
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
    task.fileLength = fileLength;
    QString urlDecode = QUrl::fromPercentEncoding(fileName.toUtf8());
    task.downloadPath = savePath + "/" + urlDecode;
    task.downloadFilename = urlDecode;
    task.createTime = QDateTime::currentDateTime();
    qDebug() << "[MainFrame] getNameFromUrl function ended";
    return;
}

void MainFrame::continueDownload(DownloadDataItem *pItem)
{
    qDebug() << "[MainFrame] continueDownload function started";
    m_ToolBar->enablePauseBtn(true);
    m_ToolBar->enableStartBtn(false);
    if (pItem->status != Global::DownloadTaskStatus::Active) {
        qDebug() << "status is not Active";
        if (pItem->status == Global::DownloadTaskStatus::Lastincomplete || pItem->status == Global::DownloadTaskStatus::Error) {
            startDownloadTask(pItem);
        } else {
            qDebug() << "status is Lastincomplete or Error";
            Aria2RPCInterface::instance()->unpause(pItem->gid, pItem->taskId);
            if (m_UpdateTimer->isActive() == false) {
                m_UpdateTimer->start(m_timeInterval);
            }
        }
    }
    qDebug() << "[MainFrame] continueDownload function ended";
}

void MainFrame::onContextMenu(const QPoint &pos)
{
    qDebug() << "[MainFrame] onContextMenu function started";
    if (m_CurrentTab == recycleTab) {
        qDebug() << "Recycle tab, getting index at pos";
        QModelIndex index = m_RecycleTableView->indexAt(pos);
        if (index.isValid()) {
            qDebug() << "Index is valid";
            QModelIndex realIndex = index.sibling(index.row(), 0);
            QString gid = m_RecycleTableView->getTableModel()->data(index, TableModel::taskId).toString();
            m_DelCheckItem = m_RecycleTableView->getTableModel()->find(gid, 0);
            m_CheckItem = nullptr;
            if (!m_RecycleTableView->getTableModel()->data(index, TableModel::Ischecked).toBool()) {
                onHeaderStatechanged(false);
            }
            m_RecycleTableView->getTableModel()->setData(realIndex, true, TableModel::Ischecked);
        }
    } else {
        qDebug() << "Downloading tab, getting index at pos";
        QModelIndex index = m_DownLoadingTableView->indexAt(pos);
        if (index.isValid()) {
            qDebug() << "Index is valid";
            QModelIndex realIndex = index.sibling(index.row(), 0);
            QString gid = m_DownLoadingTableView->getTableModel()->data(index, TableModel::taskId).toString();
            m_CheckItem = m_DownLoadingTableView->getTableModel()->find(gid);
            m_CheckIndex = index;
            m_DelCheckItem = nullptr;
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
    int waitCount = 0;
    int existFileCount = 0;
    DownloadDataItem *pDownloadItem = nullptr;
    DeleteDataItem *pDeleteItem = nullptr;
    if (m_CurrentTab == recycleTab) {
        qDebug() << "Recycle tab, getting recyle list";
        const QList<DeleteDataItem *> &recyleList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto item : recyleList) {
            if (item->isChecked) {
                chkedCnt++;
                pDeleteItem = item;
                if (QFileInfo(item->savePath).exists()) {
                    existFileCount++;
                }
            }
        }
    } else {
        qDebug() << "Downloading tab, getting select list";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : selectList) {
            if (item->isChecked) {
                chkedCnt++;
                pDownloadItem = item;
                if (pDownloadItem->status == Global::DownloadTaskStatus::Active) {
                    ++activeCount;
                }
                if ((pDownloadItem->status == Global::DownloadTaskStatus::Paused) || (pDownloadItem->status == Global::DownloadTaskStatus::Lastincomplete)) {
                    ++pauseCount;
                }
                if (pDownloadItem->status == Global::DownloadTaskStatus::Error) {
                    ++errorCount;
                }
                if (pDownloadItem->status == Global::DownloadTaskStatus::Waiting) {
                    ++waitCount;
                }
            }
        }
    }

    if (chkedCnt == 0) {
        qDebug() << "[MainFrame] No checked items, returning";
        return;
    }

    QMenu *delmenlist = new QMenu(this);
    delmenlist->setObjectName("tableMenu");
    if (m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "[MainFrame] Downloading tab, adding actions";
        if (pauseCount > 0 || errorCount > 0) {
            qDebug() << "Adding resume action";
            QAction *pActionStart = new QAction(this);
            pActionStart->setText(tr("Resume"));
            pActionStart->setObjectName("ContextResume");
            delmenlist->addAction(pActionStart);
            connect(pActionStart, &QAction::triggered, this, &MainFrame::onStartDownloadBtnClicked);
        }
        if (activeCount > 0) {
            qDebug() << "Adding pause action";
            QAction *pActionPause = new QAction(this);
            pActionPause->setText(tr("Pause"));
            pActionPause->setObjectName("ContextPause");
            delmenlist->addAction(pActionPause);
            connect(pActionPause, &QAction::triggered, this, &MainFrame::onPauseDownloadBtnClicked);
        }
        if (chkedCnt == 1 && waitCount == 1) {
            qDebug() << "Adding download first action";
            QAction *pActionDownloadNow = new QAction(this);
            pActionDownloadNow->setText(tr("Download first"));
            pActionDownloadNow->setObjectName("ContextDownloadFirst");
            delmenlist->addAction(pActionDownloadNow);
            connect(pActionDownloadNow, &QAction::triggered, this, &MainFrame::onDownloadFirstBtnClicked);
        } else if (chkedCnt == 1 && pauseCount == 1) {
            qDebug() << "[MainFrame] Downloading tab, adding download first action";
            const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
            int activec = 0;
            for (auto item : selectList) {
                if (item->status == Global::DownloadTaskStatus::Active) {
                    ++activec;
                }
            }
            if (activec == Settings::getInstance()->getMaxDownloadTaskNumber()) {
                qDebug() << "[MainFrame] Downloading tab, adding download first action";
                QAction *pActionDownloadNow = new QAction(this);
                pActionDownloadNow->setObjectName("ContextDownloadFirst");
                pActionDownloadNow->setText(tr("Download first"));
                delmenlist->addAction(pActionDownloadNow);
                connect(pActionDownloadNow, &QAction::triggered, this, &MainFrame::onDownloadFirstBtnClicked);
            }
        }
        if ((errorCount > 0) && (1 == chkedCnt)) {
            qDebug() << "[MainFrame] Downloading tab, adding download again action";
            QAction *pActionredownload = new QAction(this);
            pActionredownload->setText(tr("Download again"));
            pActionredownload->setObjectName("ContextDownloadAgain");
            delmenlist->addAction(pActionredownload);
            connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        }
        delmenlist->addSeparator();
    }

    if (m_CurrentTab == CurrentTab::recycleTab && existFileCount > 0) {
        qDebug() << "[MainFrame] Recycle tab, adding restore action";
        QAction *returnedToOrigin = new QAction(this);
        returnedToOrigin->setText(tr("Restore"));
        returnedToOrigin->setObjectName("ContextRestore");
        delmenlist->addAction(returnedToOrigin);
        connect(returnedToOrigin, &QAction::triggered, this, &MainFrame::onReturnOriginActionTriggered);
    }
    if ((m_CurrentTab == CurrentTab::recycleTab || m_CurrentTab == CurrentTab::finishTab) && (1 == chkedCnt)) {
        qDebug() << "[MainFrame] Recycle or finish tab, adding download again action";
        QAction *pActionredownload = new QAction(this);
        pActionredownload->setText(tr("Download again"));
        pActionredownload->setObjectName("ContextDownloadAgain");
        delmenlist->addAction(pActionredownload);
        connect(pActionredownload, &QAction::triggered, this, &MainFrame::onRedownloadActionTriggered);
        if ((m_CurrentTab == CurrentTab::finishTab && QFileInfo(pDownloadItem->savePath).exists()) || (m_CurrentTab == CurrentTab::recycleTab && QFileInfo(pDeleteItem->savePath).exists())) {
            qDebug() << "[MainFrame] Recycle or finish tab, adding open action";
            QAction *pActionopenFile = new QAction(this);
            pActionopenFile->setText(tr("Open"));
            pActionopenFile->setObjectName("ContextOpen");
            delmenlist->addAction(pActionopenFile);
            connect(pActionopenFile, &QAction::triggered, this, &MainFrame::onOpenFileActionTriggered);
        }
    }
    if ((1 == chkedCnt && (m_CurrentTab == CurrentTab::finishTab) && QFileInfo(pDownloadItem->savePath).exists())
        || (1 == chkedCnt && m_CurrentTab == CurrentTab::recycleTab && QFileInfo(pDeleteItem->savePath).exists())) {
        qDebug() << "[MainFrame] Recycle or finish tab, adding open folder action";
        QAction *pActionopenFoler = new QAction(this);
        pActionopenFoler->setText(tr("Open folder"));
        pActionopenFoler->setObjectName("ContextOpenFolder");
        delmenlist->addAction(pActionopenFoler);

        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "[MainFrame] Finish tab, getting select list";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        int noFileCount = 0;
        for (auto item : selectList) {
            if (item->isChecked) {
                if (!QFileInfo(item->savePath).exists()) {
                    noFileCount++;
                }
            }
        }

        if (1 == chkedCnt && QFileInfo(pDownloadItem->savePath).exists()) {
            qDebug() << "[MainFrame] Finish tab, adding rename action";
            QAction *pactionRename = new QAction(this);
            pactionRename->setObjectName("ContextRename");
            pactionRename->setText(tr("Rename"));
            delmenlist->addAction(pactionRename);
            delmenlist->addSeparator();
            connect(pactionRename, &QAction::triggered, this, &MainFrame::onRenameActionTriggered);
        }
        if (!noFileCount) {
            qDebug() << "[MainFrame] Finish tab, adding move to action";
            QAction *pActionMove = new QAction(this);
            pActionMove->setText(tr("Move to"));
            pActionMove->setObjectName("ContextMoveto");
            delmenlist->addAction(pActionMove);
            delmenlist->addSeparator();
            connect(pActionMove, &QAction::triggered, this, &MainFrame::onMoveToActionTriggered);
        }
    }

    if (1 == chkedCnt && m_CurrentTab == CurrentTab::recycleTab) {
        qDebug() << "[MainFrame] Recycle tab, adding copy download link action";
        QAction *pactionCopyDownloadUrl = new QAction(this);
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        pactionCopyDownloadUrl->setObjectName("ContextCopyDownloadLink");
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == CurrentTab::downloadingTab) {
            delmenlist->addSeparator();
        }
    }

    QAction *pactiondelDownloading = new QAction(this);
    pactiondelDownloading->setText(tr("Delete"));
    pactiondelDownloading->setObjectName("ContextDelete");
    delmenlist->addAction(pactiondelDownloading);
    connect(pactiondelDownloading, &QAction::triggered, this, &MainFrame::onDeleteActionTriggered);

    QAction *pactionDeletePermanently = new QAction(this);
    pactionDeletePermanently->setText(tr("Permanently delete"));
    pactionDeletePermanently->setObjectName("ContextPermanentlyDelete");
    delmenlist->addAction(pactionDeletePermanently);
    connect(pactionDeletePermanently, &QAction::triggered, this, &MainFrame::onDeletePermanentActionTriggered);

    if (1 == chkedCnt && m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "[MainFrame] Finish tab, adding copy download link action";
        QAction *pactionCopyDownloadUrl = new QAction(this);
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        pactionCopyDownloadUrl->setObjectName("ContextCopyDownloadLink");
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == CurrentTab::downloadingTab) {
            delmenlist->addSeparator();
        }
    }

    if (1 == chkedCnt && m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "[MainFrame] Downloading tab, adding copy download link action";
        QAction *pactionCopyDownloadUrl = new QAction(this);
        pactionCopyDownloadUrl->setText(tr("Copy download link"));
        pactionCopyDownloadUrl->setObjectName("ContextCopyDownloadLink");
        delmenlist->addAction(pactionCopyDownloadUrl);
        delmenlist->addSeparator();
        connect(pactionCopyDownloadUrl, &QAction::triggered, this, &MainFrame::onCopyUrlActionTriggered);
        if (m_CurrentTab == CurrentTab::downloadingTab) {
            delmenlist->addSeparator();
        }
        QAction *pActionopenFoler = new QAction(this);
        pActionopenFoler->setText(tr("Open folder"));
        pActionopenFoler->setObjectName("ContextOpenFolder");
        delmenlist->addAction(pActionopenFoler);
        connect(pActionopenFoler, &QAction::triggered, this, &MainFrame::onOpenFolderActionTriggered);
    }

    if (m_CurrentTab == recycleTab && 1 < chkedCnt) {
        qDebug() << "[MainFrame] Recycle tab, adding delete all action";
        QAction *pActionClearRecycle = new QAction(this);
        pActionClearRecycle->setText(tr("Delete all"));
        pActionClearRecycle->setObjectName("ContextDeleteAll");
        delmenlist->addAction(pActionClearRecycle);
        connect(pActionClearRecycle, &QAction::triggered, this, &MainFrame::onClearRecyleActionTriggered);
    }

    delmenlist->exec(QCursor::pos());
    delete delmenlist;
    qDebug() << "[MainFrame] onContextMenu function ended";
}

void MainFrame::onCheckChanged(bool checked, int flag)
{
    qDebug() << "[MainFrame] onCheckChanged function started with checked:" << checked << "flag:" << flag;
    Q_UNUSED(checked);
    Q_UNUSED(flag);
    // ToolBar禁用按钮联动：列表内复选框状态变化 begin
    int chkedCnt = 0;
    int fileExistCnt = 0;

    if (m_CurrentTab == recycleTab) {
        qDebug() << "[MainFrame] Recycle tab, getting select list";
        const QList<DeleteDataItem *> &recyleList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto item : recyleList) {
            if (item->isChecked) {
                m_DelCheckItem = item;
                m_CheckItem = nullptr;
                chkedCnt++;
                if (QFileInfo::exists(item->savePath)) {
                    fileExistCnt++;
                }
            }
        }
    } else {
        qDebug() << "[MainFrame] Downloading tab, getting select list";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : selectList) {
            if (item->isChecked) {
                m_CheckItem = item;
                m_DelCheckItem = nullptr;
                chkedCnt++;
                if (QFileInfo::exists(item->savePath)) {
                    fileExistCnt++;
                }
            }
        }
    }

    if (chkedCnt > 0) {
        qDebug() << "[MainFrame] Downloading or finish tab, enabling buttons";
        if (m_CurrentTab == CurrentTab::downloadingTab) {
            qDebug() << "[MainFrame] Downloading tab, enabling buttons";
            m_ToolBar->enableStartBtn(true);
            m_ToolBar->enablePauseBtn(true);
            m_ToolBar->enableDeleteBtn(true);
            if (m_CheckItem->status == DownloadTaskStatus::Paused || m_CheckItem->status == DownloadTaskStatus::Lastincomplete) {
                m_ToolBar->enablePauseBtn(false);
            } else {
                m_ToolBar->enablePauseBtn(true);
            }
            if (m_CheckItem->status == DownloadTaskStatus::Active) {
                m_ToolBar->enableStartBtn(false);
            } else {
                m_ToolBar->enableStartBtn(true);
            }

        } else if (m_CurrentTab == CurrentTab::finishTab) {
            qDebug() << "[MainFrame] Finish tab, enabling buttons";
            m_ToolBar->enableDeleteBtn(true);
            if (1 == chkedCnt && fileExistCnt > 0) {
                m_ToolBar->enableStartBtn(true);
                m_ToolBar->enablePauseBtn(true);
            } else {
                m_ToolBar->enableStartBtn(false);
                m_ToolBar->enablePauseBtn(false);
            }
        } else if (m_CurrentTab == CurrentTab::recycleTab) {
            qDebug() << "[MainFrame] Recycle tab, enabling buttons";
            m_ToolBar->enableStartBtn(true);
            if (fileExistCnt > 0) {
                m_ToolBar->enablePauseBtn(true);
            } else {
                m_ToolBar->enablePauseBtn(false);
            }
            m_ToolBar->enableDeleteBtn(true);
        }
    } else {
        qDebug() << "[MainFrame] No checked items, disabling buttons";
        m_ToolBar->enableStartBtn(false);
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
        if (m_CurrentTab == CurrentTab::recycleTab && m_RecycleTableView->getTableModel()->recyleList().count() > 0) {
            qDebug() << "[MainFrame] Recycle tab, enabling start button";
            m_ToolBar->enableStartBtn(true);
        }
    }
    qDebug() << "[MainFrame] onCheckChanged function ended";
}

void MainFrame::clearTableItemCheckStatus()
{
    qDebug() << "[MainFrame] clearTableItemCheckStatus function started";
    if ((m_CurrentTab == CurrentTab::downloadingTab) || (m_CurrentTab == CurrentTab::finishTab)) {
        qDebug() << "[MainFrame] Downloading or finish tab, clearing check status";
        const QList<DownloadDataItem *> &renderList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : renderList) {
            item->isChecked = false;
        }
        m_DownLoadingTableView->reset();
    } else {
        qDebug() << "[MainFrame] Recycle tab, clearing check status";
        const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto item : recycleList) {
            item->isChecked = false;
        }
        m_RecycleTableView->reset();
    }
    emit isHeaderChecked(false);
    qDebug() << "[MainFrame] clearTableItemCheckStatus function ended";
}

void MainFrame::onSearchEditTextChanged(QString text)
{
    qDebug() << "[MainFrame] onSearchEditTextChanged function started with text:" << text;
    static SearchResoultWidget *searchWidget = new SearchResoultWidget(this);
    if (text.isEmpty()) {
        qDebug() << "text is empty, hiding search widget";
        searchWidget->hide();
        return;
    }
    static int flag = 1;
    if (flag) {
        qDebug() << "onSearchEditTextChanged function, connecting signals";
        connect(searchWidget, &SearchResoultWidget::itemClicked, this, &MainFrame::onSearchItemClicked);
        connect(m_ToolBar, &TopButton::SearchEditKeyPressed, searchWidget, &SearchResoultWidget::onKeypressed);
        connect(m_ToolBar->getSearchEdit()->lineEdit(), &QLineEdit::editingFinished, this, [&]() {
            searchWidget->hide();
        });
        flag = 0;
    }
    QPoint p = m_ToolBar->getSearchEditPosition();
    QPoint pp = QPoint(p.x(), p.y());
    searchWidget->move(pp);
    m_SearchContent = text;
    QList<QString> taskIDList;
    QList<int> taskStatusList;
    QList<QString> tasknameList;
    m_DownLoadingTableView->getTableControl()->searchEditTextChanged(text, taskIDList, taskStatusList, tasknameList);
    m_RecycleTableView->getTableControl()->searchEditTextChanged(text, taskIDList, taskStatusList, tasknameList);

    searchWidget->setData(taskIDList, taskStatusList, tasknameList);
    if (!taskIDList.isEmpty()) {
        qDebug() << "onSearchEditTextChanged function, showing search widget";
        searchWidget->show();
    } else {
        qDebug() << "onSearchEditTextChanged function, hiding search widget";
        searchWidget->hide();
    }
    qDebug() << "[MainFrame] onSearchEditTextChanged function ended";
}

void MainFrame::onRedownload(QString taskId, int rd)
{
    qDebug() << "[MainFrame] onRedownload function started with taskId:" << taskId << "rd:" << rd;
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
    qDebug() << "[MainFrame] onRedownload function ended";
}

void MainFrame::onRemoveFinished()
{
    // qDebug() << "[MainFrame] onRemoveFinished function started";
    m_ToolBar->enableCreateTaskBtn(true);
}

void MainFrame::showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList)
{
    qDebug() << "[MainFrame] showWarningMsgbox function started with title:" << title;
    MessageBox msg(this);
    msg.setAccessibleName(title);
    msg.setWarings(title, tr("OK"), "", sameUrlCount, sameUrlList);
    msg.exec();
    qDebug() << "[MainFrame] showWarningMsgbox function ended";
}

void MainFrame::showClearMsgbox()
{
    qDebug() << "[MainFrame] showClearMsgbox function started";
    MessageBox msg(this);
    msg.setAccessibleName("Clearrecycle");
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
    qDebug() << "[MainFrame] showClearMsgbox function ended";
}

void MainFrame::onClearRecycle(bool ischecked)
{
    qDebug() << "[MainFrame] onClearRecycle function started with ischecked:" << ischecked;
    const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();

    if (ischecked) { //删除源文件
        qDebug() << "[MainFrame] onClearRecycle function, deleting source files";
        for (auto item : recycleList) {
            QString ariaTempFile = item->savePath + ".aria2";
            if (!item->savePath.isEmpty()) {
                QFile::remove(item->savePath);
                if (QFile::exists(ariaTempFile)) {
                    QFile::remove(ariaTempFile);
                }
            }
            Aria2RPCInterface::instance()->removeDownloadResult(item->gid);
        }
    }
    for (auto item : recycleList) {
        DBInstance::delTask(item->taskId);
    }

    m_RecycleTableView->getTableModel()->removeRecycleItems();
    if (m_RecycleTableView->getTableModel()->recyleList().isEmpty()) {
        qDebug() << "Recycle list is empty, setting header checked to false";
        emit isHeaderChecked(false);
    }
    qDebug() << "[MainFrame] onClearRecycle function ended";
}

void MainFrame::showDeleteMsgbox(bool permanently)
{
    qDebug() << "[MainFrame] showDeleteMsgbox function started with permanently:" << permanently;
    MessageBox msg(this);
    msg.setAccessibleName("Deletedownload");
    connect(&msg, &MessageBox::Deletedownload, this, &MainFrame::onDeleteConfirm);
    if (m_CurrentTab == CurrentTab::downloadingTab) {
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

bool MainFrame::showRedownloadMsgbox(const QString sameUrl, bool ret, bool isShowRedownload)
{
    qDebug() << "[MainFrame] showRedownloadMsgbox function started with sameUrl:" << sameUrl;
    MessageBox msg(this);
    msg.setAccessibleName("Redownload");
    msg.setRedownload(sameUrl, ret, isShowRedownload);
    int rs = msg.exec();
    if (rs == DDialog::Accepted) {
        return true;
    }
    qDebug() << "[MainFrame] showRedownloadMsgbox function ended with result:" << false;
    return false;
}

void MainFrame::showDiagnosticTool()
{
    qDebug() << "[MainFrame] showDiagnosticTool function started";
    DiagnosticTool control(this);
    connect(this, &MainFrame::ariaOption, &control, &DiagnosticTool::onAriaOption);
    // 最小改动：在对话框显示前主动获取一次全局配置，避免诊断时序导致的误判
    Aria2RPCInterface::instance()->getGlobalOption();
    control.exec();
    qDebug() << "[MainFrame] showDiagnosticTool function ended";
}

void MainFrame::onAria2Remove(QString gId, QString id)
{
    // qDebug() << "[MainFrame] onAria2Remove function started with gId:" << gId << "id:" << id;
    Aria2RPCInterface::instance()->remove(gId, id);
}

void MainFrame::onDeleteConfirm(bool ischecked, bool permanent)
{
    qDebug() << "[MainFrame] onDeleteConfirm function started with ischecked:" << ischecked << "permanent:" << permanent;
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

    //    if (!m_SearchContent.isEmpty()) {
    //        onSearchEditTextChanged(m_SearchContent);
    //    }
    if (m_UpdateTimer->isActive() == false) {
        m_UpdateTimer->start(m_timeInterval);
    }
    qDebug() << "[MainFrame] onDeleteConfirm function ended";
}

QString MainFrame::getDownloadSavepathFromConfig()
{
    // qDebug() << "[MainFrame] getDownloadSavepathFromConfig function started";
    return Settings::getInstance()->getDownloadSavePath();
}

void MainFrame::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "[MainFrame] keyPressEvent function started";
    if (event->key() == Qt::Key_Control) {
        // qDebug() << "[MainFrame] keyPressEvent function, key_control pressed";
        m_CtrlkeyPress = true;
        qDebug() << "Key_Control";
    }
    if (event->key() == Qt::Key_A) {
        // qDebug() << "[MainFrame] keyPressEvent function, key_a pressed";
        if (m_CtrlkeyPress) {
            onHeaderStatechanged(true);
            emit isHeaderChecked(true);
            qDebug() << "Key_Control + Key_A";
        }
    }
    QWidget::keyPressEvent(event);
    // qDebug() << "[MainFrame] keyPressEvent function ended";
}

void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
    // qDebug() << "[MainFrame] keyReleaseEvent function started";
    if (m_CtrlkeyPress) {
        // qDebug() << "[MainFrame] keyReleaseEvent function, key_control released";
        m_CtrlkeyPress = false;
    }
    QWidget::keyReleaseEvent(event);
    // qDebug() << "[MainFrame] keyReleaseEvent function ended";
}

void MainFrame::onNewBtnClicked()
{
    qDebug() << "New task button clicked";
    createNewTask("");
}

void MainFrame::onStartDownloadBtnClicked()
{
    qDebug() << "Start download button clicked for tab:" << m_CurrentTab;
    if (!Func::isNetConnect()) {
        qWarning() << "No network connection available";
        m_TaskWidget->showNetErrorMsg();
        return;
    }

    if (m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "Resuming selected downloads";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : selectList) {
            if (item->isChecked) {
                continueDownload(item);
            }
        }
    } else if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "Opening folder for finish tab";
        onOpenFolderActionTriggered();
    } else {
        qDebug() << "Clearing recycle list";
        onClearRecyleActionTriggered();
    }
    qDebug() << "[MainFrame] onStartDownloadBtnClicked function ended";
}

void MainFrame::onPauseDownloadBtnClicked()
{
    qDebug() << "Pause download button clicked for tab:" << m_CurrentTab;
    if (!Func::isNetConnect()) {
        qWarning() << "No network connection available";
        m_TaskWidget->showNetErrorMsg();
        return;
    }

    if (m_CurrentTab == CurrentTab::downloadingTab) {
        qDebug() << "Pausing selected downloads";
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableStartBtn(true);
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : selectList) {
            if (item->isChecked) {
                if (item->status != Global::DownloadTaskStatus::Paused) {
                    TaskInfoHash info;
                    DBInstance::getBtTaskById(item->taskId, info);
                    if (info.downloadType == "torrent" || item->savePath.contains("[METADATA]")) {
                        Aria2RPCInterface::instance()->forcePause(item->gid, item->taskId);
                    } else {
                        Aria2RPCInterface::instance()->pause(item->gid, item->taskId);
                    }
                    QDateTime finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");

                    TaskStatus getStatus;
                    TaskStatus downloadStatus(item->taskId,
                                              Global::DownloadTaskStatus::Paused,
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
    } else if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "Opening file for finish tab";
        onOpenFileActionTriggered();
    } else {
        qDebug() << "Returning to origin for finish tab";
        onReturnOriginActionTriggered();
    }
}

void MainFrame::onDownloadFirstBtnClicked()
{
    qDebug() << "[MainFrame] onDownloadFirstBtnClicked function started";
    showWarningMsgbox(tr("The number of max. concurrent tasks reached. Other tasks will be queuing."));
    const QList<DownloadDataItem *> &list = m_DownLoadingTableView->getTableModel()->renderList();
    const DownloadDataItem *lowSpeedItem = nullptr;
    double speed = 99999;
    for (const DownloadDataItem *item : list) {
        if (item->status == Global::DownloadTaskStatus::Active
            && Func::formatSpeed(item->speed) >= 0
            && Func::formatSpeed(item->speed) < speed) {
            speed = Func::formatSpeed(item->speed);
            lowSpeedItem = item;
        }
    }
    if (m_CheckItem == nullptr) {
        qDebug() << "m_CheckItem is nullptr";
        return;
    }
    if (m_CheckItem->status == Global::DownloadTaskStatus::Paused) {
        Aria2RPCInterface::instance()->unpause(m_CheckItem->gid, m_CheckItem->taskId);
#if QT_VERSION_MAJOR > 5
        QElapsedTimer time;
#else
        QTime time;
#endif
        time.start();
        while (time.elapsed() < 400) {
            QCoreApplication::processEvents();
        }
    }
    Aria2RPCInterface::instance()->changePosition(m_CheckItem->gid, 0);
    if (lowSpeedItem == nullptr) {
        qDebug() << "lowSpeedItem is nullptr";
        return;
    }
    Aria2RPCInterface::instance()->forcePause(lowSpeedItem->gid, lowSpeedItem->taskId);
    QTimer::singleShot(800, this, [=]() {
        Aria2RPCInterface::instance()->unpause(lowSpeedItem->gid, lowSpeedItem->taskId);
    });
    qDebug() << "[MainFrame] onDownloadFirstBtnClicked function ended";
}

void MainFrame::onDeleteDownloadBtnClicked()
{
    qDebug() << "Delete button clicked";
    onDeleteActionTriggered();
}

void MainFrame::onRpcSuccess(QString method, QJsonObject json)
{
    qDebug() << "[MainFrame] onRpcSuccess function started with method:" << method;
    if ((method == ARIA2C_METHOD_ADD_URI)
        || (method == ARIA2C_METHOD_ADD_TORRENT)
        || (method == ARIA2C_METHOD_ADD_METALINK)) {
        qDebug() << "ARIA2C_METHOD_ADD_URI, ARIA2C_METHOD_ADD_TORRENT, ARIA2C_METHOD_ADD_METALINK";
        m_DownLoadingTableView->getTableControl()->aria2MethodAdd(json, m_SearchContent);
    } else if (method == ARIA2C_METHOD_TELL_STATUS) {
        qDebug() << "ARIA2C_METHOD_TELL_STATUS";
        m_DownLoadingTableView->getTableControl()->aria2MethodStatusChanged(json, m_CurrentTab, m_SearchContent);
    } else if (method == ARIA2C_METHOD_SHUTDOWN) {
        qDebug() << "ARIA2C_METHOD_SHUTDOWN";
        m_DownLoadingTableView->getTableControl()->aria2MethodShutdown(json);
    } else if (method == ARIA2C_METHOD_GET_FILES) {
        qDebug() << "ARIA2C_METHOD_GET_FILES";
        m_DownLoadingTableView->getTableControl()->aria2MethodGetFiles(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_UNPAUSE) {
        qDebug() << "ARIA2C_METHOD_UNPAUSE";
        m_DownLoadingTableView->getTableControl()->aria2MethodUnpause(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_FORCE_REMOVE) {
        qDebug() << "ARIA2C_METHOD_FORCE_REMOVE";
        m_DownLoadingTableView->getTableControl()->aria2MethodForceRemove(json);
    } else if (method == ARIA2C_METHOD_REMOVE) {
        qDebug() << "ARIA2C_METHOD_REMOVE";
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
            qDebug() << "ARIA2C_METHOD_REMOVE: " << id;
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
        qDebug() << "ARIA2C_METHOD_UNPAUSE_ALL";
        m_DownLoadingTableView->getTableControl()->aria2MethodUnpauseAll(json, m_CurrentTab);
    } else if (method == ARIA2C_METHOD_GET_GLOBAL_STAT) {
        qDebug() << "ARIA2C_METHOD_GET_GLOBAL_STAT";
        m_DownLoadingTableView->getTableControl()->aria2GetGlobalStatus(json);
    } else if (method == ARIA2C_METHOD_GET_GLOBAL_OPTION) {
        qDebug() << "ARIA2C_METHOD_GET_GLOBAL_OPTION";
        QJsonObject obj = json.value("result").toObject();
        QString tracker = obj.value("bt-tracker").toString();
        // 最小改动：稳健解析 enable-dht（可能为字符串或布尔），避免因类型不同导致误判
        bool isHasDHT = obj.value("enable-dht").toVariant().toBool();
        emit ariaOption(!tracker.isEmpty(), isHasDHT);
    }
    qDebug() << "[MainFrame] onRpcSuccess function ended";
}

void MainFrame::onRpcError(QString method, QString id, int error, QJsonObject obj)
{
    qDebug() << "[MainFrame] onRpcError function started with method:" << method;
    QJsonObject result = obj.value("error").toObject();
    int errNo = result.value("code").toInt();
    QString message = result.value("message").toString();
    qDebug() << "slot rpc error method is:" << method << error << message;

    if (1 == errNo) {
        qDebug() << "errNo is 1";
        if (message.contains("cannot be paused now")) { //暂停失败，采用强制暂停
            DownloadDataItem *item = m_DownLoadingTableView->getTableModel()->find(id);
            if (nullptr != item) {
                Aria2RPCInterface::instance()->forcePause(item->gid, "");
            }
        } else if (message.contains("No URI to download.")) { //url错误，弹窗提示
            qDebug() << "No URI to download.";
            DBInstance::delTask(id);
            static bool isMsg = true;
            static MessageBox msg(this);
            if (msg.isVisible()) {
                return;
            }
            if (isMsg) {
                msg.setWarings(tr("Unable to parse the URL, please check"), tr("OK"));
                isMsg = false;
            }
            msg.exec();
        }
    }
    // save_data_before_close();
    if (error == 400) {
        qDebug() << "error is 400";
        if ((method == ARIA2C_METHOD_FORCE_REMOVE) && id.startsWith("REDOWNLOAD_")) {
            // 当重新下载400移除失败时直接重新下载
            QStringList sp = id.split("_");
            QString taskId = sp.at(2);
            int rd = sp.at(1).toInt();
            emit redownload(taskId, rd);
        }
        if (method == ARIA2C_METHOD_REMOVE) {
            qDebug() << "ARIA2C_METHOD_REMOVE";
            QString id = obj.value("id").toString();
            qDebug() << "ARIA2C_METHOD_REMOVE" << obj << "******************" << id;
            DownloadDataItem *item = m_DownLoadingTableView->getTableModel()->find(id);
            if (item != nullptr) {
                m_DownLoadingTableView->getTableModel()->removeItem(item);
            }
        }
    }
    qDebug() << "[MainFrame] onRpcError function ended";
}

void MainFrame::onTableItemSelected(const QModelIndex &selected)
{
    qDebug() << "[MainFrame] onTableItemSelected function started with selected:" << selected;
    bool chked = selected.model()->data(selected, TableModel::DataRole::Ischecked).toBool();

    if (m_CtrlkeyPress == false && selected.column() != 0) {
        qDebug() << "CtrlkeyPress is false and selected column is not 0";
        if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
            const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->renderList();
            for (auto item : dataList) {
                item->isChecked = false;
            }
            m_DownLoadingTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                             true,
                                                             TableModel::Ischecked);
            m_DownLoadingTableView->reset();
        } else {
            qDebug() << "Current tab is recycle tab";
            const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
            for (auto item : recycleList) {
                item->isChecked = false;
            }
            m_RecycleTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                         true,
                                                         TableModel::Ischecked);
            m_RecycleTableView->reset();
        }
    } else if (m_CtrlkeyPress == true || selected.column() == 0) {
        qDebug() << "CtrlkeyPress is true or selected column is 0";
        m_DownLoadingTableView->reset();
        m_RecycleTableView->reset();
        if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
            qDebug() << "Current tab is downloading tab or finish tab";
            m_DownLoadingTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                             !chked,
                                                             TableModel::Ischecked);
            m_DownLoadingTableView->reset();
        } else {
            qDebug() << "Current tab is recycle tab";
            m_RecycleTableView->getTableModel()->setData(selected.model()->index(selected.row(), 0),
                                                         !chked,
                                                         TableModel::Ischecked);
            m_RecycleTableView->reset();
        }
    }
    qDebug() << "[MainFrame] onTableItemSelected function ended";
}

void MainFrame::onUpdateMainUI()
{
    qDebug() << "[MainFrame] onUpdateMainUI function started";
    static int flag = 0;
    flag++;
    if (flag >= 5) {
        qDebug() << "flag is greater than or equal to 5";
        m_DownLoadingTableView->getTableControl()->updateDb();
        flag = 0;
    }
    Aria2RPCInterface::instance()->getGlobalSatat();
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();

    int activeCount = 0;
    m_ShutdownOk = true;
    for (const auto item : dataList) {
        if ((item->status == Global::DownloadTaskStatus::Active) || (item->status == Global::DownloadTaskStatus::Waiting)) {
            Aria2RPCInterface::instance()->tellStatus(item->gid, item->taskId);
        }
        if ((item->status == Global::DownloadTaskStatus::Active) || (item->status == Global::DownloadTaskStatus::Waiting)
            || (item->status == Global::DownloadTaskStatus::Lastincomplete)) {
            ++activeCount;
        }
        if (item->status == Global::DownloadTaskStatus::Active) {
            m_ShutdownOk = false;
        }
    }

    if (activeCount == 0) {
        qDebug() << "activeCount is 0";
        if (m_UpdateTimer->isActive()) {
            m_UpdateTimer->stop();
            m_NotaskLabel->show();
            if (m_CurrentTab == CurrentTab::downloadingTab) {
                m_NotaskTipLabel->show();
            }
            m_DownLoadingTableView->getTableControl()->updateDb();
        }
    }
    setTaskNum();
    qDebug() << "[MainFrame] onUpdateMainUI function ended";
}

void MainFrame::onDeleteActionTriggered()
{
    qDebug() << "[MainFrame] onDeleteActionTriggered function started";
    int selectedCount = 0;

    if (m_CurrentTab == recycleTab) {
        qDebug() << "Current tab is recycle tab";
        selectedCount = m_RecycleTableView->getTableControl()->onDelAction(m_CurrentTab);
    } else {
        qDebug() << "Current tab is downloading tab or finish tab";
        selectedCount = m_DownLoadingTableView->getTableControl()->onDelAction(m_CurrentTab);
    }
    showDeleteMsgbox(false);
    qDebug() << "[MainFrame] onDeleteActionTriggered function ended";
}

void MainFrame::onRedownloadActionTriggered()
{
    qDebug() << "[MainFrame] onRedownloadActionTriggered function started";
    if (!Func::isNetConnect()) {
        qDebug() << "No network connection";
        m_TaskWidget->showNetErrorMsg();
        return;
    }
    if (m_CurrentTab == CurrentTab::recycleTab) {
        qDebug() << "Current tab is recycle tab";
        if (QFileInfo::exists(m_DelCheckItem->savePath)) {
            qDebug() << "savePath exists";
            MessageBox msg(this);
            msg.setAccessibleName("Redownload");
            msg.setRedownload(m_DelCheckItem->fileName, true);
            int rs = msg.exec();
            if (rs != DDialog::Accepted) {
                return;
            }
        }
    } else if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "Current tab is finish tab";
        if (QFileInfo::exists(m_CheckItem->savePath)) {
            qDebug() << "savePath exists";
            MessageBox msg(this);
            msg.setAccessibleName("Redownload");
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
    if ((m_CurrentTab == CurrentTab::downloadingTab) || (m_CurrentTab == CurrentTab::finishTab)) {
        qDebug() << "Current tab is downloading tab or finish tab";
        savePath = m_CheckItem->savePath;
        fileName = m_CheckItem->fileName;
        url = m_CheckItem->url;
        taskId = m_CheckItem->taskId;
    } else {
        qDebug() << "Current tab is recycle tab";
        savePath = m_DelCheckItem->savePath;
        fileName = m_DelCheckItem->fileName;
        url = m_DelCheckItem->url;
        taskId = m_DelCheckItem->taskId;
    }

    if (url.isEmpty()) { //bt任务
        qDebug() << "bt task";
        TaskInfo task;
        QMap<QString, QVariant> opt;

        QString filePath = QString(savePath).left(savePath.lastIndexOf('/'));
        TaskInfoHash info;
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

        TaskInfoHash urlInfo;
        urlInfo.taskId = strId;
        urlInfo.url = "";
        urlInfo.downloadType = "torrent";
        urlInfo.filePath = info.filePath;
        urlInfo.selectedNum = info.selectedNum;
        urlInfo.infoHash = info.infoHash;
        DBInstance::addBtTask(urlInfo);

        deleteTaskByTaskID(taskId);
        QString selectNum = info.selectedNum;
        opt.insert("select-file", selectNum);
        opt.insert("dir", task.downloadPath);
        Aria2RPCInterface::instance()->addTorrent(info.filePath, opt, strId);
    } else { // 非bt任务
        qDebug() << "non-bt task";
        TaskInfo task;
        QMap<QString, QVariant> opt;
        opt.insert("dir", savePath);
        QString filePath = QString(savePath).left(savePath.lastIndexOf('/'));
        QString totalLength;
        if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
            qDebug() << "Current tab is downloading tab or finish tab";
            totalLength = m_CheckItem->totalLength;
        } else {
            qDebug() << "Current tab is recycle tab";
            totalLength = m_DelCheckItem->totalLength;
        }

        deleteTaskByUrl(url);
        getNameFromUrl(task, url, filePath, fileName, totalLength, "");

        DBInstance::addTask(task);
        Aria2RPCInterface::instance()->addNewUri(task.url, filePath, task.downloadFilename, task.taskId);
    }
    emit isHeaderChecked(false);
    m_NotaskWidget->hide();
    m_UpdateTimer->start(m_timeInterval);
    qDebug() << "[MainFrame] onRedownloadActionTriggered function ended";
}

void MainFrame::onReturnOriginActionTriggered()
{
    qDebug() << "[MainFrame] onReturnOriginActionTriggered function started";
    int selectedCount = 0;
    const QList<DeleteDataItem *> &recycleList = m_RecycleTableView->getTableModel()->recyleList();
    int size = recycleList.size();
    for (int i = size - 1; i >= 0; i--) {
        DeleteDataItem *data = recycleList.at(i);
        TaskStatus getStatus;
        DBInstance::getTaskStatusById(data->taskId, getStatus);
        if ((data->isChecked) && QFileInfo(data->savePath).exists()) {
            DownloadDataItem *returntoData = new DownloadDataItem;
            ++selectedCount;
            if (data->completedLength == data->totalLength) {
                if (data->totalLength != "0B") {
                    returntoData->status = Global::DownloadTaskStatus::Complete;
                    getStatus.downloadStatus = Global::DownloadTaskStatus::Complete;
                } else {
                    returntoData->status = Global::DownloadTaskStatus::Lastincomplete;
                    getStatus.downloadStatus = Global::DownloadTaskStatus::Lastincomplete;
                }
            } else {
                if ((data->completedLength != "0B") && (data->totalLength != "0B")) {
                    double d = returntoData->completedLength.toLong() * 100.0 / returntoData->totalLength.toLong();
                    returntoData->percent = static_cast<int>(d);

                    if ((returntoData->percent < 0) || (returntoData->percent > 100)) {
                        returntoData->status = Global::DownloadTaskStatus::Lastincomplete;
                        getStatus.downloadStatus = Global::DownloadTaskStatus::Lastincomplete;
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
            if (returntoData->status == Global::DownloadTaskStatus::Removed || getStatus.downloadStatus == Global::DownloadTaskStatus::Removed) {
                returntoData->status = Global::DownloadTaskStatus::Lastincomplete;
                getStatus.downloadStatus = Global::DownloadTaskStatus::Lastincomplete;
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
                TaskInfoHash taskInfo;
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
        qDebug() << "selectedCount is 0";
        //showWarningMsgbox(tr("no item is selected,please check items!"));
    } else {
        // ToolBar禁用按钮联动：还原后禁用按钮
        if (m_RecycleTableView->getTableModel()->recyleList().size() > 0) {
            qDebug() << "m_RecycleTableView->getTableModel()->recyleList().size() > 0";
            m_ToolBar->enableStartBtn(true);
        } else {
            qDebug() << "m_RecycleTableView->getTableModel()->recyleList().size() <= 0";
            m_ToolBar->enableStartBtn(false);
        }
        m_ToolBar->enablePauseBtn(false);
        m_ToolBar->enableDeleteBtn(false);
    }
    if (m_RecycleTableView->getTableModel()->recyleList().isEmpty()) {
        qDebug() << "m_RecycleTableView->getTableModel()->recyleList().isEmpty()";
        emit isHeaderChecked(false);
    }
    qDebug() << "[MainFrame] onReturnOriginActionTriggered function ended";
}

void MainFrame::onOpenFileActionTriggered()
{
    qDebug() << "[MainFrame] onOpenFileActionTriggered function started";
    if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "Current tab is finish tab";
        QString path = QString("file:///") + m_CheckItem->savePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    } else if (m_CurrentTab == recycleTab) {
        qDebug() << "Current tab is recycle tab";
        QString path = QString("file:///") + m_DelCheckItem->savePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
    qDebug() << "[MainFrame] onOpenFileActionTriggered function ended";
}

void MainFrame::onOpenFolderActionTriggered()
{
    qDebug() << "[MainFrame] onOpenFolderActionTriggered function started";
    if (m_CurrentTab == recycleTab) {
        qDebug() << "Current tab is recycle tab";
        QString filePath = m_DelCheckItem->savePath.left(m_DelCheckItem->savePath.length()
                                                         - m_DelCheckItem->savePath.split('/').last().length());
        QString path = QString("file:///") + filePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    } else {
        qDebug() << "Current tab is downloading tab or finish tab";
        QString filePath = m_CheckItem->savePath.left(m_CheckItem->savePath.length()
                                                      - m_CheckItem->savePath.split('/').last().length());
        QString path = QString("file:///") + filePath;
        QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
    }
    qDebug() << "[MainFrame] onOpenFolderActionTriggered function ended";
}

void MainFrame::onRenameActionTriggered()
{
    qDebug() << "[MainFrame] onRenameActionTriggered function started";
    m_DownLoadingTableView->setCurrentIndex(m_CheckIndex);
    m_DownLoadingTableView->edit(m_CheckIndex.sibling(m_CheckIndex.row(), 1));
    qDebug() << "[MainFrame] onRenameActionTriggered function ended";
}

void MainFrame::onMoveToActionTriggered()
{
    qDebug() << "[MainFrame] onMoveToActionTriggered function started";
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec() != QDialog::Accepted) {
        qDebug() << "fileDialog is not accepted";
        return;
    }

    QStringList fileName = fileDialog.selectedFiles();
    QString filePath = fileName.first();
    if (!filePath.isEmpty()) {
        qDebug() << "filePath is not empty";
        const QList<DownloadDataItem *> &selectList = m_DownLoadingTableView->getTableModel()->renderList();
        for (auto item : selectList) {
            if ((item->status == DownloadTaskStatus::Complete) && (item->isChecked)) {
                DownloadDataItem *data = item;
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
    qDebug() << "[MainFrame] onMoveToActionTriggered function ended";
}

void MainFrame::onClearRecyleActionTriggered()
{
    qDebug() << "[MainFrame] onClearRecyleActionTriggered function started";
    showClearMsgbox();
}

void MainFrame::onCopyUrlActionTriggered()
{
    qDebug() << "[MainFrame] onCopyUrlActionTriggered function started";
    TaskInfoHash getUrlInfo;
    QString url;
    if (m_CurrentTab == CurrentTab::downloadingTab || m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "[MainFrame] onCopyUrlActionTriggered function, getting url from downloading tab";
        DBInstance::getBtTaskById(m_CheckItem->taskId, getUrlInfo);
        if (!getUrlInfo.taskId.isEmpty()) {
            qDebug() << "getUrlInfo.taskId is not empty";
            if (getUrlInfo.downloadType == "torrent") {
                url = "magnet:?xt=urn:btih:" + getUrlInfo.infoHash;
            } else if (getUrlInfo.downloadType == "metalink") {
                url = getUrlInfo.filePath;
            }
        } else {
            qDebug() << "getUrlInfo.taskId is empty";
            url = m_CheckItem->url;
        }
    } else if (m_CurrentTab == recycleTab) {
        qDebug() << "[MainFrame] onCopyUrlActionTriggered function, getting url from recycle tab";
        DBInstance::getBtTaskById(m_DelCheckItem->taskId, getUrlInfo);
        if (!getUrlInfo.taskId.isEmpty()) {
            qDebug() << "getUrlInfo.taskId is not empty";
            if (getUrlInfo.downloadType == "torrent") {
                url = "magnet:?xt=urn:btih:" + getUrlInfo.infoHash;
            }
        } else {
            qDebug() << "getUrlInfo.taskId is empty";
            url = m_DelCheckItem->url;
        }
    }

    m_CopyUrlFromLocal = true;
    QClipboard *clipboard = DApplication::clipboard();
    clipboard->setText(url);
    //m_TaskNum->setText(tr("Copied to clipboard"));

    QString showInfo(tr("Copied to clipboard"));
    btNotificaitonSettings("", showInfo);
    qDebug() << "[MainFrame] onCopyUrlActionTriggered function ended";
}

void MainFrame::onDeletePermanentActionTriggered()
{
    qDebug() << "[MainFrame] onDeletePermanentActionTriggered function started";
    if (m_CurrentTab == recycleTab) {
        qDebug() << "Current tab is recycle tab";
        m_RecycleTableView->getTableControl()->onDelAction(m_CurrentTab);
    } else {
        qDebug() << "Current tab is downloading tab or finish tab";
        m_DownLoadingTableView->getTableControl()->onDelAction(m_CurrentTab);
    }
    showDeleteMsgbox(true);
    qDebug() << "[MainFrame] onDeletePermanentActionTriggered function ended";
}

void MainFrame::onTableViewItemDoubleClicked(QModelIndex index)
{
    qDebug() << "[MainFrame] onTableViewItemDoubleClicked function started";
    if (m_CurrentTab == CurrentTab::finishTab) {
        qDebug() << "Current tab is finish tab";
        QString taskId = m_DownLoadingTableView->getTableModel()->data(index, TableModel::taskId).toString();
        m_CheckItem = m_DownLoadingTableView->getTableModel()->find(taskId);
        onOpenFileActionTriggered();
    }
}

void MainFrame::onDownloadLimitChanged()
{
    qDebug() << "[MainFrame] onDownloadLimitChanged function started";
    QTime currentTime = QTime::currentTime();
    QTime periodStartTime;
    QTime periodEndTime;
    QString downloadSpeed, uploadSpeed;

    // get_limit_speed_time(period_start_time, period_end_time);
    DownloadSettings settings = Settings::getInstance()->getAllSpeedLimitInfo();
    if ("0" == settings.m_type) {
        qDebug() << "settings.m_type is 0";
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
        qDebug() << "bInPeriod is false";
        Aria2RPCInterface::instance()->setDownloadUploadSpeed("0", "0");
    } else {
        qDebug() << "bInPeriod is true";
        Aria2RPCInterface::instance()->setDownloadUploadSpeed(downloadSpeed, uploadSpeed);
    }
    qDebug() << "[MainFrame] onDownloadLimitChanged function ended";
}

void MainFrame::onPowerOnChanged(bool isPowerOn)
{
    qDebug() << "[MainFrame] onPowerOnChanged function started with isPowerOn:" << isPowerOn;
    // setAutoStart(isPowerOn);
    QString autostartDesktop = "downloader.desktop";
    QString defaultDesktop = "downloader.desktop";
    QString userDefaultDesktopPath = QString("%1/autostart/")
                                         .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    if (isPowerOn == 1) {
        qDebug() << "isPowerOn is 1";
        QString cmd = QString("cp %1 %2").arg(UOS_DOWNLOAD_MANAGER_DESKTOP_PATH + defaultDesktop).arg(userDefaultDesktopPath);
        qDebug() << cmd;
        char *ch;
        QByteArray ba = cmd.toLatin1();
        ch = ba.data();
        system(ch);
    } else {
        qDebug() << "isPowerOn is 0";
        QString cmd = QString("rm -f %1").arg(userDefaultDesktopPath + defaultDesktop);
        char *ch;
        QByteArray ba = cmd.toLatin1();
        ch = ba.data();
        system(ch);
    }
    qDebug() << "[MainFrame] onPowerOnChanged function ended";
}

void MainFrame::onMaxDownloadTaskNumberChanged(int nTaskNumber, bool isStopTask, bool isAddOne)
{
    qDebug() << "[MainFrame] onMaxDownloadTaskNumberChanged function started with nTaskNumber:" << nTaskNumber << "isStopTask:" << isStopTask << "isAddOne:" << isAddOne;
    static int maxDownloadTaskCount = 0;
    if (nTaskNumber != 1 || isAddOne == false) {
        qDebug() << "nTaskNumber is not 1 or isAddOne is false";
        maxDownloadTaskCount = nTaskNumber;
    } else if (isAddOne) {
        qDebug() << "isAddOne is true";
        if (maxDownloadTaskCount < 20) {
            maxDownloadTaskCount += 1;
        } else {
            qDebug() << "maxDownloadTaskCount is greater than or equal to 20";
            return;
        }
    }
    QMap<QString, QVariant> opt;
    QString value = QString("max-concurrent-downloads=%1").arg(maxDownloadTaskCount);

    Aria2RPCInterface::instance()->modifyConfigFile("max-concurrent-downloads=", value);
    opt.insert("max-concurrent-downloads", QString().number(maxDownloadTaskCount));
    Aria2RPCInterface::instance()->changeGlobalOption(opt);

    if (nTaskNumber == 1 && isAddOne) {
        qDebug() << "nTaskNumber is 1 and isAddOne is true";
        return;
    }
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    int activeCount = 0;
    m_ShutdownOk = true;
    if (isStopTask) {
        qDebug() << "isStopTask is true";
        for (const auto item : dataList) { //暂停掉之前已经开始的多余下载总数的任务
            if (item->status == Global::DownloadTaskStatus::Active) {
                activeCount++;
                if (activeCount > maxDownloadTaskCount) {
                    // qDebug() << "activeCount is greater than maxDownloadTaskCount";
                    TaskInfoHash info;
                    DBInstance::getBtTaskById(item->taskId, info);
                    if (info.downloadType == "torrent" || item->savePath.contains("[METADATA]")) {
                        Aria2RPCInterface::instance()->forcePause(item->gid, item->taskId);
                    } else {
                        Aria2RPCInterface::instance()->pause(item->gid, item->taskId);
                    }
                    QTimer::singleShot(500, this, [=]() {
                        Aria2RPCInterface::instance()->unpause(item->gid, item->taskId);
                    });
                    QDateTime finishTime = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
                    TaskStatus getStatus;
                    TaskStatus downloadStatus(item->taskId,
                                              Global::DownloadTaskStatus::Paused,
                                              QDateTime::currentDateTime(),
                                              item->completedLength,
                                              item->speed,
                                              item->totalLength,
                                              item->percent,
                                              item->total,
                                              finishTime);

                    if (DBInstance::getTaskStatusById(item->taskId, getStatus)) {
                        // qDebug() << "getStatus is not empty";
                        DBInstance::updateTaskStatusById(downloadStatus);
                    } else {
                        // qDebug() << "getStatus is empty";
                        DBInstance::addTaskStatus(downloadStatus);
                    }
                }
            }
        }
    }
    qDebug() << "[MainFrame] onMaxDownloadTaskNumberChanged function ended";
}

void MainFrame::onDisckCacheChanged(int nNum)
{
    qDebug() << "[MainFrame] onDisckCacheChanged function started with nNum:" << nNum;
    QMap<QString, QVariant> opt;
    QString cacheNum = QString().number(nNum) + "M";

    opt.insert("disk-cache", cacheNum);
    Aria2RPCInterface::instance()->changeGlobalOption(opt);
    QString value = "disk-cache=" + cacheNum;
    Aria2RPCInterface::instance()->modifyConfigFile("disk-cache=", value);
    qDebug() << "[MainFrame] onDisckCacheChanged function ended";
}

bool MainFrame::checkIfInPeriod(QTime *currentTime, QTime *periodStartTime, QTime *periodEndTime)
{
    qDebug() << "[MainFrame] checkIfInPeriod function started";
    int periodResult = checkTime(periodStartTime, periodEndTime);
    int cmpBeginResult = checkTime(currentTime, periodStartTime);
    int cmpEndResult = checkTime(currentTime, periodEndTime);

    // 开始时间与结束时间相同，只在相同时刻限速
    if (periodResult == 0) {
        qDebug() << "periodResult is 0";
        // 当前时间和限速时间相同，在限速时间内
        if (cmpBeginResult == 0) {
            qDebug() << "cmpBeginResult is 0";
            return true;
        }
    }
    // 开始时间，小于结束时间
    else if (periodResult == 1) {
        qDebug() << "periodResult is 1";
        // 当前时间大于开始时间，并且小于结束时间
        if ((cmpBeginResult == -1) && (cmpEndResult == 1)) {
            qDebug() << "cmpBeginResult is -1 and cmpEndResult is 1";
            return true;
        }
    }
    // 开始时间，大于结束时间，即限时为开始时间到24点，0点到结束时间
    else {
        qDebug() << "periodResult is -1";
        if ((cmpBeginResult == -1) || (cmpEndResult == 1)) {
            qDebug() << "cmpBeginResult is -1 or cmpEndResult is 1";
            return true;
        }
    }

    qDebug() << "[MainFrame] checkIfInPeriod function ended with result:" << false;
    return false;
}

int MainFrame::checkTime(const QTime *startTime, const QTime *endTime)
{
    // qDebug() << "[MainFrame] checkTime function started with startTime:" << startTime << "endTime:" << endTime;
    if (startTime->hour() == endTime->hour()) {
        // qDebug() << "startTime->hour() == endTime->hour()";
        if (startTime->minute() == endTime->minute()) {
            // qDebug() << "startTime->minute() == endTime->minute(),return 0";
            return 0;
        } else if (startTime->minute() < endTime->minute()) {
            // qDebug() << "startTime->minute() < endTime->minute(),return 1";
            return 1;
        } else {
            // qDebug() << "startTime->minute() > endTime->minute(),return -1";
            return -1;
        }
    } else if (startTime->hour() < endTime->hour()) {
        // qDebug() << "startTime->hour() < endTime->hour(),return 1";
        return 1;
    } else {
        // qDebug() << "startTime->hour() > endTime->hour(),return -1";
        return -1;
    }
}

void MainFrame::initDataItem(Global::DownloadDataItem *data, const TaskInfo &tbTask)
{
    qDebug() << "[MainFrame] initDataItem function started";
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
        qDebug() << "taskStatus is not empty";
        data->percent = taskStatus.percent;
        data->isChecked = false;
        data->totalLength = taskStatus.totalLength;
        data->completedLength = taskStatus.compeletedLength;
        if (data->url.toLower().contains("magnet:?xt=urn:btih")) {
            data->completedLength = "0KB";
        }
        if (taskStatus.downloadStatus == Global::DownloadTaskStatus::Active
            || taskStatus.downloadStatus == Global::DownloadTaskStatus::Paused
            || taskStatus.downloadStatus == Global::DownloadTaskStatus::Waiting) {
            data->status = Global::DownloadTaskStatus::Lastincomplete;
        } else {
            data->status = taskStatus.downloadStatus;
        }
        data->total = taskStatus.totalFromSource;
        if (data->status == Global::DownloadTaskStatus::Complete) {
            data->time = taskStatus.modifyTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    qDebug() << "[MainFrame] initDataItem function ended";
}

void MainFrame::initDelDataItem(Global::DownloadDataItem *data, Global::DeleteDataItem *delData)
{
    qDebug() << "[MainFrame] initDelDataItem function started";
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
    qDebug() << "[MainFrame] initDelDataItem function ended";
}

void MainFrame::onIsStartAssociatedBTFile(bool status)
{
    qDebug() << "[MainFrame] onIsStartAssociatedBTFile function started with status:" << status;
    if (status) {
        qDebug() << "status is true";
        Func::setMimeappsValue("application/x-bittorrent", "downloader.desktop");
    } else {
        qDebug() << "status is false";
        Func::setMimeappsValue("application/x-bittorrent", " ");
    }
    qDebug() << "[MainFrame] onIsStartAssociatedBTFile function ended";
}

void MainFrame::onIsControlBrowser(bool status)
{
    // qDebug() << "[MainFrame] onIsControlBrowser function started with status:" << status;
    //    if (status) {
    //        Func::setMimeappsValue("x-scheme-handler/downloader", "dlmExtensionService.desktop");
    //    } else {
    //        Func::setMimeappsValue("x-scheme-handler/downloader", " ");
    //    }
    QDBusInterface iface("com.dlmExtensionService.service",
                         "/dlmExtensionService/path",
                         "local.dlmextensionservice.Websockethandle",
                         QDBusConnection::sessionBus());
    QDBusMessage m = iface.call("sendTextToClient", status);
    QString msg = m.errorMessage();
    qDebug() << m;
    // qDebug() << "[MainFrame] onIsControlBrowser function ended";
}

void MainFrame::onIsMetalinkDownload(bool status)
{
    qDebug() << "[MainFrame] onIsMetalinkDownload function started with status:" << status;
    if (status) {
        Func::setMimeappsValue("application/metalink+xml", "downloader.desktop");
        Func::setMimeappsValue("application/metalink4+xml", "downloader.desktop");
    }
    qDebug() << "[MainFrame] onIsMetalinkDownload function ended";
}

void MainFrame::onIsBtDownload(bool status)
{
    qDebug() << "[MainFrame] onIsBtDownload function started with status:" << status;
    if (status) {
        Func::setMimeappsValue("application/x-bittorrent", "downloader.desktop");
    }
    qDebug() << "[MainFrame] onIsBtDownload function ended";
}

void MainFrame::onAutoDownloadBySpeed(bool status)
{
    qDebug() << "[MainFrame] onAutoDownloadBySpeed function started with status:" << status;
    if (!status) {
        onMaxDownloadTaskNumberChanged(Settings::getInstance()->getMaxDownloadTaskNumber(), false);
    }
    qDebug() << "[MainFrame] onAutoDownloadBySpeed function ended";
}

void MainFrame::btNotificaitonSettings(QString head, QString text, bool isBt)
{
    qDebug() << "[MainFrame] btNotificaitonSettings function started with head:" << head << "text:" << text << "isBt:" << isBt;
    // 获取免打扰模式值
    QVariant switchButton = Settings::getInstance()->m_settings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    bool downloadInfoNotify = Settings::getInstance()->getDownloadInfoSystemNotifyState();
    if (downloadInfoNotify) {
        qDebug() << "downloadInfoNotify is true";
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
    qDebug() << "[MainFrame] btNotificaitonSettings function ended";
}

void MainFrame::startDownloadTask(DownloadDataItem *pItem)
{
    qDebug() << "[MainFrame] startDownloadTask function started with pItem:" << pItem;
    QString savePath = pItem->savePath;

    QMap<QString, QVariant> opt;
    opt.insert("dir", savePath.left(savePath.lastIndexOf("/")));
    opt.insert("out", pItem->fileName);
    TaskInfoHash getUrlInfo;
    DBInstance::getBtTaskById(pItem->taskId, getUrlInfo);
    if (!getUrlInfo.taskId.isEmpty()) {
        qDebug() << "getUrlInfo.taskId is not empty";
        if (getUrlInfo.downloadType == "torrent") {
            qDebug() << "getUrlInfo.downloadType is torrent";
            QString selectNum = getUrlInfo.selectedNum;
            opt.insert("select-file", selectNum);
            if (!QFile(getUrlInfo.filePath).exists()) {
                showWarningMsgbox(tr("Torrent file not exist or broken"));
                qDebug() << "Torrent file not exist or broken";
            } else {
                Aria2RPCInterface::instance()->addTorrent(getUrlInfo.filePath,
                                                          opt,
                                                          getUrlInfo.taskId);
                if (m_UpdateTimer->isActive() == false) {
                    m_UpdateTimer->start(m_timeInterval);
                }
            }
        } else if (getUrlInfo.downloadType == "metalink") {
            qDebug() << "getUrlInfo.downloadType is metalink";
            QString selectNum = getUrlInfo.selectedNum;
            //opt.insert("dir", getUrlInfo.filePath);
            opt.insert("select-file", selectNum);
            Aria2RPCInterface::instance()->addMetalink(getUrlInfo.filePath,
                                                       opt,
                                                       getUrlInfo.taskId);
            if (m_UpdateTimer->isActive() == false) {
                m_UpdateTimer->start(m_timeInterval);
            }
        }
    } else {
        qDebug() << "getUrlInfo.taskId is empty";
        Aria2RPCInterface::instance()->addUri(pItem->url, opt, pItem->taskId);
        if (m_UpdateTimer->isActive() == false) {
            m_UpdateTimer->start(m_timeInterval);
        }
    }
    qDebug() << "[MainFrame] startDownloadTask function ended";
}

void MainFrame::Raise()
{
    qDebug() << "[MainFrame] Raise function started";
    // 恢复窗口显示
    Dtk::Widget::moveToCenter(this);
    show();
    setWindowState(Qt::WindowActive);
    activateWindow();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    //m_LeftList->setCurrentIndex(m_LeftList->currentIndex().sibling(0, 0));
    qDebug() << "[MainFrame] Raise function ended";
}

void MainFrame::onParseUrlList(QVector<LinkInfo> &urlList, QString path)
{
    qDebug() << "[MainFrame] onParseUrlList function started";
    QString size;
    if (Settings::getInstance()->getPriorityDownloadBySize(size)) { //优先下载大小小于多少的任务

        for (auto it = urlList.begin(); it != urlList.end();) {
            if (it->length < (size.toInt() * 1024 * 1024)) {
                LinkInfo link = *it;
                it = urlList.erase(it);
                urlList.prepend(link);
                it++;
            } else
                it++;
        }
    }

    QVector<LinkInfo> sameUrlList;
    for (LinkInfo info : urlList) {
        QString url = info.urlTrueLink.isEmpty() ? info.url : info.urlTrueLink;
        bool isExitsUrl = false;
        // 判断url是否在数据中已存在
        DBInstance::isExistUrl(url, isExitsUrl);
        if (isExitsUrl) {
            sameUrlList << info;
            continue;
        }

        onDownloadNewUrl(url, path, info.urlName, info.type, info.urlSize);
#if QT_VERSION_MAJOR > 5
        QElapsedTimer time;
#else
        QTime time;
#endif
        time.start();
        while (time.elapsed() < 1000) {
            QCoreApplication::processEvents();
        }
    }
    if (!sameUrlList.isEmpty()) {
        qDebug() << "sameUrlList is not empty";
        if (sameUrlList.size() == 1) {
            if (showRedownloadMsgbox(sameUrlList.at(0).url)) {
                QString taskID = DBInstance::getTaskIdByMagnet(sameUrlList.at(0).url);
                qDebug() << "taskID is not empty";
                if (sameUrlList.at(0).url.contains("magnet:?xt") && !taskID.isEmpty()) {
                    deleteTaskByTaskID(taskID);
                    QTimer::singleShot(1000, this, [=]() {
                        onDownloadNewUrl(sameUrlList.at(0).url, path, sameUrlList.at(0).urlName, sameUrlList.at(0).type, sameUrlList.at(0).urlSize);
                    });
                } else {
                    qDebug() << "taskID is empty";
                    deleteTaskByUrl(sameUrlList.at(0).url);
                    if (sameUrlList.at(0).url.contains("magnet:?xt")) {
                        QTimer::singleShot(1000, this, [=]() {
                            onDownloadNewUrl(sameUrlList.at(0).url, path, sameUrlList.at(0).urlName, sameUrlList.at(0).type, sameUrlList.at(0).urlSize);
                        });
                    } else {
                        onDownloadNewUrl(sameUrlList.at(0).url, path, sameUrlList.at(0).urlName, sameUrlList.at(0).type, sameUrlList.at(0).urlSize);
                    }
                }
            }
        } else {
            qDebug() << "sameUrlList.size() is greater than 1";
            QString urls;
            for (LinkInfo info : sameUrlList) {
                urls += info.url + "\n";
            }
            showRedownloadMsgbox(urls, false, true);
        }
    }
    qDebug() << "[MainFrame] onParseUrlList function ended";
}

void MainFrame::onDownloadFinish()
{
    qDebug() << "[MainFrame] onDownloadFinish function started";
    //m_UpdateTimer->stop();
    m_ShutdownOk = true;
    if (m_ShutdownAct->isChecked()) {
        qDebug() << "m_ShutdownAct is checked";
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
        qDebug() << "m_SleepAct is checked";
        m_SleepAct->setChecked(false);
        m_DownLoadingTableView->getTableControl()->saveDataBeforeClose();
        m_RecycleTableView->getTableControl()->saveDataBeforeClose();
        //Aria2RPCInterface::instance()->shutdown();
        QProcess p;
        p.start("systemctl hibernate");
        p.waitForFinished();

    } else if (m_QuitProcessAct->isChecked()) {
        qDebug() << "m_QuitProcessAct is checked";
        m_QuitProcessAct->setChecked(false);
        onTrayQuitClick(true);
    }
    qDebug() << "[MainFrame] onDownloadFinish function ended";
}

void MainFrame::onSearchItemClicked(QListWidgetItem *item)
{
    qDebug() << "[MainFrame] onSearchItemClicked function started";
    if (nullptr == sender()) {
        qDebug() << "sender is nullptr";
        return;
    }
    SearchResoultWidget *pWidget = dynamic_cast<SearchResoultWidget *>(sender());
    QString taskId = item->data(Qt::WhatsThisRole).toString();
    QString tab = item->data(Qt::UserRole).toString();
    if (tab.contains("Downloading")) {
        qDebug() << "tab is Downloading";
        onListClicked(m_LeftList->model()->index(0, 0));
        m_LeftList->setCurrentIndex(m_LeftList->model()->index(0, 0));
        DownloadDataItem *pItem = m_DownLoadingTableView->getTableModel()->find(taskId);
        int position = m_DownLoadingTableView->getTableModel()->renderList().indexOf(pItem, 0);
        if (pItem != nullptr) {
            pItem->isChecked = true;
            QModelIndex index = m_DownLoadingTableView->getTableModel()->index(position, 0);
            m_DownLoadingTableView->setCurrentIndex(index);
            m_DownLoadingTableView->scrollTo(index, QAbstractItemView::PositionAtTop);
        }
    } else if (tab.contains("Completed")) {
        qDebug() << "tab is Completed";
        onListClicked(m_LeftList->model()->index(1, 0));
        m_LeftList->setCurrentIndex(m_LeftList->model()->index(1, 0));
        DownloadDataItem *pItem = m_DownLoadingTableView->getTableModel()->find(taskId);
        int position = m_DownLoadingTableView->getTableModel()->renderList().indexOf(pItem, 0);
        if (pItem != nullptr) {
            pItem->isChecked = true;
            QModelIndex index = m_DownLoadingTableView->getTableModel()->index(position, 0);
            m_DownLoadingTableView->setCurrentIndex(index);
            m_DownLoadingTableView->scrollTo(index, QAbstractItemView::PositionAtTop);
        }
    } else if (tab.contains("Trash")) {
        qDebug() << "tab is Trash";
        onListClicked(m_LeftList->model()->index(2, 0));
        m_LeftList->setCurrentIndex(m_LeftList->model()->index(2, 0));
        DeleteDataItem *pItem = m_RecycleTableView->getTableModel()->find(taskId, 2);
        int position = m_RecycleTableView->getTableModel()->recyleList().indexOf(pItem, 0);
        if (pItem != nullptr) {
            pItem->isChecked = true;
            QModelIndex index = m_RecycleTableView->getTableModel()->index(position, 0);
            m_RecycleTableView->setCurrentIndex(index);
            m_RecycleTableView->scrollTo(index, QAbstractItemView::PositionAtTop);
            onCheckChanged(0, 0);
        }
    }
    onCheckChanged(0, 0);
    m_ToolBar->clearSearchText();
    pWidget->hide();
    qDebug() << "[MainFrame] onSearchItemClicked function ended";
}

bool MainFrame::isAutoStart()
{
    qDebug() << "[MainFrame] isAutoStart function started";
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
    qDebug() << "[MainFrame] isAutoStart function ended with result:" << false;
    return false;
}

void MainFrame::setAutoStart(bool ret)
{
    qDebug() << "[MainFrame] setAutoStart function started with ret:" << ret;
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
        writeData << list[i] << QT_ENDL;
    }
    writeData.flush();
    writerFile.close();
    qDebug() << "[MainFrame] setAutoStart function ended";
}

bool MainFrame::deleteDirectory(const QString &path)
{
    qDebug() << "[MainFrame] deleteDirectory function started with path:" << path;
    QFileInfo info(path);
    if (info.isFile()) {
        qDebug() << "path is a file";
        QFile::remove(path);
        return true;
    }
    if (path.isEmpty()) {
        qDebug() << "path is empty";
        return false;
    }

    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "dir is not exists";
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    for (QFileInfo fi : fileList) {
        if (fi.isFile()) {
            // qDebug() << "fi is a file";
            fi.dir().remove(fi.fileName());
        } else {
            // qDebug() << "fi is a directory";
            deleteDirectory(fi.absoluteFilePath());
        }
    }
    qDebug() << "[MainFrame] deleteDirectory function, deleting directory:" << dir.absolutePath();
    return dir.rmpath(dir.absolutePath());
}

void MainFrame::deleteTaskByUrl(QString url)
{
    qDebug() << "[MainFrame] deleteTaskByUrl function started with url:" << url;
    bool isExist = false;
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    for (auto pItem : dataList) {
        if (pItem->url == url) {
            isExist = true;
            deleteTask(pItem);
            break;
        }
    }
    if (!isExist) {
        const QList<DeleteDataItem *> &dataList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto pItem : dataList) {
            if (pItem->url == url) {
                isExist = true;
                deleteTask(pItem);
                break;
            }
        }
    }
    QThread::usleep(100);
    qDebug() << "[MainFrame] deleteTaskByUrl function ended";
}

void MainFrame::deleteTaskByTaskID(QString taskID)
{
    qDebug() << "[MainFrame] deleteTaskByTaskID function started with taskID:" << taskID;
    bool isExist = false;
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    for (auto pItem : dataList) {
        if (pItem->taskId == taskID) {
            isExist = true;
            deleteTask(pItem);
            break;
        }
    }
    if (!isExist) {
        const QList<DeleteDataItem *> &dataList = m_RecycleTableView->getTableModel()->recyleList();
        for (auto pItem : dataList) {
            if (pItem->taskId == taskID) {
                isExist = true;
                deleteTask(pItem);
            }
        }
    }
    QThread::usleep(200);
    qDebug() << "[MainFrame] deleteTaskByTaskID function ended";
}

void MainFrame::deleteTask(DeleteDataItem *pItem)
{
    qDebug() << "[MainFrame] deleteTask function started";
    if (pItem == nullptr) {
        return;
    }
    Aria2RPCInterface::instance()->forcePause(pItem->gid, pItem->taskId);
    Aria2RPCInterface::instance()->forceRemove(pItem->gid, pItem->taskId);
    QString filePath = pItem->savePath;
    if (!pItem->savePath.isEmpty()) {
        qDebug() << "pItem->savePath is not empty";
        if (pItem->url.isEmpty()) { //bt任务
            TaskInfoHash info;
            DBInstance::getBtTaskById(pItem->taskId, info);
            QString torrentPath = info.filePath;
            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
            QString mode = btInfo.mode;
            if (pItem->savePath.contains(btInfo.name)) {
                deleteDirectory(pItem->savePath);
            }
        } else {
            qDebug() << "pItem->url is not empty";
            deleteDirectory(pItem->savePath);
        }
        if (QFile::exists(filePath + ".aria2")) {
            qDebug() << "filePath + .aria2 is exists";
            QFile::remove(filePath + ".aria2");
            QTimer::singleShot(3000, [=]() {
                QString path = Settings::getInstance()->getDownloadSavePath();
                if (!filePath.contains(path)) {
                    qDebug() << "filePath is not contains path";
                    QFile::remove(filePath);
                    QFile::remove(filePath + ".aria2");
                }
            });
        }
    }
    DBInstance::delTask(pItem->taskId);
    m_RecycleTableView->getTableModel()->removeItem(pItem);
    qDebug() << "[MainFrame] deleteTask function ended";
}

void MainFrame::deleteTask(DownloadDataItem *pItem)
{
    qDebug() << "[MainFrame] deleteTask function started";
    Aria2RPCInterface::instance()->forcePause(pItem->gid, pItem->taskId);
    Aria2RPCInterface::instance()->remove(pItem->gid, pItem->taskId);
    QString filePath = pItem->savePath;
    if (!pItem->savePath.isEmpty()) {
        qDebug() << "pItem->savePath is not empty";
        if (pItem->url.isEmpty()) { //bt任务
            TaskInfoHash info;
            DBInstance::getBtTaskById(pItem->taskId, info);
            QString torrentPath = info.filePath;
            Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
            QString mode = btInfo.mode;
            if (pItem->savePath.contains(btInfo.name)) {
                deleteDirectory(pItem->savePath);
            }
        } else {
            qDebug() << "pItem->url is not empty";
            deleteDirectory(pItem->savePath);
        }

        if (QFile::exists(filePath + ".aria2")) {
            qDebug() << "filePath + .aria2 is exists";
            QFile::remove(filePath + ".aria2");
            QTimer::singleShot(3000, [=]() {
                QString path = Settings::getInstance()->getDownloadSavePath();
                if (!filePath.contains(path)) {
                    qDebug() << "filePath is not contains path";
                    QFile::remove(filePath);
                    QFile::remove(filePath + ".aria2");
                }
            });
        }
    }
    DBInstance::delTask(pItem->taskId);
    if (pItem->status == DownloadTaskStatus::Active || pItem->status == DownloadTaskStatus::Waiting) { //正在下载的任务，等返回删除成功在删除任务记录
        return;
    }
    m_DownLoadingTableView->getTableModel()->removeItem(pItem);

    qDebug() << "[MainFrame] deleteTask function ended";
    return;
}

bool MainFrame::checkIsHasSameTask(QString infoHash)
{
    qDebug() << "[MainFrame] checkIsHasSameTask function started with infoHash:" << infoHash;
    QList<TaskInfoHash> taskInfoList;
    DBInstance::getAllBtTask(taskInfoList);
    for (auto &info : taskInfoList) { //删除重复的任务
        DownloadDataItem *pItem = m_DownLoadingTableView->getTableModel()->find(info.taskId);
        DeleteDataItem *pDeleteItem = nullptr;
        if (pItem == nullptr) {
            pDeleteItem = m_RecycleTableView->getTableModel()->find(info.taskId, 0);
        }
        if (info.infoHash.toLower() == infoHash) {
            MessageBox msg(this);
            //msg.setWarings(tr("Task exist, Downloading again will delete the downloaded content!"), tr("View"), tr("Redownload"), 0, QList<QString>());
            msg.setRedownload(info.filePath);
            int ret = msg.exec();
            if (ret != DDialog::Accepted) {
                return false;
            } else {
                DownloadDataItem *pItemData = m_DownLoadingTableView->getTableModel()->find(info.taskId);
                if (pItem != nullptr) {
                    deleteTask(pItem);
                } else if (pDeleteItem != nullptr) {
                    deleteTask(pDeleteItem);
                } else {
                    return false;
                }
            }
            break;
        }
    }
    const QList<DownloadDataItem *> &dataList = m_DownLoadingTableView->getTableModel()->dataList();
    const QList<DeleteDataItem *> &recyleList = m_DownLoadingTableView->getTableModel()->recyleList();
    for (auto item : dataList) {
        QString url = item->url.toUpper();
        if (url.contains(infoHash.toUpper()) && item->status != DownloadTaskStatus::Complete) {
            if (showRedownloadMsgbox(item->url)) {
                deleteTaskByUrl(item->url);
                break;
            } else {
                return false;
            }
        }
    }

    for (auto item : recyleList) {
        QString url = item->url.toUpper();
        if (url.contains(infoHash.toUpper()) && item->status != DownloadTaskStatus::Complete) {
            if (showRedownloadMsgbox(item->url)) {
                deleteTaskByUrl(item->url);
                break;
            } else {
                return false;
            }
        }
    }
#if QT_VERSION_MAJOR > 5
    QElapsedTimer time;
#else
    QTime time;
#endif
    time.start();
    while (time.elapsed() < 500) {
        QCoreApplication::processEvents();
    }

    qDebug() << "[MainFrame] checkIsHasSameTask function ended with result:" << true;
    return true;
}
