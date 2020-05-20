/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author: zhanglei <zhanglei_hlj@nfschina.com>
*
* Maintainer: zhanglei <zhanglei_hlj@nfschina.com>
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

#include "headerview.h"
#include <QDebug>
HeaderView::HeaderView(Qt::Orientation orientation, QWidget * parent)
        : QHeaderView(orientation, parent)
{
   m_headerCbx = new DCheckBox(this);
   //connect(m_headerCbx,&DCheckBox::stateChanged,this,&HeaderView::get_stateChanged);
   connect(m_headerCbx,&DCheckBox::clicked,this,&HeaderView::get_stateChanged);
   connect(DGuiApplicationHelper::instance(),&DGuiApplicationHelper::paletteTypeChanged,this,&HeaderView::get_paletteTypeChanged);

   m_headerCbx->setFixedSize(25,25);
   m_headerCbx->setVisible(true);
   this->setSectionResizeMode(QHeaderView::ResizeToContents);
   if(DGuiApplicationHelper::instance()->themeType()==2)
       get_paletteTypeChanged(DGuiApplicationHelper::ColorType::DarkType);
   else {
       get_paletteTypeChanged(DGuiApplicationHelper::ColorType::LightType);

   }
}

void HeaderView::updateGeometries() {
    m_headerCbx->move(sectionPosition(0) + 5, 5);
}
void HeaderView::get_clear_header_check()
{
    m_headerCbx->setChecked(false);
}


void HeaderView::get_checkall_signals(bool checked)
{
     m_headerCbx->setChecked(checked);
}
void HeaderView::get_clickedChanged(bool checked)
{
}
void HeaderView::get_paletteTypeChanged(DGuiApplicationHelper::ColorType type)
{
    QPalette p;

    if(DGuiApplicationHelper::instance()->themeType()==2)
    {
        p.setBrush(QPalette::Base,DGuiApplicationHelper::instance()->applicationPalette().base());

    }
    else {
        p.setColor(QPalette::Base,QColor(255,255,255));

    }
    this->setPalette(p);
}

