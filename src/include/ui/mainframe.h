/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainframe.h
 *
 * @brief 主界面类
 *
 * @date 2020-06-09 09:44
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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <DMainWindow>
#include <DLabel>
#include <DListView>
#include <DStatusBar>
#include <QSystemTrayIcon>

#include <DTitlebar>
#include <DIconButton>
#include <DApplication>
#include <QJsonObject>
#include <QUuid>
#include <QNetworkReply>
#include <QProcess>

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
class Task;
class ClipboardTimer;
class BtInfoDialog;
class tableDataControl;
class CreateTaskWidget;

namespace Global {
class DownloadDataItem;
class DeleteDataItem;
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

public slots:
    /**
     * @brief 剪切板数据改变，受到bt文件url
    */
    void onClipboardDataForBt(QString url);

private slots:
    /**
     * @brief 点击托盘图表的槽函数
     * @param reason 激活原因
     */
    void onActivated(QSystemTrayIcon::ActivationReason reason);

    /**
     * @brief 设置按钮槽函数
    */
    void onSettingsMenuClicked();
    /**
     * @brief 处理rpc返回的信息
     * @param method: aria2调用的接口名称
     * @param json： 字符串
    */
    void onRpcSuccess(QString method, QJsonObject json);

    /**
     * @brief 处理rpc返回错误的信息
     * @param method: aria2调用的接口名称
     * @param id： 唯一id
     * @param error： 错误号
    */
    void onRpcError(QString method, QString id, int error);

    /**
     * @brief 表格中元素被选中
    */
    void onTableItemSelected(const QModelIndex &selected);

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
    void onPalettetypechanged(DGuiApplicationHelper::ColorType type);

    /**
     * @brief 收到新建任务url
     * @param url 收到url地址
     * @param savePath 保存路径
    */
    void onDownloadNewUrl(QStringList &urlList, QString savePath, QString fileName);

    /**
     * @brief 收到新建任务
     * @param btName 文件路径
     * @param opt  下载参数
     * opt.insert("dir",下载到指定目录);
     * opt.insert("select-file",选中下载的文件索引集合);
     * @param savePath 保存路径
     * @param infoName 文件名字
     * @param infoName 文件hash值
    */
    void onDownloadNewTorrent(QString btPath,QMap<QString,QVariant> &opt,QString infoName, QString infoHash);

    /**
     * @brief 表头全部选择按键
     * @param  isChecked ：是否全选
    */
    void onHeaderStatechanged(bool isChecked);

    /**
     * @brief 设置右键菜单
    */
    void onContextMenu(const QPoint &pos);

    /**
     * @brief 查找的文本改变
    */
    void onSearchEditTextChanged(QString text);

    /**
     * @brief 定时器更新界面显示
    */
    void onUpdateMainUI();

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
     * @brief 改变列表选中槽函数,设置顶部按钮是否可用
    */
    void onCheckChanged(bool checked, int flag);


    /**
     * @brief  获取删除窗口确定信号
     * @param ischecked 是否删除本地文件，true 删除本地文件；false 不删除
     * @param permanent 是否彻底删除，true彻底删除；false不彻底删除
     */
    void onDeleteConfirm(bool ischecked,bool permanent);

    /**
     * @brief 重新下载
     * @param taskId 任务ID
     * @param rd 重新开始前所在列表 正在下载、已完成、回收站
     */

    void onRedownload(QString taskId, int rd);

    /**
     * @brief 删除任务完成
     */
    void onRemoveFinished();

    /**
     * @brief 移除指定下载
     */
    void onAria2Remove(QString gId, QString id);

    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void onTrayQuitClick();

    /**
     * @brief messageBox关闭返回事件
     * @param index 按钮index
     */
    void onMessageBoxConfirmClick();

    /**
     * @brief 删除ACtion槽函数
    */
    void onDelActionTriggered();

    /**
     * @brief 重新下载ACtion槽函数
    */
    void onRedownloadActionTriggered();

    /**
     * @brief 还原下载ACtion槽函数
    */
    void onReturnOriginActionTriggered();

