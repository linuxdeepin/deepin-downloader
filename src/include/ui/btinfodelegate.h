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
    BtInfoDelegate(DDialog* dialog);
    ~BtInfoDelegate();

    void setHoverColor(QBrush c);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    DDialog* m_dialog;
    QCheckBox *m_checkBtn;

    int hoverRow;
    QBrush hoverColor = QColor(0,0,0,13);

public slots:
    void slot_hoverChanged(const QModelIndex &index);
};

#endif // BTINFODELEGATE_H
