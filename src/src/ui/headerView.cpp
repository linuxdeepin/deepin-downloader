/**
* @file headerView.cpp
* @brief 表头
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:44
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "headerView.h"
#include <QDebug>
HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    m_headerCbx = new DCheckBox(this);
    //connect(m_headerCbx,&DCheckBox::stateChanged,this,&HeaderView::get_stateChanged);
    connect(m_headerCbx, &DCheckBox::clicked, this, &HeaderView::get_stateChanged);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged, this, &HeaderView::get_paletteTypeChanged);

    m_headerCbx->setFixedSize(25, 25);
    m_headerCbx->setVisible(true);
    this->setSectionResizeMode(QHeaderView::ResizeToContents);
    if (DGuiApplicationHelper::instance()->themeType() == 2)
        get_paletteTypeChanged(DGuiApplicationHelper::ColorType::DarkType);
    else {
        get_paletteTypeChanged(DGuiApplicationHelper::ColorType::LightType);
    }
}

void HeaderView::updateGeometries()
{
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
void HeaderView::get_paletteTypeChanged(DGuiApplicationHelper::ColorType type)
{
    QPalette p;

    if (DGuiApplicationHelper::instance()->themeType() == 2) {
        p.setBrush(QPalette::Base, DGuiApplicationHelper::instance()->applicationPalette().base());

    } else {
        p.setColor(QPalette::Base, QColor(255, 255, 255));
    }
    this->setPalette(p);
}
