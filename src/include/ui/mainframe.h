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
class ClipboardTimer;
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
    void onTrayQuitClick();
    /**
     * @brief 新建任务按钮槽函数
    */
    void onNewBtnClicked();
    /**
     * @brief 设置按钮槽函数
    */
    void onSettingsMenuClicked();
    /**
     * @brief 处理rpc返回的信息
     * @param method: aria2调用的接口名称
     * @param json： 字符串
    */
    void slotRpcSuccess(QString method, QJsonObject json);

    /**
     * @brief 处理rpc返回错误的信息
     * @param method: aria2调用的接口名称
     * @param id： 唯一id
     * @param error： 错误号
    */
    void slotRpcError(QString method, QString id, int error);

    /**
     * @brief 剪切板数据改变，需要新建任务
    */
    void onClipboardDataChanged(QString url);
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
    void getNewDownloadTorrent(QString btPath, QMap<QString, QVariant> opt, QString infoName, QString infoHash);
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

    /**
     * @brief 开始下载新任务槽函数
     *  @param url： 地址
     *  @param savepath： 保存路径
    */
    void getNewdowloadSlot(QString url, QString savepath);

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
    void initTabledata();
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

    /**
     * @brief 显示报警窗口
     */
    void showWarningMsgbox(QString title, int sameUrlCount, QList<QString> sameUrlList);

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
     * @brief 格式化下载速度（1B1KB1MB1GB  /S）
     */
    QString formatDownloadSpeed(long size);

    /**
     * @brief 从配置文件中获取下载路径
     */
    QString getDownloadSavepathFromConfig();

    /**
     * @brief aria2下载事件
     */
    void aria2MethodAdd(QJsonObject json);

    /**
     * @brief aria2状态改变事件
     */
    void aria2MethodStatusChanged(QJsonObject json);

    /**
     * @brief aria2关闭事件
     */
    void aria2MethodShutdown(QJsonObject json);

    /**
     * @brief aria2获取文件事件
     */
    void aria2MethodGetFiles(QJsonObject json);

    /**
     * @brief aria2继续下载事件
     */
    void aria2MethodUnpause(QJsonObject json);

    /**
     * @brief aria2强制删除事件
     */
    void aria2MethodForceRemove(QJsonObject json);

private:
    enum tableView_flag {
        downloading,
        recycle
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
    QLabel *m_pTask_Num;
    DListView *m_pLeft_list;

    QStandardItem *m_pDownloading_item;
    QStandardItem *m_pDownloadFinish_item;
    QStandardItem *m_pRecycle_item;
    QSystemTrayIcon *m_pSystemTray;
    ClipboardTimer *m_pClipboard;
    QAction *m_pSettingAction;
    QTimer *m_pUpdatetimer;
    Settings *m_pSettings;

    SettingsWidget *m_pSettingWidget;
    int m_iCurrentListviewRow;
    int m_iDownloadingHeaderCheckStatus = 0;
    int m_iFinishHeaderCheckStatus = 0;
    QString m_searchContent;
    bool m_bShutdownOk = false;
signals:
    void switchTableSignal();
    void tableChanged(int index);

    void signalAutoDownloadBt(QString btFilePath);
    void signalRedownload(QString taskId, int rd);
};

#endif // MAINFRAME_H
