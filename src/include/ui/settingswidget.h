#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <DMainWindow>
#include <DDialog>
#include <DListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStackedWidget>

#include "basicsettingwidget.h"
#include "monitoringsettingwidget.h"

DWIDGET_USE_NAMESPACE
class SettingsWidget : public DMainWindow
{
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    static SettingsWidget *getInstance();

signals:

public slots:

private slots:
    void listViewItemClicked(const QModelIndex &index);

private:
    // 初始化界面
    void initUI();
    // 初始化链接
    void initConnections();

private:
    static SettingsWidget *s_pInstance;
    QWidget *m_pLeftWidget;
    QWidget *m_pRightWidget;
    DListView *m_pListView;
    QStandardItemModel *m_pStandardItemModel;
    QStandardItem *m_pBasicSettings; // 基本设置
    QStandardItem *m_pMonitoringSettings; // 接管设置
    QStandardItem *m_pDownloadsSettings; // 下载设置
    QStandardItem *m_pNotifications; // 通知提醒
    QStandardItem *m_pAdvancedSettings; // 高级设置
    QStackedWidget *m_pStackedWidget; // 叠加窗口
    BasicSettingWidget *m_pBasicSettingWidget; // 基本设置界面
    MonitoringSettingWidget *m_pMonitoringSettingWidget; // 接管设置界面
};

#endif // SETTINGSWIDGET_H
