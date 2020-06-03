/**
* @file topButton.h
* @brief
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:42
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <DIconButton>
#include <DPushButton>
#include <DSearchEdit>
#include <DLabel>

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class TopButton
 * @brief 顶部按钮和搜索框
*/
class TopButton : public QWidget
{
    Q_OBJECT
public:
    explicit TopButton(QWidget *parent = nullptr);

    /**
     * @brief 获取查找的内容
     * @return 查找的内容
    */
    QString getSearchText()
    {
        return m_pSearchEdit->text();
    }

    /**
     * @brief 启用开始按键
     * @param enable： 启用开关
    */
    void enableStartBtn(bool enable)
    {
        m_pStartDownloadBtn->setEnabled(enable);
    }
    /**
     * @brief 启用暂停按键
     * @param enable： 启用开关
    */
    void enablePauseBtn(bool enable)
    {
        m_pPauseDownloadBtn->setEnabled(enable);
    }
    /**
     * @brief 启用删除按键
     * @param enable： 启用开关
    */
    void enableDeleteBtn(bool enable)
    {
        m_pDeleteDownloadBtn->setEnabled(enable);
    }

public slots:
    void getTableChanged(int index);
private:
    /**
     * @brief 初始化界面
    */
    void Init();
    /**
     * @brief 初始化信号槽
    */
    void InitConnections();

signals:
    /**
     * @brief 新建下载信号
    */
    void newDownloadBtnClicked();
    /**
     * @brief 暂停下载信号
    */
    void pauseDownloadBtnClicked();
    /**
     * @brief 开始下载信号
    */
    void startDownloadBtnClicked();
    /**
     * @brief 删除下载信号
    */
    void deleteDownloadBtnClicked();
    /**
     * @brief 搜索框焦点改变信号
    */
    void getSearchEditFocus(bool focus);
    /**
     * @brief 搜索框内容改变信号
    */
    void getSearchEditTextChange(QString text);

private:
    DLabel *m_pIconLable;
    DIconButton *m_pNewDownloadBtn;
    DIconButton *m_pStartDownloadBtn;
    DIconButton *m_pStopDownloadBtn;
    DIconButton *m_pPauseDownloadBtn;
    DIconButton *m_pDeleteDownloadBtn;
    DSearchEdit *m_pSearchEdit;
};

#endif // TOOLBAR_H
