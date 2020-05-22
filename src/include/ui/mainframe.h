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

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE


class TableView;
class QStackedWidget;
class TopButton;
class QSystemTrayIcon;

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
     * @brief mainwidow关闭事件
     * @param event 事件类型
     */
    void closeEvent(QCloseEvent *event);
private:
    TopButton *m_pToolBar;
    TableView* m_pTableView;
    QWidget *m_pLeftWidget;
    QWidget *m_pRight_Widget;
    QWidget *m_pNoTask_Widget;
    DLabel *m_pNoTask_label;
    QLabel *m_pNoTask_tip_Label;
    QStackedWidget *m_pRightStackwidget;
    QWidget *m_pTask_Num_Widget;
    QLabel  *m_pTask_Num;
    DListView *m_pLeft_list;

    QStandardItem * m_pDownloading_item;
    QStandardItem * m_pDownloadFinish_item;
    QStandardItem * m_pRecycle_item;
    QSystemTrayIcon * m_pSystemTray;

    QAction* m_pSettingAction;
};

#endif // MAINFRAME_H
