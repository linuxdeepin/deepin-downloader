/**
* @file settinginfoinputwidget.cpp
* @brief 设置信息可编辑输入窗口控件，最大下载限速以及最大上传限速用到此控件
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 14:07
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
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

    QHBoxLayout *pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(m_pTitleLabel);
    pMainLayout->addWidget(m_pLineEdit);
    pMainLayout->addWidget(m_pUnitLabel);
    pMainLayout->addWidget(m_pRangeLabel);
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

