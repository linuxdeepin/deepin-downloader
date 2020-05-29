/**
* @file mainframe.h
* @brief 主界面类
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:37
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <DMainWindow>
#include <DLabel>
#include <DListView>
#include <QSystemTrayIcon>

#include <DTitlebar>
#include <DIconButton>
#include <DApplication>
#include <QJsonObject>
#include <QUuid>

#include "settings.h"

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE


class TableView;
class QStackedWidget;
class TopButton;
class QSystemTrayIcon;
class SettingsWidget;
class S_Task;
/**
 * @class MainFrame
 * @brief 主界面类
*/
class MainFrame : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = Q_NULLPTR);


    ~MainFrame();

private slots:
    /**
     * @brief 点击托盘图表的槽函数
     * @param reason 激活原因
     */
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void on_tray_quit_click();
    /**
     * @brief 新建任务按钮槽函数
    */
    void onNewBtnClicked();
    /**
     * @brief 设置按钮槽函数
    */
    void onSettingsMenuClicked();
    /**
     * @brief 处理rpc成功返回的信息
    */
    void slotRPCSuccess(QString method, QJsonObject json);
    /**
     * @brief 处理返回的错误信息
    */
    void slotRPCError(QString method, QString id, int);
    /**
     * @brief 剪切板数据改变，需要新建任务
    */
    void onClipboardDataChanged();
    /**
     * @brief 切换显示列表
     * @param index 节点
    */
    void onListClicked(const QModelIndex &index);
    /**
     * @brief 主题改变
     * @param type 颜色类型
    */
    void getPalettetypechanged(DGuiApplicationHelper::ColorType type);

    /**
     * @brief 收到新建任务url
     * @param url 收到url地址
     * @param savePath 保存路径
    */
    void getNewDowloadUrl(QString url, QString savePath);


    /**
     * @brief 收到新建任务orrent
     * @param btName 文件路径
     * @param opt  下载参数
     * opt.insert("dir",下载到指定目录);
     × opt.insert("select-file",选中下载的文件索引集合);
     * @param savePath 保存路径
     * @param infoName 文件名字
     * @param infoName 文件hash值
    */
    void getNewDownloadTorrent(QString btPath,QMap<QString,QVariant> opt,QString infoName, QString infoHash);
    /**
     * @brief 表头状态改变
     * @param  i：节点
    */
    void getHeaderStatechanged(bool i);

    /**
     * @brief 设置右键菜单
    */
    void slotContextMenu(QPoint pos);

    /**
     * @brief 查找的文本改变
    */
    void slotSearchEditTextChanged(QString text);

private:

    /**
     * @brief 初始化aria2
    */
    void initAria2();

    /**
     * @brief 界面初始化
    */
    void init();
    /**
     * @brief 右下角托盘初始化
    */
    void initTray();
    /**
     * @brief 新建连接
    */
    void initConnection();
    /**
     * @brief 初始化三个列表，读数据库
    */
    void init_tableData();
    /**
     * @brief 刷新列表
     * @param index 节点
     * @param isClearSelection 是否清除
    */
    void refreshTableView(const int &index, bool isClearSelection = false);

    /**
     * @brief 设置任务数
     * @param num 个数
    */
    void setTaskNum(int num);

    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief 新建下载任务
     * @param url 下载地址
     */
    void createNewTask(QString url);

    /**
     * @brief 解析url，得到url名字
     * @param url 下载地址
     * @return 解析后Task结构体
     */
    S_Task getUrlToName(QString url, QString savePaht);

    /**
     * @brief 清除item的选中状态
     */
    void clearTableItemCheckStatus();

private:
    enum tableView_flag{
        downloading,recycle
    };
    TopButton *m_pToolBar;
    TableView *m_pDownLoadingTableView, *m_pDownLoadedTableView, *m_pRecycleTableView;
    QWidget *m_pLeftWidget;
    QWidget *m_pRight_Widget;
    QWidget *m_pNoTask_Widget;
    DLabel *m_pNoTask_label;
    QLabel *m_pNoTask_tip_Label;
    QStackedWidget *m_pRightStackwidget;
    QWidget *m_pTask_Num_Widget;
    QLabel  *m_pTask_Num;
    DListView *m_pLeft_list;

    QStandardItem *m_pDownloading_item;
    QStandardItem *m_pDownloadFinish_item;
    QStandardItem *m_pRecycle_item;
    QSystemTrayIcon *m_pSystemTray;
    QClipboard *m_pClipboard;
    QAction *m_pSettingAction;

    Settings *m_pSettings;

    SettingsWidget *m_pSettingWidget;
    int m_iCurrentListviewRow;
    int m_iDownloadingHeaderCheckStatus=0;
    int m_iFinishHeaderCheckStatus=0;
    QString m_searchContent;
signals:
     void switchTableSignal();
     void tableChanged(int index);

};

#endif // MAINFRAME_H