    /**
     * @brief 打开文件ACtion槽函数
    */
    void onOpenFileActionTriggered();

    /**
     * @brief 打开文件目录ACtion槽函数
    */
    void onOpenFolderActionTriggered();

    /**
     * @brief 重命名文件ACtion槽函数
    */
    void onRenameActionTriggered();

    /**
     * @brief 移动到文件夹ACtion槽函数
    */
    void onMoveToActionTriggered();

    /**
     * @brief 清除回收站ACtion槽函数
    */
    void onClearRecyleActionTriggered();

    /**
     * @brief 复制URL ACtion槽函数
    */
    void onCopyUrlActionTriggered();

    /**
     * @brief 永久删除ACtion槽函数
    */
    void onDeletePermanentActionTriggered();

    /**
    * @brief 是否开启bt文件托管
    * @param status true 为开启 false为关闭
    */
    void onIsStartAssociatedBTFile(bool status);

    /**
    * @brief 重命名确认槽函数
    * @param name 新名称
    */
    void onRenameConfirmSlot(QString &name);

    /**
    * @brief 重新下载确认槽函数
    */
    void onRedownloadConfirmSlot(const QList<QString> &sameUrlList);

    /**
    * @brief 列表项双击事件，打开文件
    */
    void onTableViewItemDoubleClicked(QModelIndex index);

    /**
     * @brief 判断下载限速
     */
    void onDownloadLimitChanged();

    /**
     * @brief 是否开机启动
     */
    void onPowerOnChanged(bool isPowerOn);

    /**
     * @brief 设置里最大任务数改变
     */
    void onMaxDownloadTaskNumberChanged(int nTaskNumber) ;

    /**
     * @brief 设置里磁盘缓存改变
     */
    void onDisckCacheChanged(int nNum);

    /**
     * @brief 清空回收站确认槽函数
     */
    void onClearRecycle(bool ischecked);

    /**
     * @brief 剪切板将
     * @param reason 激活原因
     */
    void onShowWindowsForClipboard();

    /**
     * @brief 解析url
     */
    void onParseUrlList(QStringList urlList, QString path, QString name="");

    /**
     * @brief 解析url请求返回处理
     */
    void onHttpRequest(QNetworkReply *reply);
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
     * @param currentLab 当前显示列表，正在下载、已完成、回收站
    */
    void setTaskNum();

    /**
     * @brief 设置主题模式
    */
    void setPaletteType();

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
    Task getUrlToName(QString url, QString savePaht, QString name);

    /**
     * @brief 开始或者继续下载任务
    */
    void continueDownload(Global::DownloadDataItem *pItem);

    /**
     * @brief 清除item的选中状态
     */
    void clearTableItemCheckStatus();

    /**
     * @brief 显示报警窗口
     */
    void showWarningMsgbox(QString title, int sameUrlCount = 0, QList<QString> sameUrlList = {});

    /**
     * @brief 显示清空窗口
     */
    void showClearMsgbox();

    /**
     * @brief showDeleteMsgbox 显示删除或彻底删除警告窗口
     * @param permanently 是否是彻底删除 true是显示彻底删除窗口，false是显示删除窗口
     */
    void showDeleteMsgbox(bool permanently);

    /**
     * @brief showReloadMsgbox 显示重新下载窗口
     */
    void showReloadMsgbox();

    /**
     * @brief showRenameMsgbox 显示重命名窗口
     */
    void showRenameMsgbox();

    /**
     * @brief 显示重新下载窗口
     */
    bool showRedownloadMsgbox(QList<QString> &sameUrlList);

    /**
     * @brief 从配置文件中获取下载路径
     */
    QString   getDownloadSavepathFromConfig();

    /**
     * @brief 判断是否在限速期间
     */
    bool checkIfInPeriod(QTime *currentTime, QTime *periodStartTime, QTime *periodEndTime);

    /**
     * @brief 比较时间
     */
    int  checkTime(QTime *startTime, QTime *endTime);

    /**
     * @brief 初始化DataItem
     */
    void initDataItem(Global::DownloadDataItem *data, const Task &tbTask);

