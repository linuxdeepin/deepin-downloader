/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file btinfotableview.cpp
 *
 * @brief BT窗口中tableview类
 *
 * @date 2020-06-09 10:50
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
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

#include "btinfotableview.h"


BtInfoTableView::BtInfoTableView(QWidget *parent)
    : DTableView(parent)
{

}

void BtInfoTableView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<event->x()<<event->y();
    QModelIndex idx = indexAt(event->pos());
    emit hoverChanged(idx);
}

void BtInfoTableView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    reset();
    emit hoverChanged(QModelIndex());
}

