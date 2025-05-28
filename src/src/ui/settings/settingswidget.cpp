// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <QRadioButton>
#include <QPainter>
#include <QPaintEvent>
#include <DLabel>
#include <DSwitchButton>
#include <DLineEdit>
#include <DAlertControl>
#include <DBackgroundGroup>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
#include <DHorizontalLine>
#include <QTimer>
#include "settings.h"

SettingsControlWidget::SettingsControlWidget(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "SettingsControlWidget created";
}

// 初始化界面
bool SettingsControlWidget::initUI(QString label, QString text, bool isLineEdit)
{
    qDebug() << "Initializing settings control widget with label:" << label << "text:" << text << "isLineEdit:" << isLineEdit;
    QFont font;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    DLabel *pLabel = new DLabel(label);
    pLabel->setFont(font);
    DLabel *pTextLabel = new DLabel(text);
    pTextLabel->setFont(font);
    pTextLabel->setWordWrap(true);

    m_SwitchBtn = new DSwitchButton();
    layout->addWidget(pLabel);
    if (isLineEdit) {
        m_Edit = new DLineEdit();
        //m_Edit->setAccessibleName(text.remove(QRegExp("\\s")));
        m_Edit->setEnabled(false);
        QSharedPointer<QIntValidator> validator =
                    QSharedPointer<QIntValidator>(new QIntValidator(1, 9999), &QObject::deleteLater);
        m_Edit->lineEdit()->setValidator(validator.data());
        m_Edit->lineEdit()->setText("100");
        m_Edit->setAccessibleName(m_Edit->text());
        layout->addWidget(m_Edit);
        connect(m_Edit, &DLineEdit::textChanged, this, [=](const QString & text){
            emit TextChanged(text);
            m_Edit->setAccessibleName(text);
        });
    } else {
        m_ComboBox = new DComboBox();
        //m_ComboBox->setAccessibleName(text.remove(QRegExp("\\s")));
        m_ComboBox->setEnabled(false);
        QStringList strList;
        strList << "5"
                << "10"
                << "20"
                << "30"
                << "50"
                << "100";
        m_ComboBox->addItems(strList);
        layout->addWidget(m_ComboBox);
        connect(m_ComboBox, &DComboBox::currentTextChanged, this,  [=](const QString & text){
            emit TextChanged(text);
            m_ComboBox->setAccessibleName(text);
        });
    }

    layout->addWidget(pTextLabel);
    layout->addStretch();
    layout->addWidget(m_SwitchBtn, 0, Qt::AlignRight);
    connect(m_SwitchBtn, &DSwitchButton::checkedChanged, this, [=](bool stat) {
    qDebug() << "[SettingsControlWidget] initUI function ended with result: true";
        emit checkedChanged(stat);
        if (isLineEdit) {
            m_Edit->setEnabled(stat);
        } else {
            m_ComboBox->setEnabled(stat);
        }
    });
    qDebug() << "[SettingsControlWidget] initUI function ended with result: true";
    return true;
}

void SettingsControlWidget::setSpeend(QString speed)
{
    // qDebug() << "[SettingsControlWidget] setSpeend function started with speed:" << speed;
    m_Edit->setText(speed);
}

void SettingsControlWidget::setSize(QString size)
{
    // qDebug() << "[SettingsControlWidget] setSize function started with size:" << size;
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
    // qDebug() << "[SettingsControlWidget] setSize function ended";
}

void SettingsControlWidget::setSwitch(bool arg)
{
    // qDebug() << "[SettingsControlWidget] setSwitch function started with arg:" << arg;
    m_SwitchBtn->setChecked(arg);
}

DLineEdit *SettingsControlWidget::lineEdit()
{
    // qDebug() << "[SettingsControlWidget] lineEdit function started";
    return m_Edit;
}

SettingsLineWidget::SettingsLineWidget(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "SettingsLineWidget created";
}

bool SettingsLineWidget::initUI(QString text, bool currentStat)
{
    qDebug() << "Initializing settings line widget with text:" << text << "current state:" << currentStat;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    DLabel *pLabel = new DLabel(text,this);
    m_SwitchBtn = new DSwitchButton();
    m_SwitchBtn->setChecked(currentStat);
    layout->addWidget(pLabel);
    layout->addStretch();
    layout->addWidget(m_SwitchBtn, 0, Qt::AlignRight);

    connect(m_SwitchBtn, &DSwitchButton::checkedChanged, this, [=](bool stat) {
        emit checkedChanged(stat);
    });
    qDebug() << "[SettingsLineWidget] initUI function ended with result: true";
    return true;
}

bool SettingsLineWidget::initUI(QString text, const QStringList &textList, QString currenttext)
{
    qDebug() << "Initializing settings line widget with text:" << text << "current text:" << currenttext;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    DLabel *pLabel = new DLabel(text,this);
    m_comboBox = new QComboBox(this);
    m_comboBox->setFixedWidth(150);
    m_comboBox->addItems(textList);
    m_comboBox->setCurrentText(currenttext);
#if QT_VERSION_MAJOR > 5
    m_comboBox->setAccessibleName(text.remove(QRegularExpression("\\s")));
#else
    m_comboBox->setAccessibleName(text.remove(QRegExp("\\s")));
#endif
    layout->addWidget(pLabel);
    layout->addStretch();
    layout->addWidget(m_comboBox, 0, Qt::AlignRight);
    connect(m_comboBox, &QComboBox::currentTextChanged, this, [=](const QString & text) {
        m_comboBox->setEnabled(false);
        QTimer::singleShot(2000, this, [=]() {
            m_comboBox->setEnabled(true);
        });
        emit currentTextChanged(text);
        m_comboBox->setAccessibleName(text);
    });
    qDebug() << "[SettingsLineWidget] initUI function ended with result: true";
    return true;
}

void SettingsLineWidget::setSwitch(QString arg)
{
    // qDebug() << "[SettingsLineWidget] setSwitch function started with arg:" << arg;
    if(arg == "true"){
        m_SwitchBtn->setChecked(true);
    } else {
        m_SwitchBtn->setChecked(false);
    }
    // qDebug() << "[SettingsLineWidget] setSwitch function ended";
}

void SettingsLineWidget::setSize(QString size)
{
    // qDebug() << "[SettingsLineWidget] setSize function started with size:" << size;
    if ("3" == size) {
        m_comboBox->setCurrentIndex(0);
    } else if ("5" == size) {
        m_comboBox->setCurrentIndex(1);
    } else if ("10" == size) {
        m_comboBox->setCurrentIndex(2);
    } else if ("20" == size) {
        m_comboBox->setCurrentIndex(3);
    }
    // qDebug() << "[SettingsLineWidget] setSize function ended";
}

void SettingsLineWidget::AddressThreadSize(QString size)
{
    // qDebug() << "[SettingsLineWidget] AddressThreadSize function started with size:" << size;
    if ("1" == size) {
        m_comboBox->setCurrentIndex(0);
    }else if ("3" == size) {
        m_comboBox->setCurrentIndex(1);
    } else if ("5" == size) {
        m_comboBox->setCurrentIndex(2);
    } else if ("7" == size) {
        m_comboBox->setCurrentIndex(3);
    } else if ("10" == size) {
        m_comboBox->setCurrentIndex(4);
    }
    // qDebug() << "[SettingsLineWidget] AddressThreadSize function ended";
}
