/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file downloadsettingwidget.cpp
 *
 * @brief 下载设置窗口，主要实现全速下载和限速下载的切换，以及限速下载时限制参数的设置
 *
 *@date 2020-06-09 11:03
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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
#include "downloadsettingwidget.h"
#include "settinginfoinputwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

DownloadSettingWidget::DownloadSettingWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnections();
}

// 初始化界面
void DownloadSettingWidget::initUI()
{
    m_pFullSpeedDownloadButton = new DRadioButton(tr("Speed unlimited")); // 全速下载
    DLabel *pFullSpeedLabel = new DLabel;
    // 下载文件会通过P2P的方式上传分享，帮助其他网友加速下载，不会涉及任何用户隐私。
    pFullSpeedLabel->setText(tr("Downloaded files will be uploaded through P2P, \nwhich could help other users speed up the downloading, \ndoes not involve the privacy."));
    pFullSpeedLabel->setWordWrap(true);
    m_pSpeedLimitDownloadButton = new DRadioButton(tr("Speed limited")); // 限速下载
    m_pSpeedLimitDownloadButton->setChecked(true);
    m_pMaxDownloadSpeedLimit = new SettingInfoInputWidget;
    m_pMaxUploadSpeedLimit = new SettingInfoInputWidget;

    m_pMaxDownloadSpeedLimit->setWidgetWidth(290);
    m_pMaxUploadSpeedLimit->setWidgetWidth(290);

    QHBoxLayout *pFullSpeedLabelLayout = new QHBoxLayout;
    pFullSpeedLabelLayout->addWidget(pFullSpeedLabel);
    pFullSpeedLabelLayout->addStretch();
    pFullSpeedLabelLayout->setContentsMargins(28, 0, 0, 0);

    QVBoxLayout *pVFullSpeedLayout = new QVBoxLayout;
    pVFullSpeedLayout->addWidget(m_pFullSpeedDownloadButton);
    pVFullSpeedLayout->addSpacing(3);
    pVFullSpeedLayout->addLayout(pFullSpeedLabelLayout);
    pVFullSpeedLayout->setSpacing(0);
    pVFullSpeedLayout->setContentsMargins(0, 0, 0, 0);

    m_pMaxDownloadSpeedLimit->setTitleLabelText(tr("Max download speed")); // 最大下载限速
    m_pMaxDownloadSpeedLimit->setUnitLabelText(tr("KB/s"));
    m_pMaxDownloadSpeedLimit->setRangeLabelText(tr("(100-102400)"));
    m_pMaxDownloadSpeedLimit->setLineEditText(tr("102400"));
    m_pMaxDownloadSpeedLimit->setValid(100, 102400);

    m_pMaxUploadSpeedLimit->setTitleLabelText(tr("Max upload speed")); // 最大上传限速
    m_pMaxUploadSpeedLimit->setUnitLabelText(tr("KB/s"));
    m_pMaxUploadSpeedLimit->setRangeLabelText(tr("(16-5120)"));
    m_pMaxUploadSpeedLimit->setLineEditText(tr("32"));
    m_pMaxUploadSpeedLimit->setValid(16,5120);

    DLabel *pLeftLabel = new DLabel(tr("Limited period")); // 限速时段
    DLabel *pCenterLabel = new DLabel(tr("to")); // 至
    m_pStartTimeEdit = new QTimeEdit(QTime(8, 0, 0));
    m_pEndTimeEdit = new QTimeEdit(QTime(17, 0, 0));
    m_pStartTimeEdit->setDisplayFormat("h:mm");
    m_pEndTimeEdit->setDisplayFormat("h:mm");

    QHBoxLayout *pTimeLayout = new QHBoxLayout;
    pTimeLayout->addWidget(pLeftLabel);
//    pTimeLayout->addSpacing(6);
    pTimeLayout->addWidget(m_pStartTimeEdit);
//    pTimeLayout->addSpacing(1);
    pTimeLayout->addWidget(pCenterLabel);
//    pTimeLayout->addSpacing(1);
    pTimeLayout->addWidget(m_pEndTimeEdit);
//    pTimeLayout->setSpacing(0);
    pTimeLayout->addStretch();
    pTimeLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *pSpeedLimitDownloadLayout = new QVBoxLayout;
    pSpeedLimitDownloadLayout->addWidget(m_pMaxDownloadSpeedLimit);
    pSpeedLimitDownloadLayout->addWidget(m_pMaxUploadSpeedLimit);
    pSpeedLimitDownloadLayout->addLayout(pTimeLayout);
    pSpeedLimitDownloadLayout->setContentsMargins(28, 0, 0, 0);

    QVBoxLayout *pMainLayout = new QVBoxLayout;
//    pMainLayout->addWidget(m_pFullSpeedDownloadButton);
    pMainLayout->addLayout(pVFullSpeedLayout);
    pMainLayout->addWidget(m_pSpeedLimitDownloadButton);
    pMainLayout->addLayout(pSpeedLimitDownloadLayout);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pMainLayout);
}

