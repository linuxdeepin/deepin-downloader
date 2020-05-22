#ifndef BTINFOTABLEVIEW_H
#define BTINFOTABLEVIEW_H

#include <DTableView>
#include <DHeaderView>

DWIDGET_USE_NAMESPACE

/**
 * @class BtInfoTableView
 * @brief BT窗口中tableview类
*/

class BtInfoTableView : public DTableView
{
    Q_OBJECT
public:
    explicit BtInfoTableView(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event);        //鼠标移动事件，鼠标若移动该行，给窗口发送信号
    void leaveEvent(QEvent *event);                 //鼠标离开时间，鼠标若离开，给窗口发送信号

signals:
    void signal_hoverChanged(const QModelIndex &index); //发送表格中哪行被选中

public slots:

private:
};

#endif // BTINFOTABLEVIEW_H
