/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file filesavepathchooser.h
 *
 * @brief 下载目录窗口，主要实现切换单选按钮时，文件保存路径控件的使能以及设置数据的传递与显示
 *
 *@date 2020-06-09 11:01
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
#ifndef FILESAVEPATHCHOOSER_H
#define FILESAVEPATHCHOOSER_H

#include <DRadioButton>
#include <dfilechooseredit.h>

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class FileSavePathChooser
 * @brief 下载目录窗口类
*/
class FileSavePathChooser : public QWidget
{
    Q_OBJECT
public:
    explicit FileSavePathChooser(int nCurrentSelect, const QString &strDownloadPath);

    /**
     * @brief 设置文件保存路径
     * @param strText 文件路径
     */
    void setLineEditText(const QString &strText);

    /**
     * @brief 设置当前选中按钮，当为1时，选中上次使用目录按钮
     * @param nCurrentSelect 当前选中值
     */
    void setCurrentSelectRadioButton(int nCurrentSelect);

signals:
    void signal_textChanged(QString strText);

public slots:

    /**
     * @brief 单选按钮选择响应的槽函数
     */
    void slot_radioButtonClickSlot();

    /**
     * @brief 文件保存路径改变响应的槽函数
     * @param strText 改变后的路径
     */
    void slot_lineEditTextChanged(const QString &strText);

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
    DFileChooserEdit *m_pFileChooserEdit; // 文件保存路径选择
    DRadioButton *m_pAutoLastPathRadioButton; // 自动修改为上次使用的目录按钮
    DRadioButton *m_pCustomsPathRadioButton; // 设置默认目录按钮
    int m_nCurrentSelect; // 当前选中按钮值
    QString m_strDownloadPath; // 当前保存路径
};

#endif // FILESAVEPATHCHOOSER_H
