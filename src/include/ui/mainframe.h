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
class BtInfoDialog;
class tableDataControl;

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

public slots:
    /**
     * @brief 剪切板数据改变，受到bt文件url
    */
    void slot_ClipboardDataForBt(QString url);

private slots:
    /**
     * @brief 点击托盘图表的槽函数
     * @param reason 激活原因
     */
    void slot_Activated(QSystemTrayIcon::ActivationReason reason);

    /**
     * @brief 设置按钮槽函数
    */
    void slot_SettingsMenuClicked();
    /**
     * @brief 处理rpc返回的信息
     * @param method: aria2调用的接口名称
     * @param json： 字符串
    */
    void slot_RpcSuccess(QString method, QJsonObject json);

    /**
     * @brief 处理rpc返回错误的信息
     * @param method: aria2调用的接口名称
     * @param id： 唯一id
     * @param error： 错误号
    */
    void slot_RpcError(QString method, QString id, int error);

    /**
     * @brief 表格中元素被选中
    */
    void slot_TableItemSelected(const QModelIndex &selected);

    /**
     * @brief 剪切板数据改变，需要新建任务
    */
    void slot_ClipboardDataChanged(QString url);

    /**
     * @brief 切换显示列表
     * @param index 节点
    */
    void slot_ListClicked(const QModelIndex &index);
    /**
     * @brief 主题改变
     * @param type 颜色类型
    */
    void slot_Palettetypechanged(DGuiApplicationHelper::ColorType type);

    /**
     * @brief 收到新建任务url
     * @param url 收到url地址
     * @param savePath 保存路径
    */
    void slot_getNewDownloadUrl(QStringList url, QString savePath, QString fileName);

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
    void slot_getNewDownloadTorrent(QString btPath,QMap<QString,QVariant> opt,QString infoName, QString infoHash);

    /**
     * @brief 表头全部选择按键
     * @param  isChecked ：是否全选
    */
    void slot_HeaderStatechanged(bool isChecked);

    /**
     * @brief 设置右键菜单
    */
    void slot_ContextMenu(const QPoint &pos);

    /**
     * @brief 查找的文本改变
    */
    void slot_SearchEditTextChanged(QString text);

    /**
     * @brief 定时器更新界面显示
    */
    void slot_updateMainUI();

    /**
     * @brief 新建任务按钮槽函数
    */
    void slot_NewBtnClicked();

    /**
     * @brief 开始下载按键按下槽函数
    */
    void slot_StartDownloadBtnClicked();

    /**
     * @brief 暂停下载按键按下槽函数
    */
    void slot_PauseDownloadBtnClicked();

    /**
     * @brief 删除按键按下槽函数
    */
    void slot_DeleteDownloadBtnClicked();

    /**
     * @brief 改变列表选中槽函数
    */
    void slot_CheckChanged(bool checked, int flag);


    /**
     * @brief  获取删除窗口确定信号
     * @param ischecked 是否删除本地文件，true 删除本地文件；false 不删除
     * @param permanent 是否彻底删除，true彻底删除；false不彻底删除
     */
    void slot_GetDeleteConfirm(bool ischecked,bool permanent);

    /**
     * @brief 重新下载
     * @param taskId 任务ID
     * @param rd 重新开始前所在列表 正在下载、已完成、回收站
     */

    void slot_Redownload(QString taskId, int rd);

    /**
     * @brief 移除指定下载
     */
    void slot_Aria2Remove(QString gId, QString id);

    /**
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void slot_TrayQuitClick();

    /**
     * @brief messageBox关闭返回事件
     * @param index 按钮index
     */
    void slot_MessageBoxConfirmClick();

    /**
     * @brief 删除ACtion槽函数
    */
    void slot_DelActionTriggered();

    /**
     * @brief 重新下载ACtion槽函数
    */
    void slot_RedownloadActionTriggered();

    /**
     * @brief 还原下载ACtion槽函数
    */
    void slot_ReturnOriginActionTriggered();

    /**
     * @brief 打开文件ACtion槽函数
    */
    void slot_OpenFileActionTriggered();

    /**
     * @brief 打开文件目录ACtion槽函数
    */
    void slot_OpenFolderActionTriggered();

    /**
     * @brief 重命名文件ACtion槽函数
    */
    void slot_RenameActionTriggered();

    /**
     * @brief 移动到文件夹ACtion槽函数
    */
    void slot_MoveToActionTriggered();

    /**
     * @brief 清除回收站ACtion槽函数
    */
    void slot_ClearRecyleActionTriggered();

    /**
     * @brief 复制URL ACtion槽函数
    */
    void slot_CopyUrlActionTriggered();

    /**
     * @brief 永久删除ACtion槽函数
    */
    void slot_DeletePermanentActionTriggered();

    /**
    * @brief 是否开启bt文件托管
    * @param status true 为开启 false为关闭
    */
    void slot_isStartAssociatedBTFile(bool status);

    /**
    * @brief 重命名确认槽函数
    * @param name 新名称
    */
    void slot_RenameConfirmSlot(QString &name);

    /**
    * @brief 重新下载确认槽函数
    */
    void slot_RedownloadConfirmSlot(const QList<QString> &sameUrlList);

    /**
     * @brief 判断下载限速
     */
    void slot_DownloadLimitChanged();

    /**
     * @brief 是否开机启动
     */
    void slot_PowerOnChanged(bool isPowerOn);

    /**
     * @brief 设置里最大任务数改变
     */
    void slot_MaxDownloadTaskNumberChanged(int nTaskNumber) ;

    /**
     * @brief 设置里磁盘缓存改变
     */
    void slot_DisckCacheChanged(int nNum);

    /**
     * @brief 清空回收站确认槽函数
     */
    void slot_ClearRecycle(bool ischecked);

    /**
     * @brief 剪切板将
     * @param reason 激活原因
     */
    void slot_showWindowsForClipboard();

    void getTruetUrlList(QStringList urlList, QString path, QString name="");
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
    void setTaskNum(int currentLab);

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
    S_Task getUrlToName(QString url, QString savePaht, QString name);

    /**
     * @brief 开始或者继续下载任务
    */
    void continueDownload(Global::DataItem *pItem);

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
    bool showRedownloadMsgbox(QList<QString> sameUrlList);

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
    void initDataItem(Global::DataItem *data, const S_Task &tbTask);

    /**
     * @brief 初始化DelDataItem
     */
    void initDelDataItem(Global::DataItem* data, Global::DelDataItem *delData);

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

