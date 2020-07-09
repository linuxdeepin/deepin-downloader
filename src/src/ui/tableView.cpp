/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tableView.cpp
 *
 * @brief 下载条目列表
 *
 * @date 2020-06-09 09:56
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

#include "tableView.h"

#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHeaderView>
#include <QModelIndex>
#include <QJsonObject>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QDesktopServices>
#include <QStandardItemModel>

#include "../database/dbinstance.h"
#include "global.h"
#include "../aria2/aria2rpcinterface.h"
#include "tableModel.h"
#include "headerView.h"
#include "itemDelegate.h"
#include "settings.h"
#include "topButton.h"
#include "tabledatacontrol.h"
#include "global.h"

using namespace Global;

TableView::TableView(int Flag, TopButton *pToolBar)
    : QTableView()
    , m_iTableFlag(Flag)
    , m_pTableModel(new TableModel(Flag))
    , m_ptableDataControl(new tableDataControl(this))
    ,m_pItemdegegate(new ItemDelegate(this, m_iTableFlag))
    , m_pSetting(Settings::getInstance())
    , m_pToolBar(pToolBar)
{
    initUI();
    initConnections();
}

void TableView::initUI()
{
    setModel(m_pTableModel);
    setItemDelegate(m_pItemdegegate);
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(636);
    setMouseTracking(true);
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    verticalHeader()->hide();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::SelectedClicked);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setShowGrid(false);

    setSelectionMode(QAbstractItemView::SingleSelection);

    m_pHeaderView = new  HeaderView(Qt::Horizontal, this);
    setHorizontalHeader(m_pHeaderView);
    //m_pHeaderView->setDefaultSectionSize(20);
    //m_pHeaderView->setSortIndicatorShown(false);
//    m_pHeaderView->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
//    m_pHeaderView->setSectionResizeMode(0, QHeaderView::Interactive);
//    m_pHeaderView->setSectionResizeMode(1, QHeaderView::Interactive);
//    m_pHeaderView->setSectionResizeMode(2, QHeaderView::Interactive);
//    m_pHeaderView->setSectionResizeMode(3, QHeaderView::Interactive);
//    m_pHeaderView->setSectionResizeMode(4, QHeaderView::Interactive);
//    m_pHeaderView->setTextElideMode(Qt::ElideMiddle);
//    m_pHeaderView->setFixedHeight(36);
    setColumnWidth(0, 20);
    setColumnWidth(1, 260);
    setColumnWidth(2, 110);
    setColumnWidth(3, 200);
    setColumnWidth(4, 200);
}

void TableView::initConnections()
{
    connect(m_pHeaderView, &HeaderView::getStatechanged, this, &TableView::signalHeaderStatechanged);
    connect(this, &TableView::signalClearHeaderCheck, m_pHeaderView, &HeaderView::getClearHeaderCheck);
    connect(m_pTableModel, &TableModel::tableviewAllcheckedOrAllunchecked, this, &TableView::signalTableViewAllChecked);
    connect(this, &TableView::signalTableViewAllChecked, m_pHeaderView, &HeaderView::getCheckall);
    connect(this, &TableView::signalHoverchanged, m_pItemdegegate, &ItemDelegate::onHoverchanged);
}

void TableView::initTableView()
{
}

void TableView::reset(bool switched)
{
    QModelIndex idx = this->selectionModel()->currentIndex();
    int size = QTableView::verticalScrollBar()->value();

    QTableView::reset();

    this->selectRow(idx.row());
    if(switched) {
        size = 0;
    }
    QTableView::verticalScrollBar()->setValue(size);
}

void TableView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        setCurrentIndex(QModelIndex());
        QTableView::mousePressEvent(event);
    }
}

TableModel * TableView::getTableModel()
{
    return m_pTableModel;
}

tableDataControl *TableView::getTableControl()
{
    return m_ptableDataControl;
}

HeaderView *TableView::getTableHeader()
{
    return m_pHeaderView;
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = this->indexAt(event->pos());
    emit signalHoverchanged(idx);
}

void TableView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    this->reset();
    emit signalHoverchanged(QModelIndex());
}

void TableView::keyPressEvent(QKeyEvent *event)
{
    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
        return;
    }
    QWidget::keyPressEvent(event);
}

void TableView::refreshTableView(const int &index)
{
    switch(index) {
    case 0: {
        //if(1 == getTableModel()->getTablemodelMode()){
            getTableModel()->switchDownloadingMode();
        //}
        setColumnHidden(3, false);
        setColumnHidden(4, true);

        // 联动工具栏按钮 begin
        int chkedCnt = 0;
        QList<DataItem *> selectList = getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); i++) {
            if(selectList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
        if(chkedCnt > 0) {
            m_pToolBar->enableStartBtn(true);
            m_pToolBar->enablePauseBtn(true);
            m_pToolBar->enableDeleteBtn(true);
        } else {
            m_pToolBar->enableStartBtn(false);
            m_pToolBar->enablePauseBtn(false);
            m_pToolBar->enableDeleteBtn(false);
        }
        break;
    }

    case 1:
        //if(0 == getTableModel()->getTablemodelMode()){
            getTableModel()->switchFinishedMode();
        //}
        setColumnHidden(3, true);
        setColumnHidden(4, false);
        break;
    }


    update();
}


