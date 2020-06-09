/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file groupselectionwidget.h
 *
 * @brief 基于ItemSelectionWidget窗口控件，根据传入的选项名称列表，绘制一组复选框窗口控件，并实现单选功能
 *
 *@date 2020-06-09 10:55
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
#ifndef GROUPSELECTIONWIDGET_H
#define GROUPSELECTIONWIDGET_H

#include <DLabel>

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class GroupSelectionWidget
 * @brief 自定义组复选框窗口类
*/
class GroupSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GroupSelectionWidget(QStringList lstItemName,QWidget *parent = nullptr);

    /**
     * @brief 设置描述信息
     * @param strText 描述信息
     */
    void setLabelText(QString strText);

    /**
     * @brief 设置描述信息Label是否隐藏，隐藏则表示没有描述
     * @param bIsHide 布尔值true或者false
     */
    void setLabelIsHide(bool bIsHide);

    /**
     * @brief 设置当前选择项
     * @param strName 选择项名称
     */
    void setCurrentSelected(QString strName);

signals:
    void selectedChanged(QString strItemText);

public slots:

private slots:

    /**
     * @brief 选项选中响应的槽函数
     * @param bIsChecked 选项的选择状态
     */
    void itemCheckedSlot(bool bIsChecked);

private:

    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DLabel *m_pLabel; // 描述信息
    QStringList m_lstItemName; // 选项名称列表

};

#endif // GROUPSELECTIONWIDGET_H
