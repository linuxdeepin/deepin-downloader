/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file monitoringsettingwidget.h
 *
 * @brief 接管设置
 *
 * @date 2020-06-09 10:52
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

#ifndef MONITORINGSETTINGWIDGET_H
#define MONITORINGSETTINGWIDGET_H

#include <QWidget>
#include <DSwitchButton>

DWIDGET_USE_NAMESPACE

/**
 * @class MonitoringSettingWidget
 * @brief 接管设置类
*/
class MonitoringSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitoringSettingWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    // 初始化界面
    void initUI();
    // 初始化链接
    void initConnections();

private:
    DSwitchButton *m_pClipBoardButton; // 剪切板
};

#endif // MONITORINGSETTINGWIDGET_H
