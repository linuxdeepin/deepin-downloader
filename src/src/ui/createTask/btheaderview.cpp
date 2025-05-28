// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file btheaderview.cpp
 *
 * @brief bt表格头
 *
 * @date 2020-06-09 10:48
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

#include "btheaderview.h"
#include <QPainter>
#include <QStyleOptionButton>
#include <QLabel>

headerView::headerView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    // qDebug() << "headerView constructor, orientation:" << orientation;

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        // qDebug() << "[BtHeaderView] Setting dark theme";
        onPalettetypechanged(DGuiApplicationHelper::ColorType::DarkType);
    } else {
        // qDebug() << "[BtHeaderView] Setting light theme";
        onPalettetypechanged(DGuiApplicationHelper::ColorType::LightType);
    }
    //set
    // qDebug() << "[BtHeaderView] Constructor ended";
}

void headerView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // qDebug() << "[BtHeaderView] paintSection function started";
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    QStyleOptionButton option;
    option.rect = checkBoxRect(rect);
    style()->drawControl(QStyle::CE_Header, &option, painter);

    // QPixmap pixmap;
    //  pixmap.load(":/icons/icon/headerIcon.png");
    //  style()->drawItemPixmap(painter, option.rect, logicalIndex, pixmap);
    // qDebug() << "[BtHeaderView] paintSection function ended";
}

QRect headerView::checkBoxRect(const QRect &sourceRect) const
{
    // qDebug() << "[BtHeaderView] checkBoxRect function started";
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                 &checkBoxStyleOption);
    m_curWidget += sourceRect.width();
    QPoint checkBoxPoint(m_curWidget - 20, 14);

    if (m_curWidget > 400) {
        // qDebug() << "[BtHeaderView] Resetting curWidget to 0";
        m_curWidget = 0;
    }
    // qDebug() << "[BtHeaderView] checkBoxRect function ended";
    return QRect(checkBoxPoint, checkBoxRect.size());
}

void headerView::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    // qDebug() << "onPalettetypechanged to:" << type;

    Q_UNUSED(type);
    QPalette p;


    //palette.setColor(DPalette::Background, c);

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        // qDebug() << "[BtHeaderView] Setting dark theme palette";
        p.setColor(QPalette::Base, QColor(0,0,0,20));
        p.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base());
        
       // QColor c = DGuiApplicationHelper::instance()->applicationPalette().base();
       // c.setAlpha(70);
       // p.setColor(DPalette::Background, c);

    } else {
        // qDebug() << "[BtHeaderView] Setting light theme palette";
       // p.setColor(QPalette::Base, QColor(0,0,0,20));
        QColor c = DGuiApplicationHelper::instance()->applicationPalette().base().color();
        c.setAlpha(70);
        p.setColor(DPalette::Window, c);
        //p.setColor(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base().color());
    }
    setPalette(p);
    // qDebug() << "[BtHeaderView] onPalettetypechanged function ended";
}
