/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file downloadsettingwidget.h
 *
 * @brief 下载设置窗口，主要实现全速下载和限速下载的切换，以及限速下载时限制参数的设置
 *
 *@date 2020-06-09 11:03
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
#ifndef DOWNLOADSETTINGWIDGET_H
#define DOWNLOADSETTINGWIDGET_H

#include <DRadioButton>

#include <QWidget>
#include <QTimeEdit>

DWIDGET_USE_NAMESPACE

class SettingInfoInputWidget;

/**
 * @class DownloadSettingWidget
 * @brief 下载设置窗口类
*/
class DownloadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadSettingWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置当前选择的选项，当传入为1时，选中全速下载
     * @param nCurrentSelect 当前选项值
     */
    void setCurrentSelectRadioButton(int nCurrentSelect);

    /**
     * @brief 设置最大下载限速值
     * @param strText 限速值
     */
    void setMaxDownloadSpeedLimit(QString strText);

    /**
     * @brief 设置最大上传限速值
     * @param strText 限速值
     */
    void setMaxUploadSpeedLimit(QString strText);

    /**
     * @brief 设置限速开始时间
     * @param strText 开始时间
     */
    void setStartTime(QString strText);

    /**
     * @brief 设置限速结束时间
     * @param strText 结束时间
     */
    void setEndTime(QString strText);

signals:
    void speedLimitInfoChanged(QString strText);

public slots:

private slots:

    /**
     * @brief 限速时间改变响应的槽函数
     * @param time 改变后的时间
     */
    void timeChangedSlot(const QTime &time);

    /**
     * @brief 限速值改变响应的槽函数
     * @param strText 改变后的值
     */
    void textChangedSlot(QString strText);

private slots:
    void radioButtonClickSlot();

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
    DRadioButton *m_pFullSpeedDownloadButton; // 全速下载按钮
    DRadioButton *m_pSpeedLimitDownloadButton; // 限速下载按钮
    SettingInfoInputWidget *m_pMaxDownloadSpeedLimit; //最大下载限速输入框
    SettingInfoInputWidget *m_pMaxUploadSpeedLimit; // 最大上传限速输入框
    QTimeEdit *m_pStartTimeEdit; // 开始时间
    QTimeEdit *m_pEndTimeEdit; // 结束时间

};

#endif // DOWNLOADSETTINGWIDGET_H
