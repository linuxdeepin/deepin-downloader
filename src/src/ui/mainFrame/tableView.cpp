// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
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

TableView::TableView(int Flag)
    : QTableView()
    , m_TableFlag(Flag)
    , m_TableModel(new TableModel(Flag))
    , m_TableDataControl(new TableDataControl(this))
    , m_Itemdegegate(new ItemDelegate(this, m_TableFlag))
    , m_Setting(Settings::getInstance())
{
    qDebug() << "TableView created with flag:" << Flag;
    initUI();
    initConnections();
    qDebug() << "[TableView] Constructor ended";
}

TableView::~TableView()
{
    // qDebug() << "TableView destroyed";
    delete (m_TableModel);
    delete (m_TableDataControl);
    delete (m_HeaderView);
}

void TableView::initUI()
{
    qDebug() << "Initializing table view UI";
    setModel(m_TableModel);
    setItemDelegate(m_Itemdegegate);
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

    m_HeaderView = new DownloadHeaderView(Qt::Horizontal, this);
    setHorizontalHeader(m_HeaderView);
    m_HeaderView->setStretchLastSection(true);
    m_HeaderView->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    setColumnWidth(0, 30);
    setColumnWidth(1, 248);
    m_HeaderView->setSectionResizeMode(1, QHeaderView::Interactive);
    m_HeaderView->setSectionResizeMode(0, QHeaderView::Fixed);
    setColumnWidth(2, 110);
    setColumnWidth(3, QHeaderView::Interactive);
    setColumnWidth(4, QHeaderView::Interactive);
    setTabKeyNavigation(true);
    QFont font;
    font.setFamily("Source Han Sans");
    setFont(font);
    qDebug() << "[TableView] initUI function ended";
}

void TableView::initConnections()
{
    qDebug() << "Initializing table view connections";
    connect(m_HeaderView, &DownloadHeaderView::Statechanged, this, &TableView::HeaderStatechanged);
    //connect(this, &TableView::ClearHeaderCheck, m_HeaderView, &HeaderView::onClearHeaderChecked);
    connect(m_TableModel, &TableModel::tableviewAllcheckedOrAllunchecked, this, &TableView::isCheckHeader);
    connect(this, &TableView::isCheckHeader, m_HeaderView, &DownloadHeaderView::onHeaderChecked);
    connect(this, &TableView::Hoverchanged, m_Itemdegegate, &ItemDelegate::onHoverchanged);
    connect(m_TableModel, &TableModel::layoutChanged, this, &TableView::onModellayoutChanged);
    qDebug() << "[TableView] initConnections function ended";
}

void TableView::onListchanged()
{
    qDebug() << "[TableView] onListchanged function started";
    currentChanged(m_PreviousIndex.sibling(m_PreviousIndex.row(), 0), m_PreviousIndex);
}

void TableView::reset(bool switched)
{
    // qDebug() << "[TableView] reset function started with switched:" << switched;
    int size = QTableView::verticalScrollBar()->value();

    QTableView::reset();

    //selectRow(idx.row());
    if (switched) {
        size = 0;
    }
    QTableView::verticalScrollBar()->setValue(size);
    // qDebug() << "[TableView] reset function ended";
}

TableModel *TableView::getTableModel()
{
    // qDebug() << "[TableView] getTableModel function started";
    return m_TableModel;
}

TableDataControl *TableView::getTableControl()
{
    // qDebug() << "[TableView] getTableControl function started";
    return m_TableDataControl;
}

DownloadHeaderView *TableView::getTableHeader()
{
    // qDebug() << "[TableView] getTableHeader function started";
    return m_HeaderView;
}

void TableView::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "[TableView] mousePressEvent function started";
    if (event->button() == Qt::LeftButton) {
        //setCurrentIndex(QModelIndex());
        QTableView::mousePressEvent(event);
        QModelIndex index = indexAt(event->pos());
        this->reset();
        if ((index.row() < 0) && (index.column() < 0)) {
            currentChanged(m_PreviousIndex.sibling(m_PreviousIndex.row(), 0), m_PreviousIndex);
            //return;
        } else {
            m_PreviousIndex = index;
        }
    }
    // qDebug() << "[TableView] mousePressEvent function ended";
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "[TableView] mouseMoveEvent function started";
    QModelIndex idx = indexAt(event->pos());
    emit Hoverchanged(idx);
    // qDebug() << "[TableView] mouseMoveEvent function ended";
}

