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

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    m_headerCbx = new QCheckBox(this);

    // connect(m_headerCbx,&DCheckBox::stateChanged,this,&HeaderView::get_stateChanged);
    connect(m_headerCbx,
            &DCheckBox::clicked,
            this,
            &HeaderView::getStatechanged);
    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::paletteTypeChanged,
            this,
            &HeaderView::getPalettetypechanged);

    connect(DGuiApplicationHelper::instance(),
            &DGuiApplicationHelper::themeTypeChanged,
            this,
            &HeaderView::getPalettetypechanged);

    m_headerCbx->setFixedSize(25, 25);
    m_headerCbx->setVisible(true);
    this->setSectionResizeMode(QHeaderView::ResizeToContents); // 设置resize模式自适应，不能由程序和用户更改

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        getPalettetypechanged(DGuiApplicationHelper::ColorType::DarkType);
    } else {
        getPalettetypechanged(DGuiApplicationHelper::ColorType::LightType);
    }
    setSortIndicatorShown(true);
    setSectionsClickable(true);
}

void HeaderView::updateGeometries()
{
    m_headerCbx->move(sectionPosition(0) + 5, 5);
}

void HeaderView::getClearHeaderCheck()
{
    m_headerCbx->setChecked(false);
}

void HeaderView::getCheckall(bool checked)
{
    m_headerCbx->setChecked(checked);
}

void HeaderView::getPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    QPalette p;

    if(DGuiApplicationHelper::instance()->themeType() == 2) {
        p.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base());
    } else {
        p.setColor(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base().color());
    }
    this->setPalette(p);
}
