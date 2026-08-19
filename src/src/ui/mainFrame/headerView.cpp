// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file headerView.cpp
 *
 * @brief 表头
 *
 * @date 2020-06-09 09:59
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
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

#include "headerView.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOptionHeader>
#include <QStyle>
#include <QAbstractItemModel>

DownloadHeaderView::DownloadHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    m_headerCbx = new QCheckBox(this);

    // connect(m_headerCbx,&DCheckBox::stateChanged,this,&HeaderView::get_stateChanged);
    connect(m_headerCbx,
            &DCheckBox::clicked,
            this,
            &DownloadHeaderView::Statechanged);
    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::paletteTypeChanged,
            this,
            &DownloadHeaderView::onPalettetypechanged);

    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::themeTypeChanged,
            this,
            &DownloadHeaderView::onPalettetypechanged);

    m_headerCbx->setFixedSize(20, 20);
    m_headerCbx->setVisible(true);
    //setSectionResizeMode(QHeaderView::ResizeToContents); // 设置resize模式自适应，不能由程序和用户更改

    if (DGuiApplicationHelper::instance()->themeType() == 2) {
        onPalettetypechanged(DGuiApplicationHelper::ColorType::DarkType);
    } else {
        onPalettetypechanged(DGuiApplicationHelper::ColorType::LightType);
    }
    setSortIndicatorShown(true);
    setSectionsClickable(true);
}

DownloadHeaderView::~DownloadHeaderView()
{
    delete (m_headerCbx);
}

void DownloadHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter, rect, logicalIndex);

    // BUG-243731: 列被拖窄时表头文字默认被裁剪（不省略），改为按可用宽度省略绘制，
    // 使窄列显示干净的省略号而非半截文字。
    const QAbstractItemModel *m = model();
    if (!m)
        return;
    const QString text = m->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    if (text.isEmpty())
        return;

    const int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);
    int rightReserve = margin;
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
        rightReserve += style()->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, this) + margin;

    const QRect textRect = rect.adjusted(margin, 0, -rightReserve, 0);
    if (textRect.width() <= 0)
        return;

    const QString elided = fontMetrics().elidedText(text, Qt::ElideRight, textRect.width());
    if (elided == text)
        return; // 文字完整放得下，基类已正确绘制，无需重绘

    QStyleOptionHeader opt;
    opt.initFrom(this);
    opt.text = elided;
    opt.rect = textRect;
    opt.textAlignment = defaultAlignment();
    style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);
}

void DownloadHeaderView::updateGeometries()
{
    m_headerCbx->resize(this->height()-2, this->height()-2);
    m_headerCbx->move(sectionPosition(0) + 5, 1);
    QHeaderView::updateGeometries();
}

void DownloadHeaderView::onHeaderChecked(bool checked)
{
    m_headerCbx->setChecked(checked);
}

void DownloadHeaderView::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    Q_UNUSED(type);
    QPalette p;

    if (DGuiApplicationHelper::instance()->themeType() == 2) {
        p.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base());
    } else {
        p.setColor(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base().color());
    }
    setPalette(p);
}
