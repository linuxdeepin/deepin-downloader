/**
* @file downloadsettingwidget.h
* @brief 下载设置窗口，主要实现全速下载和限速下载的切换，以及限速下载时限制参数的设置
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 16:19
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef DOWNLOADSETTINGWIDGET_H
#define DOWNLOADSETTINGWIDGET_H

#include <QWidget>
#include <DRadioButton>
#include <QTimeEdit>

#include "settinginfoinputwidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @class DownloadSettingWidget
 * @brief 下载设置窗口类
*/
class DownloadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadSettingWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置当前选择的选项，当传入为1时，选中全速下载
     * @param nCurrentSelect 当前选项值
     */
    void setCurrentSelectRadioButton(int nCurrentSelect);

    /**
     * @brief 设置最大下载限速值
     * @param strText 限速值
     */
    void setMaxDownloadSpeedLimit(QString strText);

    /**
     * @brief 设置最大上传限速值
     * @param strText 限速值
     */
    void setMaxUploadSpeedLimit(QString strText);

    /**
     * @brief 设置限速开始时间
     * @param strText 开始时间
     */
    void setStartTime(QString strText);

    /**
     * @brief 设置限速结束时间
     * @param strText 结束时间
     */
    void setEndTime(QString strText);

signals:
    void speedLimitInfoChanged(QString strText);

public slots:

private slots:

    /**
     * @brief 限速时间改变响应的槽函数
     * @param time 改变后的时间
     */
    void timeChangedSlot(const QTime &time);

    /**
     * @brief 限速值改变响应的槽函数
     * @param strText 改变后的值
     */
    void textChangedSlot(QString strText);

private slots:
    void radioButtonClickSlot();

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
    DRadioButton *m_pFullSpeedDownloadButton; // 全速下载按钮
    DRadioButton *m_pSpeedLimitDownloadButton; // 限速下载按钮
    SettingInfoInputWidget *m_pMaxDownloadSpeedLimit; //最大下载限速输入框
    SettingInfoInputWidget *m_pMaxUploadSpeedLimit; // 最大上传限速输入框
    QTimeEdit *m_pStartTimeEdit; // 开始时间
    QTimeEdit *m_pEndTimeEdit; // 结束时间

};

#endif // DOWNLOADSETTINGWIDGET_H
