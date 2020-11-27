/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file settingswidget.cpp
 *
 * @brief 设置界面小控件
 *
 * @date 2020-08-18 10:00
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
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

#include "settingswidget.h"

#include <QHBoxLayout>
#include <DLabel>
#include <DSwitchButton>
#include <DLineEdit>
#include <DAlertControl>

#include "settings.h"
DWIDGET_USE_NAMESPACE
SettingsControlWidget::SettingsControlWidget(QWidget *parent)
    : QWidget(parent)
{
}

// 初始化界面
bool SettingsControlWidget::initUI(QString label, QString text, bool isLineEdit)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    DLabel *pLabel = new DLabel(label);
    DLabel *pTextLabel = new DLabel(text);

    m_SwitchBtn = new DSwitchButton();
    layout->addWidget(pLabel);
    if (isLineEdit) {
        m_Edit = new DLineEdit();
        m_Edit->setEnabled(false);
        m_Edit->setMinimumWidth(200);
        DAlertControl *alertControl = new DAlertControl(m_Edit, m_Edit);
        QIntValidator *validator = new QIntValidator(1, 9999);
        m_Edit->lineEdit()->setValidator(validator);
        m_Edit->lineEdit()->setText("100");
        layout->addWidget(m_Edit);
        connect(m_Edit, &DLineEdit::textChanged, this, &SettingsControlWidget::TextChanged);
        connect(m_Edit, &DLineEdit::textChanged, this, [=](const QString &text) { //设置速度不能高于最大限速
            if (Settings::getInstance()->getDownloadSettingSelected()
                && text.toInt() > Settings::getInstance()->getMaxDownloadSpeedLimit().toLong()
                && text.toInt() <= 0) {
                alertControl->showAlertMessage(tr("Total speed neet less than Max download speed!"),
                                               m_Edit->parentWidget()->parentWidget(), -1);
                alertControl->setMessageAlignment(Qt::AlignLeft);
            } else {
                alertControl->hideAlertMessage();
            }
            if (text.contains('+')) {
                QString str = text;
                int pos = m_Edit->lineEdit()->cursorPosition();
                m_Edit->lineEdit()->setText(str.remove('+'));
                m_Edit->lineEdit()->setCursorPosition(pos);
            }
            if (text.size() > 0 && text.at(0) == '0') {
                QString str = text;
                int pos = m_Edit->lineEdit()->cursorPosition();
                m_Edit->lineEdit()->setText(str.remove(0, 1));
                m_Edit->lineEdit()->setCursorPosition(pos);
            }
        });

        connect(m_Edit, &DLineEdit::editingFinished, this, [=]() {
            if (m_Edit->lineEdit()->text().toInt() <= 0) {
                m_Edit->lineEdit()->setText("100");
            }
        });

        connect(m_Edit, &DLineEdit::focusChanged, this, [=](bool onFocus) { //设置速度不能高于最大限速
            if (!onFocus) {
                alertControl->hideAlertMessage();
            }
            if (m_Edit->lineEdit()->text().toInt() <= 0) {
                m_Edit->lineEdit()->setText("100");
            }
        });
    } else {
        m_ComboBox = new DComboBox();
        m_ComboBox->setEnabled(false);
        m_ComboBox->setFixedWidth(80);
        QStringList strList;
        strList << "5"
                << "10"
                << "20"
                << "30"
                << "50"
                << "100";
        m_ComboBox->addItems(strList);
        layout->addWidget(m_ComboBox);
        connect(m_ComboBox, &DComboBox::currentTextChanged, this, &SettingsControlWidget::TextChanged);
    }

    layout->addWidget(pTextLabel);
    layout->addStretch();
    layout->addWidget(m_SwitchBtn, 0, Qt::AlignRight);
    connect(m_SwitchBtn, &DSwitchButton::checkedChanged, this, [=](bool stat) {
        emit checkedChanged(stat);
        if (isLineEdit) {
            m_Edit->setEnabled(stat);
        } else {
            m_ComboBox->setEnabled(stat);
        }
    });
    return true;
}

void SettingsControlWidget::setSpeend(QString speed)
{
    m_Edit->setText(speed);
}

void SettingsControlWidget::setSize(QString size)
{
    if ("5" == size) {
        m_ComboBox->setCurrentIndex(0);
    } else if ("10" == size) {
        m_ComboBox->setCurrentIndex(1);
    } else if ("20" == size) {
        m_ComboBox->setCurrentIndex(2);
    } else if ("30" == size) {
        m_ComboBox->setCurrentIndex(3);
    } else if ("50" == size) {
        m_ComboBox->setCurrentIndex(4);
    } else if ("100" == size) {
        m_ComboBox->setCurrentIndex(5);
    }
}

void SettingsControlWidget::setSwitch(bool arg)
{
    m_SwitchBtn->setChecked(arg);
}
