/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file topButton.h
 *
 * @brief 主界面顶部按钮加搜索框
 *
 * @date 2020-06-09 10:00
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <DIconButton>
#include <DPushButton>
#include <DSearchEdit>
#include <DLabel>


#include <QWidget>

//class DownloadManagerBtn : public Dtk::Widget::DIconButton
//{
//    Q_OBJECT
//public:
//     explicit DownloadManagerBtn(QWidget *parent = nullptr);
//    ~DownloadManagerBtn();
//protected:
//    void mouseMoveEvent(QMouseEvent *event);
//private slots:
//    void onTimeOut();
//private:
//    QTimer *m_pHoverTimer;
//};

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
    /**
     * @brief 当前显示列表改变
     * @param index： 列表索引
    */
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
    Dtk::Widget::DLabel *m_pIconLable;
    Dtk::Widget::DIconButton *m_pNewDownloadBtn;
    Dtk::Widget::DIconButton *m_pStartDownloadBtn;
    Dtk::Widget::DIconButton *m_pStopDownloadBtn;
    Dtk::Widget::DIconButton *m_pPauseDownloadBtn;
    Dtk::Widget::DIconButton *m_pDeleteDownloadBtn;
    Dtk::Widget::DSearchEdit *m_pSearchEdit;
};



#endif // TOOLBAR_H
