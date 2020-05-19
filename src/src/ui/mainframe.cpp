#include "mainframe.h"

#include <QLayout>
#include <QAction>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QHeaderView>
#include <QCloseEvent>
#include <DMenu>
#include <DLabel>
#include <DTitlebar>
#include "tableView.h"
#include "topButton.h"


MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{
    init();
}

void MainFrame::init()
{
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


    m_pTableView = new TableView(0);
    m_pTableView->verticalHeader()->setDefaultSectionSize(56);
    m_pTableView->setColumnHidden(4, true);
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
    m_pNoTask_label->setText(tr("暂无下载的任务"));
    m_pNoTask_label->setAlignment(Qt::AlignHCenter);

    pNoTask_WidgetLayout->addWidget(m_pNoTask_label);
    m_pNoTask_tip_Label = new QLabel();
    QFont noTask_tip_Label_font;
    noTask_tip_Label_font.setPointSize(13);
    m_pNoTask_tip_Label->setFont(noTask_tip_Label_font);
    QPalette noTask_tip_Label_p;
    noTask_tip_Label_p.setColor(QPalette::WindowText, QColor(65, 77, 104, 70));
    m_pNoTask_tip_Label->setText(tr("可点击添加按钮新建下载任务"));
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
    m_pRightStackwidget->addWidget(m_pTableView);
    //m_pRightStackwidget->addWidget(recycle_tableview);
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

    m_pDownloading_item = new QStandardItem(QIcon::fromTheme("ndm_list_downloading"), tr("下载中"));
    m_pDownloadFinish_item = new QStandardItem(QIcon::fromTheme("ndm_print_done"), tr("下载完成"));
    m_pRecycle_item = new QStandardItem(QIcon::fromTheme("ndm_list_delete"), tr("回收站"));
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
    m_pTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    //recycle_tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    m_pRightStackwidget->setCurrentIndex(0);

    QIcon tryIcon = QIcon(":/icons/deepin/builtin/light/icons/ndm_preferencesystem_24px.svg");
    m_pSystemTray = new QSystemTrayIcon(this);
    m_pSystemTray->setIcon(tryIcon);
    m_pSystemTray->setToolTip(tr("Download Manager"));

    QAction *pQuitAct = new QAction(tr("退出"), this);
    connect(pQuitAct, SIGNAL(triggered()), this, SLOT(on_tray_quit_click()));

    QMenu* pTrayMenu = new QMenu(this);
    pTrayMenu->addAction(pQuitAct);
    connect(m_pSystemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    m_pSystemTray->setContextMenu(pTrayMenu);
    m_pSystemTray->show();

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
void MainFrame::on_tray_quit_click()
{
    //save_data_before_close();
    qApp->quit();
}

MainFrame::~MainFrame()
{

}

//初始化aria2
void MainFrame::initAria2()
{
    //aria2c = new Aria2cInterface(this);
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_success(QString, QJsonObject)), this, SLOT(slotRpcSuccess(QString, QJsonObject)));
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_error(QString, QString, int)), this, SLOT(slotRpcError(QString, QString, int)));
}

void MainFrame::slotRPCSuccess(QString method, QJsonObject json)
{

}

void MainFrame::slotRPCError(QString method, QString id, int error)
{

}


