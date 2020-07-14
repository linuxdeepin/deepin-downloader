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

GroupSelectionWidget::GroupSelectionWidget(QStringList itemNameList, QWidget *parent)
    : QWidget(parent)
{
    m_itemNameList = itemNameList;
    initUI();
    initConnections();
}

// 初始化界面
void GroupSelectionWidget::initUI()
{
    m_label = new DLabel;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_label);

    for (int i = 0; i < m_itemNameList.count(); i++) {
        ItemSelectionWidget *itemSelectionWidget = new ItemSelectionWidget;

        itemSelectionWidget->setLabelText(m_itemNameList.at(i));
        itemSelectionWidget->setObjectName(m_itemNameList.at(i));

        connect(itemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, this, &GroupSelectionWidget::onItemChecked);

        if (i == 0) {
            itemSelectionWidget->setCheckBoxChecked(true);
        }

        mainLayout->addWidget(itemSelectionWidget);
    }

    setLayout(mainLayout);
}

// 初始化链接
void GroupSelectionWidget::initConnections()
{

}

void GroupSelectionWidget::onItemChecked(bool isChecked)
{
    ItemSelectionWidget *itemSelectionWidget = qobject_cast<ItemSelectionWidget *>(sender());

    if (!isChecked) {
        itemSelectionWidget->setBlockSignals(true);
        itemSelectionWidget->setCheckboxState(Qt::Checked);
        itemSelectionWidget->setBlockSignals(false);
    } else {
        QList<ItemSelectionWidget *> itemWidgetList = findChildren<ItemSelectionWidget *>();

        for (int i = 0; i < itemWidgetList.count(); i++) {
            ItemSelectionWidget *itemWidget = itemWidgetList.at(i);

            if (itemSelectionWidget->objectName() != itemWidget->objectName()) {
                itemWidget->setBlockSignals(true);
                itemWidget->setCheckboxState(Qt::Unchecked);
                itemWidget->setBlockSignals(false);
            }
        }

        QString text = itemSelectionWidget->objectName();

        emit selectedChanged(text);
    }
}

void GroupSelectionWidget::setLabelText(const QString &text)
{
    m_label->setText(text);
}

void GroupSelectionWidget::setLabelIsHide(bool isHide)
{
    if (isHide) {
        m_label->hide();
    }
}

void GroupSelectionWidget::setCurrentSelected(const QString &name)
{
    QList<ItemSelectionWidget *> itemWidgetList = findChildren<ItemSelectionWidget *>();

    for (int i = 0; i < itemWidgetList.count(); i++) {
        ItemSelectionWidget *itemWidget = itemWidgetList.at(i);

        if (name != itemWidget->objectName()) {
            itemWidget->setBlockSignals(true);
            itemWidget->setCheckboxState(Qt::Unchecked);
            itemWidget->setBlockSignals(false);
        } else {
            itemWidget->setBlockSignals(true);
            itemWidget->setCheckboxState(Qt::Checked);
            itemWidget->setBlockSignals(false);
        }
    }
}


