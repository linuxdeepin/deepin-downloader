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
    , m_pSetting(Settings::getInstance())
    , m_pToolBar(pToolBar)
{
    m_ptableDataControl = new tableDataControl(this);
    initUI();
}

void TableView::initUI()
{

//    QStandardItemModel *pModel = new QStandardItemModel;
//    setModel(pModel);
//    pModel->setItem(0, 0, new QStandardItem(false));
//    pModel->setItem(0, 1, new QStandardItem("3"));
//    pModel->setItem(0, 2, new QStandardItem("男"));
//    pModel->setItem(0, 3, new QStandardItem(5));
//    pModel->setItem(0, 4, new QStandardItem(5));

//    pModel->setItem(1, 0, new QStandardItem("张三"));
//    pModel->setItem(1, 1, new QStandardItem("3"));
//    pModel->setItem(1, 2, new QStandardItem("男"));
//    pModel->setItem(1, 3, new QStandardItem(4));
//    pModel->setItem(1, 4, new QStandardItem(5));


//    pModel->setItem(2, 0, new QStandardItem(false));
//    pModel->setItem(2, 1, new QStandardItem("3"));
//    pModel->setItem(2, 2, new QStandardItem("男"));
//    pModel->setItem(2, 3, new QStandardItem(3));
//    pModel->setItem(2, 4, new QStandardItem(5));

//    pModel->setItem(3, 0, new QStandardItem(false));
//    pModel->setItem(3, 1, new QStandardItem("3"));
//    pModel->setItem(3, 2, new QStandardItem("男"));
//    pModel->setItem(3, 3, new QStandardItem(2));
//    pModel->setItem(3, 4, new QStandardItem(5));

//    pModel->setItem(4, 0, new QStandardItem(false));
//    pModel->setItem(4, 1, new QStandardItem("3"));
//    pModel->setItem(4, 2, new QStandardItem("男"));
//    pModel->setItem(4, 3, new QStandardItem(2));
//    pModel->setItem(4, 4, new QStandardItem(5));

    setModel(m_pTableModel);
    m_pItemdegegate = new ItemDelegate(this, m_iTableFlag);
    setItemDelegate(m_pItemdegegate);
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(636);
    setMouseTracking(true);
    setSortingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    verticalHeader()->hide();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked
                                      | QAbstractItemView::SelectedClicked);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setShowGrid(false);

    setSelectionMode(QAbstractItemView::SingleSelection);

    HeaderView *pHeaderView = new  HeaderView(Qt::Horizontal, this);
    setHorizontalHeader(pHeaderView);
    pHeaderView->setDefaultSectionSize(20);
    //pHeaderView->setSortIndicatorShown(false);
    pHeaderView->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    pHeaderView->setSectionResizeMode(0, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(1, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(2, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(3, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(4, QHeaderView::Stretch);
    pHeaderView->setTextElideMode(Qt::ElideMiddle);
    pHeaderView->setFixedHeight(36);
    setColumnWidth(0, 20);

    connect(pHeaderView, &HeaderView::getStatechanged, this, &TableView::signalHeaderStatechanged);
    connect(this, &TableView::signalClearHeaderCheck, pHeaderView, &HeaderView::getClearHeaderCheck);
    connect(m_pTableModel, &TableModel::tableviewAllcheckedOrAllunchecked, this, &TableView::signalTableViewAllChecked);
    connect(this, &TableView::signalTableViewAllChecked, pHeaderView, &HeaderView::getCheckall);
    connect(this, &TableView::signalHoverchanged, m_pItemdegegate, &ItemDelegate::onHoverchanged);

}

void TableView::initConnections()
{
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
    int num = m_pTableModel->rowCount();
    if(event->button() == Qt::LeftButton) {
        setCurrentIndex(QModelIndex());
        QTableView::mousePressEvent(event);
        QModelIndex index = currentIndex();
        if((index.row() < 0) && (index.column() < 0)) {
            return;
        }
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

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = this->indexAt(event->pos());
    emit signalHoverchanged(idx);
}

void TableView::leaveEvent(QEvent *event)
{
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


