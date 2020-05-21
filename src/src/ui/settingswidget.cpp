#include "settingswidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <DTitlebar>
#include <DSearchEdit>
#include <DLabel>
#include <DIconButton>
#include <DListView>
#include <DApplication>
#include <QDebug>
#include <QBitmap>
#include <QPainter>
#include <QListWidget>

SettingsWidget *SettingsWidget::s_pInstance = NULL;
SettingsWidget *SettingsWidget::getInstance()
{
    if(s_pInstance == NULL)
    {
        s_pInstance = new SettingsWidget;
    }

    return s_pInstance;
}

SettingsWidget::SettingsWidget(QWidget *parent) : DMainWindow(parent)
{
    initUI();
    initConnections();
}

// 初始化界面
void SettingsWidget::initUI()
{
    setMinimumSize(678, 532);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint); // 隐藏最大化按钮
    titlebar()->setMenuVisible(false); // 隐藏操作按钮
    titlebar()->setFixedHeight(50);

    QWidget *pTitleWidget = new QWidget;

    DLabel *pTitleLabel = new DLabel;
    QIcon logo_icon=QIcon(":/icons/images/icon/downloader3.svg");
    pTitleLabel->setPixmap(logo_icon.pixmap(32,32));
    pTitleLabel->setFixedSize(36,36);

    QHBoxLayout *pTitleLayout = new QHBoxLayout;
    pTitleLayout->addWidget( pTitleLabel );
    pTitleLayout->addStretch();
    pTitleLayout->setContentsMargins( 0, 0, 0, 0 );
    pTitleWidget->setLayout( pTitleLayout );

    titlebar()->setCustomWidget( pTitleWidget );

    QPalette palette;
    palette.setColor(QPalette::Background,QColor(255,255,255));

    m_pLeftWidget = new QWidget;
    m_pLeftWidget->setAutoFillBackground(true);
    m_pLeftWidget->setPalette(palette);
    m_pLeftWidget->setFixedWidth(128);
    QVBoxLayout *pLeftLayout = new QVBoxLayout(m_pLeftWidget);
    pLeftLayout->setContentsMargins(5,0,5,0);

    m_pBasicSettings= new QStandardItem(QIcon::fromTheme(":/btn/bases_light.svg"),tr(" 基本设置"));
    m_pMonitoringSettings= new QStandardItem(QIcon::fromTheme(":/btn/take_over_light.svg"),tr(" 接管设置"));
    m_pDownloadsSettings= new QStandardItem(QIcon::fromTheme(":/btn/download_normal_light.svg"),tr(" 下载设置"));
    m_pNotifications= new QStandardItem(QIcon::fromTheme(":/btn/notification_normal_light.svg"),tr(" 通知提醒"));
    m_pAdvancedSettings= new QStandardItem(QIcon::fromTheme(":/btn/advance_normal_light.svg"),tr(" 高级设置"));
    // 设置Item背景颜色
    m_pBasicSettings->setBackground(QColor(255,255,255));
    m_pMonitoringSettings->setBackground(QColor(255,255,255));
    m_pDownloadsSettings->setBackground(QColor(255,255,255));
    m_pNotifications->setBackground(QColor(255,255,255));
    m_pAdvancedSettings->setBackground(QColor(255,255,255));
    // 设置Item不可编辑
    m_pBasicSettings->setEditable(false);
    m_pMonitoringSettings->setEditable(false);
    m_pDownloadsSettings->setEditable(false);
    m_pNotifications->setEditable(false);
    m_pAdvancedSettings->setEditable(false);

    // 将Item添加进模型
    m_pStandardItemModel= new QStandardItemModel(this);
    m_pStandardItemModel->appendRow(m_pBasicSettings);
    m_pStandardItemModel->appendRow(m_pMonitoringSettings);
    m_pStandardItemModel->appendRow(m_pDownloadsSettings);
    m_pStandardItemModel->appendRow(m_pNotifications);
    m_pStandardItemModel->appendRow(m_pAdvancedSettings);

    // 设置模型
    m_pListView = new DListView;
    m_pListView->setItemSpacing(0);
    m_pListView->setItemSize(QSize(112,40));
    m_pListView->setItemMargins(QMargins(10,2,5,2));
    m_pListView->setIconSize(QSize(14,14));
    QFont font;
    font.setFamily("Source Han Sans");
    font.setPointSize(14);
    m_pListView->setFont(font);
    m_pListView->setModel(m_pStandardItemModel);
    pLeftLayout->addWidget(m_pListView);

    m_pBasicSettingWidget = new BasicSettingWidget;
    m_pMonitoringSettingWidget = new MonitoringSettingWidget;

    m_pStackedWidget = new QStackedWidget;
    m_pStackedWidget->addWidget(m_pBasicSettingWidget);
    m_pStackedWidget->addWidget(m_pMonitoringSettingWidget);
    m_pStackedWidget->setCurrentIndex(0);
//    m_pStackedWidget->setStyleSheet("background:rgb(255,0,0);");

    m_pRightWidget = new QWidget;
    m_pRightWidget->setAutoFillBackground(true);
    m_pRightWidget->setFixedWidth(530);
    m_pRightWidget->setStyleSheet("background:rgb(255,255,255);border-radius:10px");
    QVBoxLayout *pRightLayout = new QVBoxLayout(m_pRightWidget);
    pRightLayout->addWidget(m_pStackedWidget);
    pRightLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *pVLayout = new QVBoxLayout;
    pVLayout->addWidget(m_pRightWidget);
    pVLayout->setSpacing(0);
    pVLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *pMainHLayout = new QHBoxLayout;
    pMainHLayout->addWidget(m_pLeftWidget);
    pMainHLayout->addLayout(pVLayout);
    pMainHLayout->setContentsMargins(0,0,0,0);
    pMainHLayout->setSpacing(0);

    QFrame *pMainWidget = new QFrame;
    pMainWidget->setFrameShape(QFrame::NoFrame);
    pMainWidget->setLayout(pMainHLayout);
    this->setCentralWidget(pMainWidget);
}

// 初始化链接
void SettingsWidget::initConnections()
{
    connect(m_pListView, SIGNAL(clicked(const QModelIndex)),this,SLOT(listViewItemClicked(const QModelIndex)));
}

void SettingsWidget::listViewItemClicked(const QModelIndex &index)
{
    qDebug() << index.row();
    int nCurrentListviewRow = index.row();
    m_pStackedWidget->setCurrentIndex(nCurrentListviewRow);
}

