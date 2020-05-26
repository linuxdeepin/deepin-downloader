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
#include "log.h"

MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{
    init();
    initAria2();
    initConnection();
}

void MainFrame::init()
{

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
    m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
    m_pDownLoadingTableView->setColumnHidden(4, true);
    //connect(m_pDownLoadingTableView, &TableView::header_stateChanged, this, &MainFrame::get_header_stateChanged);
    //connect(this, &MainFrame::switch_table_signal, m_pDownLoadingTableView, &TableView::clear_header_check);
    m_pRecycleTableView = new TableView(recycle);
    m_pRecycleTableView->verticalHeader()->setDefaultSectionSize(30);
   // connect(m_pRecycleTableView, &TableView::header_stateChanged,  this, &MainFrame::get_header_stateChanged);
   // connect(this, &MainFrame::switch_table_signal, m_pRecycleTableView, &TableView::clear_header_check);

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
        QAction *pShutdownAct = new QAction(tr("关机"), this);
        QAction *pSleepAct = new QAction(tr("休眠"), this);
        QAction *pQuiteAct = new QAction(tr("退出下载器"), this);
        QMenu *pAfterDownloadMenu = new QMenu(tr("下载完成后"), this);
        pAfterDownloadMenu->addAction(pShutdownAct);
        pAfterDownloadMenu->addAction(pSleepAct);
        pAfterDownloadMenu->addAction(pQuiteAct);

        QAction *pShowAct = new QAction(tr("显示"), this);
        QAction *pShowOnDownloadAct = new QAction(tr("下载时显示"), this);
        QAction *pHideAct = new QAction(tr("隐藏"), this);
        QMenu *pTraySetMenu = new QMenu(tr("托盘设置"), this);
        pTraySetMenu->addAction(pShowAct);
        pTraySetMenu->addAction(pShowOnDownloadAct);
        pTraySetMenu->addAction(pHideAct);

        QAction *pQuitAct = new QAction(tr("退出"), this);


        QMenu* pTrayMenu = new QMenu(this);
        pTrayMenu->addAction(pShowMainAct);
        pTrayMenu->addAction(pNewDownloadAct);
        pTrayMenu->addAction(pStartAllAct);
        pTrayMenu->addAction(pPauseAllAct);
        pTrayMenu->addMenu(pAfterDownloadMenu);
        pTrayMenu->addMenu(pTraySetMenu);
        pTrayMenu->addAction(pQuitAct);

        //连接信号与槽
        connect(pShowMainAct, &QAction::triggered, [=](){this->show();});
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

void MainFrame::onNewBtnClicked()
{
    createNewTask("");
}

void MainFrame::onSettingsMenuClicked()
{
    SettingsWidget *pSettingWidget = new  SettingsWidget;
    QDesktopWidget *pDeskWdg = QApplication::desktop();
    QRect rctAvaild = pDeskWdg->availableGeometry();
    pSettingWidget->move((rctAvaild.width() - pSettingWidget->width()) / 2, (rctAvaild.height() - pSettingWidget->height()) / 2);
    pSettingWidget->show();
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
    if((index.row() == 0) || (index.row() == 1)) {
        m_pRightStackwidget->setCurrentIndex(0);
        m_pDownLoadingTableView->horizontalHeader()->reset();
        bool switched = true;
        m_pDownLoadingTableView->reset(switched);
        if(index.row() == 1) {
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(30);
            m_pNoTask_Widget->show();
            m_pNoTask_label->setText(tr("current no download finish task"));
            m_pNoTask_tip_Label->hide();
        } else {
            m_pDownLoadingTableView->verticalHeader()->setDefaultSectionSize(56);
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


