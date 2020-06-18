/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file settinginfoinputwidget.cpp
 *
 * @brief 设置信息可编辑输入窗口控件，最大下载限速以及最大上传限速用到此控件
 *
 *@date 2020-06-09 10:45
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
#include "settinginfoinputwidget.h"

#include <QHBoxLayout>

SettingInfoInputWidget::SettingInfoInputWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnections();
}

void SettingInfoInputWidget::initUI()
{
    m_pLineEdit = new DLineEdit;
    m_pTitleLabel = new DLabel;
    m_pUnitLabel = new DLabel;
    m_pRangeLabel = new DLabel;

    QHBoxLayout *pHLayout = new QHBoxLayout;
    pHLayout->addWidget(m_pTitleLabel);
    pHLayout->addWidget(m_pLineEdit);
    pHLayout->setContentsMargins(0, 0, 0, 0);

    m_pWidget = new QWidget;
    m_pWidget->setLayout(pHLayout);

    QHBoxLayout *pMainLayout = new QHBoxLayout;
//    pMainLayout->addWidget(m_pTitleLabel);
//    pMainLayout->addWidget(m_pLineEdit);
    pMainLayout->addWidget(m_pWidget);
    pMainLayout->addWidget(m_pUnitLabel);
    pMainLayout->addWidget(m_pRangeLabel);
    pMainLayout->addStretch();
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pMainLayout);
}

void SettingInfoInputWidget::initConnections()
{
    connect(m_pLineEdit, &DLineEdit::textChanged, this, &SettingInfoInputWidget::lineEditTextChangedSlot);
}

void SettingInfoInputWidget::lineEditTextChangedSlot(const QString &strText)
{
    emit textChanged(strText);
}

void SettingInfoInputWidget::setTitleLabelText(const QString &strText)
{
    m_pTitleLabel->setText(strText);
}

void SettingInfoInputWidget::setUnitLabelText(const QString &strText)
{
    m_pUnitLabel->setText(strText);
}

void SettingInfoInputWidget::setRangeLabelText(const QString &strText)
{
    m_pRangeLabel->setText(strText);
}

void SettingInfoInputWidget::setLineEditText(const QString &strText)
{
    m_pLineEdit->setText(strText);
}

void SettingInfoInputWidget::setLineEditIsDisabled(bool bIsDisabled)
{
    m_pLineEdit->setDisabled(bIsDisabled);
}

void SettingInfoInputWidget::setValid(int nMinValid ,int nMaxValid)
{
    QIntValidator *validator = new QIntValidator(nMinValid, nMaxValid, this);
    m_pLineEdit->lineEdit()->setValidator(validator);
}

QString SettingInfoInputWidget::getLineEditText()
{
    QString strText = m_pLineEdit->text();

    return strText;
}

void SettingInfoInputWidget::setWidgetWidth(int nWidth)
{
    m_pWidget->setFixedWidth(nWidth);
}