void TableView::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "[TableView] mouseReleaseEvent function started";
    QModelIndex index = indexAt(event->pos());

    if ((index.row() < 0) && (index.column() < 0)) {
        emit HeaderStatechanged(false);
        emit isCheckHeader(false);
        //return;
    }
    //reset();
    QTableView::mouseReleaseEvent(event);
    // qDebug() << "[TableView] mouseReleaseEvent function ended";
}

void TableView::leaveEvent(QEvent *event)
{
    // qDebug() << "[TableView] leaveEvent function started";
    Q_UNUSED(event);
    //reset();
    // currentChanged(m_PreviousIndex.sibling(m_PreviousIndex.row(), 0), m_PreviousIndex);
    emit Hoverchanged(QModelIndex());
    // qDebug() << "[TableView] leaveEvent function ended";
}

void TableView::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "[TableView] keyPressEvent function started";
    if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
        // qDebug() << "[TableView] Ctrl+C pressed, returning";
        return;
    }
    //    if(event->key() == Qt::Key_Down) {
    //        currentChanged(m_PreviousIndex.sibling(m_PreviousIndex.row() + 1, 0), m_PreviousIndex);
    //        setCurrentIndex(m_PreviousIndex.sibling(m_PreviousIndex.row() + 1, 0));
    //    }
    //    if(event->key() == Qt::Key_Up) {
    //        currentChanged(m_PreviousIndex.sibling(m_PreviousIndex.row() - 1, 0), m_PreviousIndex);
    //        setCurrentIndex(m_PreviousIndex.sibling(m_PreviousIndex.row() - 1, 0));
    //    }
    QWidget::keyPressEvent(event);
    // qDebug() << "[TableView] keyPressEvent function ended";
}

void TableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    // qDebug() << "[TableView] currentChanged function started";
    QTableView::currentChanged(current, previous);
    // qDebug() << "[TableView] currentChanged function ended";
}

bool TableView::refreshTableView(const int &index)
{
    // qDebug() << "[TableView] refreshTableView function started with index:" << index;
    if (index > 1) {
        // qDebug() << "[TableView] refreshTableView function ended with result: false (index > 1)";
        return false;
    }
    switch (index) {
    case 0: {
        // qDebug() << "[TableView] Switching to downloading mode";
        //if(1 == getTableModel()->getTablemodelMode()){
        getTableModel()->switchDownloadingMode();
        //}
        setColumnHidden(3, false);
        setColumnHidden(4, true);
        break;
    }

    case 1:
        // qDebug() << "[TableView] Switching to finished mode";
        //if(0 == getTableModel()->getTablemodelMode()){
        getTableModel()->switchFinishedMode();
        //}
        setColumnHidden(3, true);
        setColumnHidden(4, false);
        //getTableHeader()->setSortIndicator(4, Qt::AscendingOrder);
        break;
    }
#if QT_VERSION_MAJOR > 5
    resizeColumnToContents(4);
#endif
    update();
    // qDebug() << "[TableView] refreshTableView function ended with result: true";
    return true;
}

void TableView::onModellayoutChanged()
{
    // qDebug() << "[TableView] onModellayoutChanged function started";
    if (m_TableFlag == 0) {
        qDebug() << "[TableView] Processing download data items";
        const QList<DownloadDataItem *> &selectList = getTableModel()->renderList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.at(i)->isHide) {
                setRowHidden(i, true);
            } else {
                setRowHidden(i, false);
            }
        }
    } else {
        // qDebug() << "[TableView] Processing delete data items";
        const QList<DeleteDataItem *> &selectList = getTableModel()->recyleList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.at(i)->isHide) {
                setRowHidden(i, true);
            } else {
                setRowHidden(i, false);
            }
        }
    }
    // qDebug() << "[TableView] onModellayoutChanged function ended";
}

LeftListView::LeftListView()
{
    // qDebug() << "[LeftListView] Constructor started";
}

void LeftListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    // qDebug() << "[LeftListView] currentChanged function started";
    Q_UNUSED(previous);
    emit currentIndexChanged(current);
    // qDebug() << "[LeftListView] currentChanged function ended";
}

void LeftListView::paintEvent(QPaintEvent *e)
{
    // qDebug() << "[LeftListView] paintEvent function started";
    DPalette pa;
#if QT_VERSION_MAJOR > 5
    pa = this->palette();
    pa.setBrush(DPalette::ItemBackground, pa.brush(DPalette::Base));
    this->setPalette(pa);
#else
    pa = DApplicationHelper::instance()->palette(this);
    pa.setBrush(DPalette::ItemBackground, pa.brush(DPalette::Base));
    DApplicationHelper::instance()->setPalette(this, pa);
#endif
    DListView::paintEvent(e);
    // qDebug() << "[LeftListView] paintEvent function ended";
}
