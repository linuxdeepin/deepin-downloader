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

#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <DCheckBox>
#include <DApplication>
#include <dguiapplicationhelper.h>

DWIDGET_USE_NAMESPACE

class HeaderView: public QHeaderView
{
    Q_OBJECT
public:
    explicit HeaderView(Qt::Orientation orientation, QWidget * parent = 0);

protected:
    void updateGeometries() ;

private:
   DCheckBox *m_headerCbx;
signals:
   void get_stateChanged(bool checked);
public slots:
   void get_clear_header_check();
   void get_checkall_signals(bool checked);
   void get_clickedChanged(bool);
   void get_paletteTypeChanged(DGuiApplicationHelper::ColorType type);




};

#endif // HEADERVIEW_H
