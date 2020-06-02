/**
* @file itemselectionwidget.cpp
* @brief 自定义复选框窗口控件，主要实现标题名称居左，复选框居右，且复选框为圆形的控件窗口
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 14:28
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#include "itemselectionwidget.h"

#include <QHBoxLayout>
#include <QDebug>

ItemSelectionWidget::ItemSelectionWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnections();
}

// 初始化界面
void ItemSelectionWidget::initUI()
{
    //    setStyleSheet("background:rgba(211, 211, 211, 1)");

    m_pLabel = new DLabel("HTTP下载");
    m_pCheckBox = new DCheckBox;
    m_pCheckBox->setStyleSheet("QCheckBox::indicator:unchecked{image:url(:/icons/unchecked);width:24px;height:24px;}"
                               "QCheckBox::indicator:checked{image:url(:/icons/checked);width:24px;height:24px;}");

    QHBoxLayout *pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(m_pLabel);
    pMainLayout->addStretch();
    pMainLayout->addWidget(m_pCheckBox);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pMainLayout);
}

// 初始化链接
void ItemSelectionWidget::initConnections()
{
    connect(m_pCheckBox, &QCheckBox::stateChanged, this, &ItemSelectionWidget::checkBoxstateChangedSlot);
}

void ItemSelectionWidget::checkBoxstateChangedSlot(int nState)
{
    if (nState == 0) {
        emit checkBoxIsChecked(false);
    } else if (nState == 2) {
        emit checkBoxIsChecked(true);
    }
}

void ItemSelectionWidget::setLabelText(const QString &strText)
{
    m_pLabel->setText(strText);
}

void ItemSelectionWidget::setCheckBoxChecked(bool bIsChecked)
{
    m_pCheckBox->setChecked(bIsChecked);
}

void ItemSelectionWidget::setCheckboxState(Qt::CheckState State)
{
    m_pCheckBox->setCheckState(State);
}

void ItemSelectionWidget::setBlockSignals(bool block)
{
    m_pCheckBox->blockSignals(block);
}
