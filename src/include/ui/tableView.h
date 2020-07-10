/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tableView.h
 *
 * @brief 下载条目列表
 *
 * @date 2020-06-09 09:56
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

#ifndef TABLEVIEW_H
#define TABLEVIEW_H


#include <QTableView>

class Settings;
class TableModel;
class ItemDelegate;
class TopButton;
class tableDataControl;
class HeaderView;
/**
 * @class TableView
 * @brief 下载条目列表
*/
class TableView:public QTableView
{
    Q_OBJECT
public:
    TableView(int Flag, TopButton* pToolBar);
    void reset(bool switched=false);
    /**
     * @brief 获取model
     * @return model
    */
    TableModel* getTableModel();

    /**
     * @brief 获取control
     * @return Control
    */
    tableDataControl* getTableControl();

    /**
     * @brief 获取header
     * @return header
    */
    HeaderView* getTableHeader();

    /**
     * @brief 刷新列表
    */
    void refreshTableView(const int &index);

    /**
     * @brief 右键菜单
     */
    void onContextMenu(QPoint pos);
private:
    /**
     * @brief 界面初始化
    */
    void initUI();

    /**
     * @brief 信号槽连接初始化
    */
    void initConnections();

    /**
     * @brief 表格初始化
    */
    void initTableView();

signals:

    //void getDatachanged();
    /**
     * @brief 表头全选按键状态改变信号
     */
    void signal_HeaderStatechanged(bool checked);

    /**
     * @brief 清除表头选中状态信号
     */
    void signal_ClearHeaderCheck();

    /**
     * @brief 表头全选按键选中
     */
    void signal_TableViewAllChecked(bool checked);

    /**
     * @brief 鼠标悬停行改变
     */
    void signal_Hoverchanged(const QModelIndex &index);
protected:
    /**
     * @brief 鼠标按下事件
    */
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief 离开事件
    */
    void leaveEvent(QEvent *event);
    /**
     * @brief 鼠标移动事件
    */
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief 键盘按下事件
    */
    void keyPressEvent(QKeyEvent *event);
private:
    int m_iTableFlag;
    TableModel *m_pTableModel;
    tableDataControl * m_ptableDataControl;
    HeaderView* m_pHeaderView;
    ItemDelegate *m_pItemdegegate;
    Settings *m_pSetting;
    TopButton *m_pToolBar;
};

#endif // TABLEVIEW_H