// 初始化链接
void DownloadSettingWidget::initConnections()
{
    connect(m_pFullSpeedDownloadButton,SIGNAL(clicked()),this,SLOT(radioButtonClickSlot()));
    connect(m_pSpeedLimitDownloadButton,SIGNAL(clicked()),this,SLOT(radioButtonClickSlot()));
    connect(m_pMaxDownloadSpeedLimit,&SettingInfoInputWidget::textChanged,this,&DownloadSettingWidget::textChangedSlot);
    connect(m_pMaxUploadSpeedLimit,&SettingInfoInputWidget::textChanged,this,&DownloadSettingWidget::textChangedSlot);
    connect(m_pStartTimeEdit,SIGNAL(timeChanged(const QTime &)),this,SLOT(timeChangedSlot(const QTime &)));
    connect(m_pEndTimeEdit,SIGNAL(timeChanged(const QTime &)),this,SLOT(timeChangedSlot(const QTime &)));
}

void DownloadSettingWidget::radioButtonClickSlot()
{
    DRadioButton *pRadioButton = qobject_cast<DRadioButton *>(sender());
    if(m_pFullSpeedDownloadButton == pRadioButton)
    {
        m_pFullSpeedDownloadButton->setChecked(true);
        m_pSpeedLimitDownloadButton->setChecked(false);
        m_pMaxDownloadSpeedLimit->setLineEditIsDisabled(true);
        m_pMaxUploadSpeedLimit->setLineEditIsDisabled(true);
        m_pStartTimeEdit->setDisabled(true);
        m_pEndTimeEdit->setDisabled(true);

        QString strText = QString("fullspeed;%1;%2;%3;%4")
                .arg(m_pMaxDownloadSpeedLimit->getLineEditText())
                .arg(m_pMaxUploadSpeedLimit->getLineEditText())
                .arg(m_pStartTimeEdit->time().toString("hh:mm:ss"))
                .arg(m_pEndTimeEdit->time().toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strText);
    }
    else if(m_pSpeedLimitDownloadButton == pRadioButton)
    {
        m_pFullSpeedDownloadButton->setChecked(false);
        m_pSpeedLimitDownloadButton->setChecked(true);
        m_pMaxDownloadSpeedLimit->setLineEditIsDisabled(false);
        m_pMaxUploadSpeedLimit->setLineEditIsDisabled(false);
        m_pStartTimeEdit->setDisabled(false);
        m_pEndTimeEdit->setDisabled(false);

        QString strText = QString("speedlimit;%1;%2;%3;%4")
                .arg(m_pMaxDownloadSpeedLimit->getLineEditText())
                .arg(m_pMaxUploadSpeedLimit->getLineEditText())
                .arg(m_pStartTimeEdit->time().toString("hh:mm:ss"))
                .arg(m_pEndTimeEdit->time().toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strText);
    }
}

