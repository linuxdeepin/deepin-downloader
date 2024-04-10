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
#include <QStyleOptionButton>

DownloadHeaderView::DownloadHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{

    connect(this, &DownloadHeaderView::sectionClicked, this, &DownloadHeaderView::onSectionClicked);
    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::paletteTypeChanged,
            this,
            &DownloadHeaderView::onPalettetypechanged);

    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::themeTypeChanged,
            this,
            &DownloadHeaderView::onPalettetypechanged);

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
}

void DownloadHeaderView::onHeaderChecked(bool checked)
{
    m_isChecked = checked;
    updateSection(0);
}

void DownloadHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{

    if (logicalIndex == 0) {
        painter->save();
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        QStyle::State state = QStyle::State_None;
        state |= QStyle::State_Enabled;
        opt.rect = rect;
        opt.section = logicalIndex;
        opt.state |= state;
        style()->drawControl(QStyle::CE_Header, &opt, painter, this);
        painter->restore();
        painter->save();
        QStyleOptionButton checkBoxStyle;
        checkBoxStyle.state = m_isChecked ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyle.state |= QStyle::State_Enabled;
        checkBoxStyle.rect = rect;
        checkBoxStyle.rect.setX(rect.x() + 5);
        checkBoxStyle.rect.setWidth(20);
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter);
        painter->restore();
        return;
    }
    QHeaderView::paintSection(painter, rect, logicalIndex);
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

void DownloadHeaderView::onSectionClicked(int logicalIndex)
{
    if (logicalIndex == 0) {
        m_isChecked = !m_isChecked;
        Q_EMIT Statechanged(m_isChecked);
    }
}
