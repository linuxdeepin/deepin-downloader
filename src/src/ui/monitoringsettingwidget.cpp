#include "monitoringsettingwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>

MonitoringSettingWidget::MonitoringSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnections();
}

// 初始化界面
void MonitoringSettingWidget::initUI()
{
    DLabel *pMonitoringObjectLabel = new DLabel("接管对象");
    pMonitoringObjectLabel->setFont(QFont("Source Han Sans", 14, 75));
    DLabel *pClipBoardLabel = new DLabel("剪切板");
    m_pClipBoardButton = new DSwitchButton;
    m_pClipBoardButton->setStyleSheet("background:rgba(211, 211, 211, 1)");

    QHBoxLayout *pHMonitoringObjectLayout = new QHBoxLayout;
    pHMonitoringObjectLayout->addSpacing(8);
    pHMonitoringObjectLayout->addWidget(pMonitoringObjectLabel);
    pHMonitoringObjectLayout->addStretch();

    QHBoxLayout *pHClipBoardLayout = new QHBoxLayout;
    pHClipBoardLayout->addWidget(pClipBoardLabel);
    pHClipBoardLayout->addStretch();
    pHClipBoardLayout->addWidget(m_pClipBoardButton);

    QVBoxLayout *pMonitoringObjectLayout = new QVBoxLayout;
    pMonitoringObjectLayout->addLayout(pHClipBoardLayout);

    QWidget *pMonitoringObjectWidget = new QWidget;
    pMonitoringObjectWidget->setStyleSheet("background:rgba(249, 249, 249, 1)");
    pMonitoringObjectWidget->setLayout(pMonitoringObjectLayout);

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    pMainLayout->addLayout(pHMonitoringObjectLayout);
    pMainLayout->addWidget(pMonitoringObjectWidget);
    pMainLayout->addSpacing(34);
    pMainLayout->addStretch();
    pMainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(pMainLayout);
}

// 初始化链接
void MonitoringSettingWidget::initConnections()
{
}
