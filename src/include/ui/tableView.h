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
     * @brief aria2下载事件
     */
    void aria2MethodAdd(QJsonObject &json, QString &searchContent);

    /**
     * @brief aria2状态改变事件
     */
    void aria2MethodStatusChanged(QJsonObject &json, int iCurrentRow,  QString &searchContent);

    /**
     * @brief aria2关闭事件
     */
    void aria2MethodShutdown(QJsonObject &json);

    /**
     * @brief aria2获取文件事件
     */
    void aria2MethodGetFiles(QJsonObject &json, int iCurrentRow);

    /**
     * @brief aria2继续下载事件
     */
    void aria2MethodUnpause(QJsonObject &json, int iCurrentRow);

    /**
     * @brief aria2强制删除事件
     */
    void aria2MethodForceRemove(QJsonObject &json);

    /**
     * @brief 查找的文本改变
    */
    void searchEditTextChanged(QString text);

    /**
     * @brief 刷新列表
    */
    void refreshTableView(const int &index);

    /**
     * @brief 退出之前保存
     */
    void saveDataBeforeClose();

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

    /**
     * @brief 获取url中的文件名称
     * @param url 下载地址
     */
    QString getFileName(const QString &url);

    /**
     * @brief 处理设置界面通知设置函数
     */
    void dealNotificaitonSettings(QString statusStr, QString fileName);

    /**
     * @brief 格式化文件大小 （1B1KB1MB1GB）
     */
    QString formatFileSize(long size);

    /**
     * @brief 从配置文件中获取下载路径
     */
    QString   getDownloadSavepathFromConfig();

    /**
     * @brief 格式化下载速度（1B1KB1MB1GB  /S）
     */
    QString formatDownloadSpeed(long size);



signals:

    //void getDatachanged();
    /**
     * @brief 表头全选按键状态改变信号
     */
    void signalHeaderStatechanged(bool checked);

    /**
     * @brief 清除表头选中状态信号
     */
    void signalClearHeaderCheck();

    /**
     * @brief 表头全选按键选中
     */
    void signalTableViewAllChecked(bool checked);

    /**
     * @brief 鼠标悬停行改变
     */
    void signalHoverchanged(const QModelIndex &index);

    /**
     * @brief 文件为bt，自动开始下载
     */
    void signalAutoDownloadBt(QString btFilePath);

    /**
     * @brief 文件为bt，自动开始下载
     */
    void signalRedownload(QString taskId, int rd);
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
    ItemDelegate *m_pItemdegegate;
    Settings *m_pSetting;
    TopButton *m_pToolBar;
};

#endif // TABLEVIEW_H
