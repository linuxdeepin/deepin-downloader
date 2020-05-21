#ifndef BTINFOTABLEVIEW_H
#define BTINFOTABLEVIEW_H

#include <DTableView>
#include <DHeaderView>

DWIDGET_USE_NAMESPACE

class BtInfoTableView : public DTableView
{
    Q_OBJECT
public:
    explicit BtInfoTableView(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

signals:
    void signal_hoverChanged(const QModelIndex &index);

public slots:

private:
};

#endif // BTINFOTABLEVIEW_H
