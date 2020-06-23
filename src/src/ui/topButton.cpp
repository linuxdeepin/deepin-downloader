/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file topButton.cpp
 *
 * @brief 主界面顶部按钮加搜索框
 *
 * @date 2020-06-09 10:00
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

#include "topButton.h"
#include <QDebug>
#include <DToolTip>
#include <QTimer>
#include <QCursor>

DWIDGET_USE_NAMESPACE

TopButton::TopButton(QWidget *parent) : QWidget(parent)
{
    Init();
    InitConnections();
}

void TopButton::Init()
{
    QHBoxLayout *mainHlayout = new QHBoxLayout(this);

    mainHlayout->setContentsMargins(0, 6, 0, 10);
    mainHlayout->setSpacing(10);
    m_pIconLable = new DLabel;
    QIcon logo_icon = QIcon(":icons/icon/downloader5.svg");
    m_pIconLable->setPixmap(logo_icon.pixmap(32, 32));
    m_pIconLable->setFixedSize(36, 36);
    m_pSearchEdit = new DSearchEdit();
    m_pSearchEdit->setMinimumWidth(350);
    m_pSearchEdit->setFixedHeight(36);


    // searchEdit->setFixedSize(350,36);
    m_pNewDownloadBtn = new DIconButton(this);
    m_pNewDownloadBtn->setFixedSize(36, 36);
    m_pNewDownloadBtn->setIcon(QIcon::fromTheme("dcc_newdownload"));
    m_pNewDownloadBtn->setToolTip(tr("Create"));

    m_pPauseDownloadBtn = new Dtk::Widget::DIconButton(this);

    // pauseDownloadBtn->setFixedSize(36,36);

    m_pPauseDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_pause"));
    m_pPauseDownloadBtn->setEnabled(false);
    m_pPauseDownloadBtn->setGeometry(90, 0, 36, 36);
    m_pPauseDownloadBtn->setToolTip(tr("Pause"));

    m_pStartDownloadBtn = new DIconButton(this);
    m_pStartDownloadBtn->setFixedSize(36, 36);
    m_pStartDownloadBtn->setIcon(QIcon::fromTheme("dcc_icon_start"));
    m_pStartDownloadBtn->setEnabled(false);
    m_pStartDownloadBtn->setToolTip(tr("Resume"));


    m_pDeleteDownloadBtn = new DIconButton(this);
    m_pDeleteDownloadBtn->setFixedSize(36, 36);
    m_pDeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));
    m_pDeleteDownloadBtn->setEnabled(false);
    m_pDeleteDownloadBtn->setToolTip(tr("Delete"));

    mainHlayout->addSpacing(5);
    mainHlayout->addWidget(m_pIconLable);
    mainHlayout->addSpacing(7);
    mainHlayout->addWidget( m_pPauseDownloadBtn);
    mainHlayout->addWidget( m_pStartDownloadBtn);

    mainHlayout->addWidget(m_pDeleteDownloadBtn);
    mainHlayout->addWidget(   m_pNewDownloadBtn);

    mainHlayout->addWidget(       m_pSearchEdit);

    // mainHlayout->addStretch();
    qDebug() << "asdwasdw";
}

void TopButton::InitConnections()
{
    connect(   m_pNewDownloadBtn, &DIconButton::clicked,      this, &TopButton::newDownloadBtnClicked);
    connect( m_pPauseDownloadBtn, &DIconButton::clicked,      this, &TopButton::pauseDownloadBtnClicked);
    connect( m_pStartDownloadBtn, &DIconButton::clicked,      this, &TopButton::startDownloadBtnClicked);
    connect(m_pDeleteDownloadBtn, &DIconButton::clicked,      this, &TopButton::deleteDownloadBtnClicked);
    connect(       m_pSearchEdit, &DSearchEdit::focusChanged, this, &TopButton::getSearchEditFocus);
    connect(       m_pSearchEdit, &DSearchEdit::textChanged,  this, &TopButton::getSearchEditTextChange);
}

void TopButton::getTableChanged(int index)
{
    if((index == 1) || (index == 2)) {
        m_pStartDownloadBtn->setEnabled(false);
        m_pPauseDownloadBtn->setEnabled(false);
        m_pDeleteDownloadBtn->setEnabled(false);
        if(index == 2) {
            m_pDeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_recycel_delete"));
        } else   {
            m_pDeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));
        }
    } else   {
        m_pStartDownloadBtn->setEnabled(false);
        m_pPauseDownloadBtn->setEnabled(false);
        m_pDeleteDownloadBtn->setEnabled(false);
        m_pDeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));
    }
}


//DownloadManagerBtn::DownloadManagerBtn(QWidget *parent)
//{

//}

//void DownloadManagerBtn::mouseMoveEvent(QMouseEvent *event)
//{
//    m_pHoverTimer = new QTimer(this);
//    m_pHoverTimer->start(1000);
//    connect(m_pHoverTimer, &QTimer::timeout, this, &DownloadManagerBtn::onTimeOut);
//}

//void DownloadManagerBtn::onTimeOut()
//{
//    if(geometry().contains(this->mapFromGlobal(QCursor::pos()))){
//        DToolTip* pTip = new DToolTip("Stop");
//        pTip->show(QCursor::pos(), 1000);
//    }
//}
