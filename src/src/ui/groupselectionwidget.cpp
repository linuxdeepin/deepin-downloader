/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file groupselectionwidget.cpp
 *
 * @brief 基于ItemSelectionWidget窗口控件，根据传入的选项名称列表，绘制一组复选框窗口控件，并实现单选功能
 *
 *@date 2020-06-09 10:54
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
#include "groupselectionwidget.h"
#include "itemselectionwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

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


