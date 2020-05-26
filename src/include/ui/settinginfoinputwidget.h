/**
* @file settinginfoinputwidget.h
* @brief 设置信息可编辑输入窗口控件，最大下载限速以及最大上传限速用到此控件
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 13:52
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef SETTINGINFOINPUTWIDGET_H
#define SETTINGINFOINPUTWIDGET_H

#include <QWidget>
#include <DLineEdit>
#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @class SettingInfoInputWidget
 * @brief 设置信息可编辑输入窗口类
*/
class SettingInfoInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingInfoInputWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置标题
     * @param strText 标题文本
     */
    void setTitleLabelText(const QString &strText);

    /**
     * @brief 设置单位
     * @param strText 单位文本
     */
    void setUnitLabelText(const QString &strText);

    /**
     * @brief 设置范围
     * @param strText 范围文本
     */
    void setRangeLabelText(const QString &strText);

    /**
     * @brief 设置行编辑器当前值
     * @param strText 当前值
     */
    void setLineEditText(const QString &strText);

    /**
     * @brief 设置行编辑器是否可编辑
     * @param bIsDisabled 布尔值true或者false
     */
    void setLineEditIsDisabled(bool bIsDisabled);

    /**
     * @brief 设置行编辑器数值输入范围
     * @param minValid 最小输入值；nMaxValid 最大输入值
     */
    void setValid(int nMinValid ,int nMaxValid);

    /**
     * @brief 获取行编辑器数值
     * @return 行编辑器当前文本
     */
    QString getLineEditText();

signals:
    void textChanged(QString strText);

private slots:

    /**
     * @brief 行编辑器文本改变响应的槽函数
     * @param strText 改变后的文本
     */
    void lineEditTextChangedSlot(const QString &strText);

private:

    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DLineEdit *m_pLineEdit; // 行编辑器
    DLabel *m_pTitleLabel; // 标题
    DLabel *m_pUnitLabel; // 单位
    DLabel *m_pRangeLabel; // 范围
};

#endif // SETTINGINFOINPUTWIDGET_H
