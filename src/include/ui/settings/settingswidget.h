/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file settingswidget.h
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <DLineEdit>
#include <DComboBox>
#include <DSwitchButton>
/**
 * @class SettingsControlWidget
 * @brief 设置界面小控件类
*/
class SettingsControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsControlWidget(QWidget *parent = nullptr);
    // 初始化界面
    bool initUI(QString label, QString text, bool isLineEdit = true);
    void setSpeend(QString speed);
    void setSize(QString size);
    void setSwitch(bool arg);
    Dtk::Widget::DLineEdit *lineEdit();
private:
    Dtk::Widget::DLineEdit *m_Edit;
    Dtk::Widget::DComboBox *m_ComboBox;
    Dtk::Widget::DSwitchButton *m_SwitchBtn;
signals:
    void TextChanged(const QString &text);
    void checkedChanged(bool arg);
};

#endif // SETTINGSWIDGET_H
