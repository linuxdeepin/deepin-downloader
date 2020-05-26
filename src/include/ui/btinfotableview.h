#ifndef BTINFOTABLEVIEW_H
#define BTINFOTABLEVIEW_H

#include <DTableView>
#include <DHeaderView>

DWIDGET_USE_NAMESPACE

/**
* @file %{CurrentDocument:btinfotableview.h}
* @brief BT窗口中tableview类
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date %{CurrentDate:2020-05-26} %{CurrentTime:17:59}
* @copyright 2020-%{CurrentDate:2020} Uniontech Technology Co., Ltd.
*/


class BtInfoTableView : public DTableView
{
    Q_OBJECT
public:
    explicit BtInfoTableView(QWidget *parent = nullptr);

protected:
    /**
     * @brief 重写鼠标移动事件，鼠标移动事件，鼠标若移动该行，给窗口发送信号
     * @return event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief 重写鼠标离开事件，鼠标离开时间，鼠标若离开，给窗口发送信号
     * @return event 鼠标事件
     */
    void leaveEvent(QEvent *event);

signals:
    /**
     * @brief 发送选中表格信号
     * @return index 所选中行
     */
    void signal_hoverChanged(const QModelIndex &index);

public slots:

private:
};

#endif // BTINFOTABLEVIEW_H
