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


class QStackedWidget;
class QSystemTrayIcon;
class QAction;
class SettingsWidget;
class TopButton;
class TableView;
class S_Task;
class ClipboardTimer;

namespace Global {
class DataItem;
class DelDataItem;
}


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

    void slotTableItemSelected(const QModelIndex &selected);

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
    void getNewDownloadTorrent(QString btPath,QMap<QString,QVariant> opt,QString infoName, QString infoHash);
    /**
     * @brief 表头全部选择按键
     * @param  isChecked ：是否全选
    */
    void getHeaderStatechanged(bool isChecked);

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

    /**
     * @brief 定时器更新界面显示
    */
    void updateMainUI();

    /**
     * @brief 新建任务按钮槽函数
    */
    void onNewBtnClicked();

    /**
     * @brief 开始下载按键按下槽函数
    */
    void onStartDownloadBtnClicked();

    /**
     * @brief 暂停下载按键按下槽函数
    */
    void onPauseDownloadBtnClicked();

    /**
     * @brief 删除按键按下槽函数
    */
    void onDeleteDownloadBtnClicked();

    /**
     * @brief 改变列表选中槽函数
    */
    void slotCheckChange(bool checked, int flag);

    /**
     * @brief 删除槽函数
    */
    void delDownloadingAction();

    /**
     * @brief get_delete_confirm_slot 获取删除窗口确定信号
     * @param ischecked 是否删除本地文件，true 删除本地文件；false 不删除
     * @param permanent 是否彻底删除，true彻底删除；false不彻底删除
     */
    void getDeleteConfirmSlot(bool ischecked,bool permanent);

    /**
     * @brief 移除指定下载
     */
    void slotAria2Remove(QString gId, QString id);

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
     * @brief 设置任务数
     * @param num 个数
    */
    void setTaskNum(int num);


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
    void showWarningMsgbox(QString title, int sameUrlCount = 0, QList<QString> sameUrlList = {});

    /**
     * @brief show_delete_MsgBox 显示删除或彻底删除警告窗口
     * @param permanently 是否是彻底删除 tru    e是显示彻底删除窗口，false是显示删除窗口
     */
    void showDeleteMsgbox(bool permanently);

    /**
     * @brief 从配置文件中获取下载路径
     */
    QString   getDownloadSavepathFromConfig();

    /**
     * @brief 退出之前保存
     */
    void saveDataBeforeClose();

protected:
    /**
     * @brief 鼠标按下事件
     * @param event 事件类型
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * @brief 鼠标释放事件
     * @param event 事件类型
     */
    void keyReleaseEvent(QKeyEvent *event);
    /**
     * @brief 主窗口大小变化事件
     * @param event 事件类型
     */
    void resizeEvent(QResizeEvent *event);

    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void closeEvent(QCloseEvent *event);


private:
    enum tableviewFlag{
        downloading,recycle
    };
    TopButton *m_pToolBar;
    TableView *m_pDownLoadingTableView, *m_pRecycleTableView;
    QWidget *m_pLeftWidget;
    QWidget *m_pRight_Widget;
    QWidget *m_pnotaskWidget;
    DLabel *m_pnotaskLabel;
    QLabel *m_pnotaskTipLabel;
    QStackedWidget *m_pRightStackwidget;
    QWidget *m_ptaskNumWidget;
    QLabel  *m_ptaskNum;
    DListView *m_pleftList;

    QStandardItem *m_pDownloading_item;
    QStandardItem *m_pDownloadFinish_item;
    QStandardItem *m_pRecycle_item;
    QSystemTrayIcon *m_pSystemTray;
    ClipboardTimer *m_pClipboard;
    QAction *m_pSettingAction;
    QTimer *m_pUpdatetimer;
    Settings *m_pSettings;

    SettingsWidget *m_pSettingWidget;
    int m_iCurrentListviewRow; // 当前显示列表，正在下载、已完成、回收站
    int m_iDownloadingHeaderCheckStatus=0;
    int m_iFinishHeaderCheckStatus=0;
    QString m_searchContent;
    bool m_bShutdownOk = false;

    QList<Global::DataItem*> reload_list;  /*已完成界面点击重新下载的数据列表*/
    QList<Global::DelDataItem*> recycle_reload_list;  /*回收站界面点击重新下载的数据列表*/
    QList<Global::DataItem*> rename_list;
    QList<Global::DataItem*> m_pDeleteList;
    QList<Global::DelDataItem*> m_pRecycleDeleteList;

    bool m_bCtrlKey_press=false;
signals:
     void switchTableSignal();
     void tableChanged(int index);

     void signalRedownload(QString taskId, int rd);
};

#endif // MAINFRAME_H
