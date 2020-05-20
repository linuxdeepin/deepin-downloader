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
class MainFrame : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = Q_NULLPTR);

    void init();
    ~MainFrame();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason);
    void closeEvent(QCloseEvent *event);
    void on_tray_quit_click();
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

private:
    void initAria2();//初始化aria2
private slots:
    void slotRPCSuccess(QString method, QJsonObject json);//处理rpc成功返回的信息
    void slotRPCError(QString method, QString id, int);//处理返回的错误信息

};

#endif // MAINFRAME_H
