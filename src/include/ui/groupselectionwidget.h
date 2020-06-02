/**
* @file groupselectionwidget.h
* @brief 基于ItemSelectionWidget窗口控件，根据传入的选项名称列表，绘制一组复选框窗口控件，并实现单选功能
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 15:56
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef GROUPSELECTIONWIDGET_H
#define GROUPSELECTIONWIDGET_H

#include <QWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @class GroupSelectionWidget
 * @brief 自定义组复选框窗口类
*/
class GroupSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GroupSelectionWidget(QStringList lstItemName, QWidget *parent = nullptr);

    /**
     * @brief 设置描述信息
     * @param strText 描述信息
     */
    void setLabelText(QString strText);

    /**
     * @brief 设置描述信息Label是否隐藏，隐藏则表示没有描述
     * @param bIsHide 布尔值true或者false
     */
    void setLabelIsHide(bool bIsHide);

    /**
     * @brief 设置当前选择项
     * @param strName 选择项名称
     */
    void setCurrentSelected(QString strName);

signals:
    void selectedChanged(QString strItemText);

public slots:

private slots:

    /**
     * @brief 选项选中响应的槽函数
     * @param bIsChecked 选项的选择状态
     */
    void itemCheckedSlot(bool bIsChecked);

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
    DLabel *m_pLabel; // 描述信息
    QStringList m_lstItemName; // 选项名称列表
};

#endif // GROUPSELECTIONWIDGET_H
