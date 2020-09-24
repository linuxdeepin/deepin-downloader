#include "basicsettingwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>
#include <QStandardPaths>

BasicSettingWidget::BasicSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnections();
}

// 初始化界面
void BasicSettingWidget::initUI()
{
    DLabel *pStartLabel = new DLabel("启动");
    pStartLabel->setFont(QFont("Source Han Sans", 14, 75));
    DLabel *pPowerOnLabel = new DLabel("开机启动");
    DLabel *pUndisturbedModeLabel = new DLabel("免打扰模式");
    DLabel *pAutostartUnfinishedTaskLabel = new DLabel("启动后自动开始未完成的任务");
    DLabel *pDownloadLabel = new DLabel("下载目录");
    pDownloadLabel->setFont(QFont("Source Han Sans", 14, 75));
    m_pPowerOnButton = new DSwitchButton;
    m_pUndisturbedModeButton = new DSwitchButton;
    m_pAutostartUnfinishedTaskButton = new DSwitchButton;
    m_pAutoLastPathRadioButton = new DRadioButton("自动修改为上次使用的目录");
    m_pCustomsPathRadioButton = new DRadioButton("设置默认目录");
    m_pFileChooserEdit = new DFileChooserEdit;
    m_pFileChooserEdit->lineEdit()->setReadOnly(true);
    m_pFileChooserEdit->lineEdit()->setClearButtonEnabled(false);
    m_pFileChooserEdit->setFileMode(QFileDialog::FileMode::DirectoryOnly);
    m_pFileChooserEdit->lineEdit()->setFixedHeight(36);
    m_pFileChooserEdit->setText(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads"));
    m_pFileChooserEdit->lineEdit()->setStyleSheet("QLineEdit{background:rgba(230, 230, 230, 1)}"
                                                  "QLineEdit:disabled{background:rgba(230, 230, 230, 100)}");
    m_pFileChooserEdit->setStyleSheet("QPushButton{background-color:rgba(0, 129, 255, 1);width:36px;height:36px;font-size:20px;color:#fff}"
                                      "QPushButton:hover{background-color:rgba(0, 129, 255, 200);width:36px;height:36px;color:#fff}"
                                      "QPushButton:disabled{background-color:rgba(0, 129, 255, 50);width:36px;height:36px;color:#fff}");
    //    m_pFileChooserEdit->setDisabled(true);
    m_pCustomsPathRadioButton->setChecked(true);

    m_pPowerOnButton->setStyleSheet("background:rgba(211, 211, 211, 1)");
    m_pUndisturbedModeButton->setStyleSheet("background:rgba(211, 211, 211, 1)");
    m_pAutostartUnfinishedTaskButton->setStyleSheet("background:rgba(211, 211, 211, 1)");

    QHBoxLayout *pHStartLayout = new QHBoxLayout;
    pHStartLayout->addSpacing(8);
    pHStartLayout->addWidget(pStartLabel);
    pHStartLayout->addStretch();

    QHBoxLayout *pHPowerOnLayout = new QHBoxLayout;
    pHPowerOnLayout->addWidget(pPowerOnLabel);
    pHPowerOnLayout->addStretch();
    pHPowerOnLayout->addWidget(m_pPowerOnButton);

    QHBoxLayout *pHUndisturbedModeLayout = new QHBoxLayout;
    pHUndisturbedModeLayout->addWidget(pUndisturbedModeLabel);
    pHUndisturbedModeLayout->addStretch();
    pHUndisturbedModeLayout->addWidget(m_pUndisturbedModeButton);

    QHBoxLayout *pHAutostartUnfinishedTaskLayout = new QHBoxLayout;
    pHAutostartUnfinishedTaskLayout->addWidget(pAutostartUnfinishedTaskLabel);
    pHAutostartUnfinishedTaskLayout->addStretch();
    pHAutostartUnfinishedTaskLayout->addWidget(m_pAutostartUnfinishedTaskButton);

    QVBoxLayout *pVStartLayout = new QVBoxLayout;
    pVStartLayout->addLayout(pHPowerOnLayout);
    pVStartLayout->addLayout(pHUndisturbedModeLayout);
    pVStartLayout->addLayout(pHAutostartUnfinishedTaskLayout);

    QHBoxLayout *pHDownloadLayout = new QHBoxLayout;
    pHDownloadLayout->addSpacing(8);
    pHDownloadLayout->addWidget(pDownloadLabel);
    pHDownloadLayout->addStretch();

    QHBoxLayout *pFileChooserLayout = new QHBoxLayout;
    pFileChooserLayout->addWidget(m_pFileChooserEdit);
    pFileChooserLayout->setContentsMargins(28, 0, 0, 0);

    QVBoxLayout *pDownloadLayout = new QVBoxLayout;
    pDownloadLayout->addWidget(m_pAutoLastPathRadioButton);
    pDownloadLayout->addSpacing(12);
    pDownloadLayout->addWidget(m_pCustomsPathRadioButton);
    pDownloadLayout->addSpacing(8);
    pDownloadLayout->addLayout(pFileChooserLayout);
    pDownloadLayout->setSpacing(0);
    pDownloadLayout->setContentsMargins(10, 10, 10, 10);

    QWidget *pStartWidget = new QWidget;
    pStartWidget->setStyleSheet("background:rgba(249, 249, 249, 1)");
    pStartWidget->setLayout(pVStartLayout);

    QWidget *pDownloadWidget = new QWidget;
    pDownloadWidget->setStyleSheet("background:rgba(249, 249, 249, 1)");
    pDownloadWidget->setLayout(pDownloadLayout);

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    pMainLayout->addLayout(pHStartLayout);
    pMainLayout->addWidget(pStartWidget);
    pMainLayout->addSpacing(34);
    pMainLayout->addLayout(pHDownloadLayout);
    pMainLayout->addWidget(pDownloadWidget);
    pMainLayout->addStretch();
    pMainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(pMainLayout);
}

// 初始化链接
void BasicSettingWidget::initConnections()
{
    connect(m_pAutoLastPathRadioButton, SIGNAL(clicked()), this, SLOT(radioButtonClickSlot()));
    connect(m_pCustomsPathRadioButton, SIGNAL(clicked()), this, SLOT(radioButtonClickSlot()));
}

void BasicSettingWidget::radioButtonClickSlot()
{
    DRadioButton *pRadioButton = qobject_cast<DRadioButton *>(sender());
    if (m_pAutoLastPathRadioButton == pRadioButton) {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);
    } else if (m_pCustomsPathRadioButton == pRadioButton) {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);
    }
}
