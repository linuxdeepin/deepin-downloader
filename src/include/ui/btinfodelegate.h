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

/**
 * @class BtInfoDelegate
 * @brief bt窗口中表格item代理类
*/

class BtInfoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BtInfoDelegate(DDialog* dialog);
    ~BtInfoDelegate();

    void setHoverColor(QBrush c);   //设置颜色

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;      //绘制事件
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);   // 响应按钮事件 - 划过、按下

private:
    DDialog* m_dialog;      //父类窗口指针
    QCheckBox *m_checkBtn;
    int hoverRow;           //当前选择行
    QBrush hoverColor = QColor(0,0,0,13);

public slots:
    void slot_hoverChanged(const QModelIndex &index);       //获取tableview类中当前选择行
};

#endif // BTINFODELEGATE_H
