#ifndef MONITORINGSETTINGWIDGET_H
#define MONITORINGSETTINGWIDGET_H

#include <QWidget>
#include <DSwitchButton>

DWIDGET_USE_NAMESPACE
class MonitoringSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitoringSettingWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    // 初始化界面
    void initUI();
    // 初始化链接
    void initConnections();

private:
    DSwitchButton *m_pClipBoardButton; // 剪切板
};

#endif // MONITORINGSETTINGWIDGET_H
