#ifndef BASICSETTINGWIDGET_H
#define BASICSETTINGWIDGET_H

#include <QWidget>
#include <DSwitchButton>
#include <DRadioButton>
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE
class BasicSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BasicSettingWidget(QWidget *parent = nullptr);

signals:

public slots:
    void radioButtonClickSlot();
    //    void lineEditTextChanged(const QString &strText);

private:
    // 初始化界面
    void initUI();
    // 初始化链接
    void initConnections();

private:
    DSwitchButton *m_pPowerOnButton; // 开机启动
    DSwitchButton *m_pUndisturbedModeButton; // 免打扰模式
    DSwitchButton *m_pAutostartUnfinishedTaskButton; // 启动后自动开始未完成的任务
    DRadioButton *m_pAutoLastPathRadioButton; // 自动修改为上次使用的目录
    DRadioButton *m_pCustomsPathRadioButton; // 设置默认目标目录
    DFileChooserEdit *m_pFileChooserEdit; // 下载路径选择
};

#endif // BASICSETTINGWIDGET_H
