#include "btinfotableview.h"

#include <QMouseEvent>
#include <QDebug>

BtInfoTableView::BtInfoTableView(QWidget *parent)
    : DTableView(parent)
{
}

void BtInfoTableView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<event->x()<<event->y();
    QModelIndex idx = this->indexAt(event->pos());
    emit signal_hoverChanged(idx);
}

void BtInfoTableView::leaveEvent(QEvent *event)
{
    this->reset();
    emit signal_hoverChanged(QModelIndex());
}
