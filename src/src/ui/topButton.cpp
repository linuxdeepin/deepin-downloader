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
#include <QMouseEvent>

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
    m_IconLable = new DLabel;
    QIcon logo_icon = QIcon(":icons/icon/downloader5.svg");
    m_IconLable->setPixmap(logo_icon.pixmap(32, 32));
    m_IconLable->setFixedSize(36, 36);
    m_SearchEdit = new DSearchEdit();
    m_SearchEdit->setMinimumWidth(350);
    m_SearchEdit->setFixedHeight(36);
    m_SearchEdit->lineEdit()->setMaxLength(256);

    // searchEdit->setFixedSize(350,36);
    m_NewDownloadBtn = new DIconButton(this);
    m_NewDownloadBtn->setFixedSize(36, 36);
    m_NewDownloadBtn->setIcon(QIcon::fromTheme("dcc_newdownload"));
    m_NewDownloadBtn->setToolTip(tr("New task"));

    m_PauseDownloadBtn = new DIconButton(this);
    m_PauseDownloadBtn->setFixedSize(36, 36);
    m_PauseDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_pause"));
    m_PauseDownloadBtn->setEnabled(false);
    m_PauseDownloadBtn->setToolTip(tr("Pause"));

    m_StartDownloadBtn = new DIconButton(this);
    m_StartDownloadBtn->setFixedSize(36, 36);
    m_StartDownloadBtn->setIcon(QIcon::fromTheme("dcc_icon_start"));
    m_StartDownloadBtn->setEnabled(false);
    m_StartDownloadBtn->setToolTip(tr("Resume"));

    m_DeleteDownloadBtn = new DIconButton(this);
    m_DeleteDownloadBtn->setFixedSize(36, 36);
    m_DeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));
    m_DeleteDownloadBtn->setEnabled(false);
    m_DeleteDownloadBtn->setToolTip(tr("Delete"));

    mainHlayout->addSpacing(5);
    mainHlayout->addWidget(m_IconLable);
    mainHlayout->addSpacing(7);
    mainHlayout->addWidget( m_PauseDownloadBtn);
    mainHlayout->addWidget( m_StartDownloadBtn);

    mainHlayout->addWidget(m_DeleteDownloadBtn);
    mainHlayout->addWidget(   m_NewDownloadBtn);

    mainHlayout->addWidget(       m_SearchEdit);

    // mainHlayout->addStretch();
    qDebug() << "asdwasdw";
}

void TopButton::InitConnections()
{
    connect(   m_NewDownloadBtn, &DIconButton::clicked,      this, &TopButton::newDownloadBtnClicked);
    connect( m_PauseDownloadBtn, &DIconButton::clicked,      this, &TopButton::pauseDownloadBtnClicked);
    connect( m_StartDownloadBtn, &DIconButton::clicked,      this, &TopButton::startDownloadBtnClicked);
    connect(m_DeleteDownloadBtn, &DIconButton::clicked,      this, &TopButton::deleteDownloadBtnClicked);
    connect(       m_SearchEdit, &DSearchEdit::focusChanged, this, &TopButton::SearchEditFocus);
    connect(       m_SearchEdit, &DSearchEdit::textChanged,  this, &TopButton::SearchEditTextChange);
}

void TopButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        return;
    }
    QWidget::mousePressEvent(event);
}

void TopButton::onTableChanged(int index)
{
    if(index == 2) {
        m_StartDownloadBtn->setIcon(QIcon::fromTheme("dcc_recycel_delete"));
        m_PauseDownloadBtn->setIcon(QIcon::fromTheme("dcc_recycel_restore"));
        m_DeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));

        m_StartDownloadBtn->setToolTip(tr("Empty"));
        m_PauseDownloadBtn->setToolTip(tr("Restore"));
        m_DeleteDownloadBtn->setToolTip(tr("Delete"));
    } else if(index == 1)   {
        m_StartDownloadBtn->setIcon(QIcon::fromTheme("dcc_finish_openfolder"));
        m_PauseDownloadBtn->setIcon(QIcon::fromTheme("dcc_finish_openfile"));
        m_DeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));

        m_StartDownloadBtn->setToolTip(tr("Open Folder"));
        m_PauseDownloadBtn->setToolTip(tr("Open"));
        m_DeleteDownloadBtn->setToolTip(tr("Delete"));
    } else   {
        m_StartDownloadBtn->setIcon(QIcon::fromTheme("dcc_icon_start"));
        m_PauseDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_pause"));
        m_DeleteDownloadBtn->setIcon(QIcon::fromTheme("dcc_list_icon_delete"));

        m_StartDownloadBtn->setToolTip(tr("Resume"));
        m_PauseDownloadBtn->setToolTip(tr("Pause"));
        m_DeleteDownloadBtn->setToolTip(tr("Delete"));

        m_StartDownloadBtn->setEnabled(false);
        m_PauseDownloadBtn->setEnabled(false);
        m_DeleteDownloadBtn->setEnabled(false);
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
//    if(geometry().contains(mapFromGlobal(QCursor::pos()))){
//        DToolTip* pTip = new DToolTip("Stop");
//        pTip->show(QCursor::pos(), 1000);
//    }
//}