void DownloadSettingWidget::timeChangedSlot(const QTime &time)
{
    QTimeEdit *pTimeEdit = qobject_cast<QTimeEdit *>(sender());
    if(m_pStartTimeEdit == pTimeEdit)
    {
        QString strText = QString("speedlimit;%1;%2;%3;%4")
                .arg(m_pMaxDownloadSpeedLimit->getLineEditText())
                .arg(m_pMaxUploadSpeedLimit->getLineEditText())
                .arg(time.toString("hh:mm:ss"))
                .arg(m_pEndTimeEdit->time().toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strText);
    }
    else if(m_pEndTimeEdit == pTimeEdit)
    {
        QString strText = QString("speedlimit;%1;%2;%3;%4")
                .arg(m_pMaxDownloadSpeedLimit->getLineEditText())
                .arg(m_pMaxUploadSpeedLimit->getLineEditText())
                .arg(m_pStartTimeEdit->time().toString("hh:mm:ss"))
                .arg(time.toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strText);
    }
}

void DownloadSettingWidget::textChangedSlot(QString strText)
{
    SettingInfoInputWidget *pSettingInfoInputWidget = qobject_cast<SettingInfoInputWidget *>(sender());
    if(m_pMaxDownloadSpeedLimit == pSettingInfoInputWidget)
    {
        QString strInfo = QString("speedlimit;%1;%2;%3;%4")
                .arg(strText)
                .arg(m_pMaxUploadSpeedLimit->getLineEditText())
                .arg(m_pStartTimeEdit->time().toString("hh:mm:ss"))
                .arg(m_pEndTimeEdit->time().toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strInfo);
    }
    else if(m_pMaxUploadSpeedLimit == pSettingInfoInputWidget)
    {
        QString strInfo = QString("speedlimit;%1;%2;%3;%4")
                .arg(m_pMaxDownloadSpeedLimit->getLineEditText())
                .arg(strText)
                .arg(m_pStartTimeEdit->time().toString("hh:mm:ss"))
                .arg(m_pEndTimeEdit->time().toString("hh:mm:ss"));

        emit speedLimitInfoChanged(strInfo);
    }
}

void DownloadSettingWidget::setCurrentSelectRadioButton(int nCurrentSelect)
{
    if(1 == nCurrentSelect)
    {
        m_pFullSpeedDownloadButton->setChecked(true);
        m_pSpeedLimitDownloadButton->setChecked(false);
        m_pMaxDownloadSpeedLimit->setLineEditIsDisabled(true);
        m_pMaxUploadSpeedLimit->setLineEditIsDisabled(true);
        m_pStartTimeEdit->setDisabled(true);
        m_pEndTimeEdit->setDisabled(true);
    }
    else
    {
        m_pFullSpeedDownloadButton->setChecked(false);
        m_pSpeedLimitDownloadButton->setChecked(true);
        m_pMaxDownloadSpeedLimit->setLineEditIsDisabled(false);
        m_pMaxUploadSpeedLimit->setLineEditIsDisabled(false);
        m_pStartTimeEdit->setDisabled(false);
        m_pEndTimeEdit->setDisabled(false);
    }
}

void DownloadSettingWidget::setMaxDownloadSpeedLimit(QString strText)
{
    m_pMaxDownloadSpeedLimit->setLineEditText(strText);
}

void DownloadSettingWidget::setMaxUploadSpeedLimit(QString strText)
{
    m_pMaxUploadSpeedLimit->setLineEditText(strText);
}

void DownloadSettingWidget::setStartTime(QString strText)
{
    QTime time = QTime::fromString(strText, "hh:mm:ss");
    m_pStartTimeEdit->setTime(time);
}

void DownloadSettingWidget::setEndTime(QString strText)
{
    QTime time = QTime::fromString(strText, "hh:mm:ss");
    m_pEndTimeEdit->setTime(time);
}



