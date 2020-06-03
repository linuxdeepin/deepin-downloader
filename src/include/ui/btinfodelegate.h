/**
* @file btinfodelegate.h
* @brief bt窗口中表格item代理类
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date 2020-06-01 16:00
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef BTINFODELEGATE_H
#define BTINFODELEGATE_H

#include <QStyledItemDelegate>
#include <QEvent>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <DDialog>
#include <QCheckBox>
#include <QDir>

DWIDGET_USE_NAMESPACE

class BtInfoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BtInfoDelegate(DDialog *dialog);
    ~BtInfoDelegate();

    /**
     * @brief 设置选择行颜色
     * @param c 颜色
     */
    void setHoverColor(QBrush c);

protected:
    /**
     * @brief 重写绘画事件
     * @param painter 画笔
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const; //绘制事件
    /**
     * @brief 响应按钮事件 - 划过、按下
     * @param event 事件
     * @param model 模型
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index); // 响应按钮事件 - 划过、按下

private:
    DDialog *m_dialog; //父类窗口指针
    QCheckBox *m_checkBtn;
    int hoverRow; //当前选择行
    QBrush hoverColor = QColor(0, 0, 0, 13);

public slots:
    /**
     * @brief 选择行变动槽函数
     * @param index 当前选择行
     */
    void slot_hoverChanged(const QModelIndex &index);
};

#endif // BTINFODELEGATE_H
