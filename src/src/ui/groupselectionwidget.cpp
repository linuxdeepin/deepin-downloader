/**
* @file groupselectionwidget.cpp
* @brief 基于ItemSelectionWidget窗口控件，根据传入的选项名称列表，绘制一组复选框窗口控件，并实现单选功能
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 15:56
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#include "groupselectionwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#include "itemselectionwidget.h"

GroupSelectionWidget::GroupSelectionWidget(QStringList lstItemName, QWidget *parent) : QWidget(parent)
{
    m_lstItemName = lstItemName;
    initUI();
    initConnections();
}

// 初始化界面
void GroupSelectionWidget::initUI()
{
    m_pLabel = new DLabel;
    QVBoxLayout *pMainLayout = new QVBoxLayout;
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->addWidget(m_pLabel);
    for(int i = 0; i < m_lstItemName.count(); i++)
    {
        ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget;
        pItemSelectionWidget->setLabelText(m_lstItemName.at(i));
        pItemSelectionWidget->setObjectName(m_lstItemName.at(i));
        connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, this, &GroupSelectionWidget::itemCheckedSlot);
        if(i == 0)
        {
            pItemSelectionWidget->setCheckBoxChecked(true);
        }

        pMainLayout->addWidget(pItemSelectionWidget);
    }

    setLayout(pMainLayout);
}

// 初始化链接
void GroupSelectionWidget::initConnections()
{

}

void GroupSelectionWidget::itemCheckedSlot(bool bIsChecked)
{
    ItemSelectionWidget *pItemSelectionWidget = qobject_cast<ItemSelectionWidget *>(sender());
    if(!bIsChecked)
    {
        pItemSelectionWidget->setBlockSignals(true);
        pItemSelectionWidget->setCheckboxState(Qt::Checked);
        pItemSelectionWidget->setBlockSignals(false);
    }
    else
    {
        QList<ItemSelectionWidget *> lstItemWidget = findChildren<ItemSelectionWidget *>();
        for(int i = 0; i < lstItemWidget.count(); i++)
        {
            ItemSelectionWidget *pItemWidget = lstItemWidget.at(i);
            if(pItemSelectionWidget->objectName() != pItemWidget->objectName())
            {
                pItemWidget->setBlockSignals(true);
                pItemWidget->setCheckboxState(Qt::Unchecked);
                pItemWidget->setBlockSignals(false);
            }

        }

        QString strText = pItemSelectionWidget->objectName();
        emit selectedChanged(strText);
    }
}

void GroupSelectionWidget::setLabelText(QString strText)
{
    m_pLabel->setText(strText);
}

void GroupSelectionWidget::setLabelIsHide(bool bIsHide)
{
    if(bIsHide)
    {
        m_pLabel->hide();
    }
}

void GroupSelectionWidget::setCurrentSelected(QString strName)
{
    QList<ItemSelectionWidget *> lstItemWidget = findChildren<ItemSelectionWidget *>();
    for(int i = 0; i < lstItemWidget.count(); i++)
    {
        ItemSelectionWidget *pItemWidget = lstItemWidget.at(i);
        if(strName != pItemWidget->objectName())
        {
            pItemWidget->setBlockSignals(true);
            pItemWidget->setCheckboxState(Qt::Unchecked);
            pItemWidget->setBlockSignals(false);
        }
        else
        {
            pItemWidget->setBlockSignals(true);
            pItemWidget->setCheckboxState(Qt::Checked);
            pItemWidget->setBlockSignals(false);
        }
    }
}