protected:
    /**
     * @brief 键盘按下事件
     * @param event 事件类型
     */

    void keyPressEvent(QKeyEvent *event);
    /**
     * @brief 键盘释放事件set_rename_MsgBox
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

    /**
     * @brief 绘图事件
     * @param event 事件类型
     */
    void paintEvent(QPaintEvent *event);

private:
    enum tableviewFlag{
        downloading,recycle
    };
    enum currentLab{
        downloadingLab = 0,finishLab,recycleLab
    };
    TopButton *m_pToolBar;
    TableView *m_pDownLoadingTableView, *m_pRecycleTableView;
    QWidget *m_pLeftWidget;
    QWidget *m_pRight_Widget;
    QWidget *m_pNotaskWidget;
    DLabel *m_pNotaskLabel;
    DLabel *m_pNoResultlabel;
    DLabel *m_pNotaskTipLabel;
    QStackedWidget *m_pRightStackwidget;
    QWidget *m_pTaskNumWidget;
    QLabel  *m_pTaskNum;
    DListView *m_pLeftList;

    DStandardItem *m_pdownloadingItem;
    QStandardItem *m_pdownloadfinishItem;
    QStandardItem *m_precycleItem;
    QSystemTrayIcon *m_pSystemTray;
    ClipboardTimer *m_pClipboard;
    QAction *m_pSettingAction;
    QTimer *m_pUpdateTimer;
    QTimer *m_TrayClickTimer;

    SettingsWidget *m_pSettingWidget;
    currentLab m_iCurrentLab; // 当前显示列表，正在下载、已完成、回收站
    int m_iDownloadingHeaderCheckStatus=0;
    int m_iFinishHeaderCheckStatus=0;
    QString m_SearchContent;
    bool m_bShutdownOk = true;

    QList<Global::DataItem*> m_reloadList;  /*已完成界面点击重新下载的数据列表*/
    QList<Global::DelDataItem*> m_recycleReloadList;  /*回收站界面点击重新下载的数据列表*/
    Global::DataItem* m_pCheckItem;
    QModelIndex m_CheckIndex;
    QList<Global::DataItem*> m_pDeleteList;
    QList<Global::DelDataItem*> m_pRecycleDeleteList;

    QString m_curOpenBtDialogPath;  //当前打开bt文件地址

    bool m_bctrlkeyPress = false;
    bool m_bIsCopyUrlFromLocal = false;
signals:
     void signal_switchTable();
     void signal_headerViewChecked(bool checked);
     void signal_tableChanged(int index);
     void signal_Redownload(QString taskId, int rd);
};

#endif // MAINFRAME_H