    /**
     * @brief 初始化DelDataItem
     */
    void initDelDataItem(Global::DownloadDataItem* data, Global::DeleteDataItem *delData);

    /**
     * @brief 将bt文件设置右键启动
     */
    void startBtAssociat();

    /**
     * @brief 将bt文件关闭右键启动
     */
    void endBtAssociat();
     /**
     * @brief 修改aria2配置文件
     * @param configItem 配置文件中的id
     * @param value 配置文件中的value
     */
    void modifyConfigFile(QString configItem, QString value);
    /**
     * @brief 处理设置界面通知设置函数
     */
    void btNotificaitonSettings(QString head,QString fileName,bool isBt=false);

    /**
     * @brief 是否为磁力链接
     * @param url 路径
     * @return  true 为磁力链接  false 为不是磁力链接
    */
    bool isMagnet(QString url);

    /**
     * @brief 清空共享内存
    */
    bool clearSharedMemory();

    /**
     * @brief 开始下载任务
     */
    void startDownloadTask(Global::DownloadDataItem *pItem);

    /**
     * @brief 网络是否联通
     * @return true为通 false为不通
     */
    bool isNetConnect();

    /**
     * @brief 当前是否是自动开启
     * @return true为是 false为否
     */
    bool isAutoStart();

    /**
     * @brief 设置开机自启
     * @return true为是 false为否
     */
    bool setAutoStart(bool ret);

protected:
    /**
     * @brief 键盘按下事件
     * @param event 事件类型
     */

    void keyPressEvent(QKeyEvent *event) override;
    /**
     * @brief 键盘释放事件set_rename_MsgBox
     * @param event 事件类型
     */

    void keyReleaseEvent(QKeyEvent *event) override;
    /**
     * @brief 主窗口大小变化事件
     * @param event 事件类型
     */

    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief 绘图事件
     * @param event 事件类型
     */
    void paintEvent(QPaintEvent *event) override;

private:
    enum tableviewFlag{
        downloading,recycle
    };
    enum CurrentTab{
        downloadingTab = 0,finishTab,recycleTab
    };
    TopButton *m_ToolBar;
    TableView *m_DownLoadingTableView, *m_RecycleTableView;
    QWidget *m_LeftWidget;
    QWidget *m_RightWidget;
    QWidget *m_NotaskWidget;
    DLabel *m_NotaskLabel;
    DLabel *m_NoResultlabel;
    DLabel *m_NotaskTipLabel;
    QStackedWidget *m_RightStackwidget;
    QWidget *m_TaskNumWidget;
    QLabel  *m_TaskNum;
    DListView *m_LeftList;

    DStandardItem *m_DownloadingItem;
    QStandardItem *m_DownloadFinishItem;
    QStandardItem *m_RecycleItem;
    QSystemTrayIcon *m_SystemTray;
    ClipboardTimer *m_Clipboard;
    QAction *m_SettingAction;
    QTimer *m_UpdateTimer;
    QTimer *m_TrayClickTimer;
    CreateTaskWidget *m_TaskWidget;
    SettingsWidget *m_SettingWidget;
    CurrentTab m_CurrentTab; // 当前显示列表，正在下载、已完成、回收站
    QString m_SearchContent;
    bool m_ShutdownOk = true;

    QList<Global::DownloadDataItem*> m_ReloadList;  /*已完成界面点击重新下载的数据列表*/
    QList<Global::DeleteDataItem*> m_RecycleReloadList;  /*回收站界面点击重新下载的数据列表*/
    Global::DownloadDataItem* m_CheckItem;
    Global::DeleteDataItem* m_DelCheckItem;
    QModelIndex m_CheckIndex;
    QList<Global::DownloadDataItem*> m_DeleteList;
    QList<Global::DeleteDataItem*> m_RecycleDeleteList;

    QString m_CurOpenBtDialogPath;  //当前打开bt文件地址
    QStringList m_ErrorUrlList;

    bool m_CtrlkeyPress = false;
    bool m_CopyUrlFromLocal = false;
signals:
     void isHeaderChecked(bool checked);
     void tableChanged(int index);
     void redownload(QString taskId, int rd);
};

#endif // MAINFRAME_H
