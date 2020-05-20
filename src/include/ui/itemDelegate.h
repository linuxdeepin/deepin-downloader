/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H


#include <QStyledItemDelegate>

class QProgressBar;
class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:    
    ItemDelegate(QObject *parent = nullptr,int Flag=0);
    ~ItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent*event, QAbstractItemModel *model,  const QStyleOptionViewItem &option,const QModelIndex &index);
private:
    int Table_Flag;
     QProgressBar *progressbar;
     int hoverRow;
     QPixmap *bg;
     QPixmap *front;
public slots:
     void slot_hoverChanged(const QModelIndex &index);

};

#endif
