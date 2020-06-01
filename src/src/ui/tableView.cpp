/**
* @file tableView.cpp
* @brief 下载条目列表
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:41
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "tableView.h"
#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHeaderView>
#include <QModelIndex>
#include "tableModel.h"
#include "headerView.h"
#include "itemDelegate.h"

TableView::TableView(int Flag)
    :QTableView (), m_pTableModel(new TableModel(Flag))
{
    m_iTableFlag=Flag;
    initUI();
    //setTestData();
}
void TableView::initUI()
{
    setModel(m_pTableModel);
    m_pItemdegegate= new ItemDelegate(this,Table_Flag);
    setItemDelegate(m_pItemdegegate);
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(636);
    setMouseTracking(true);

    //this->setMaximumWidth(2000);
    verticalHeader()->hide();
    //this->verticalHeader()->setDefaultSectionSize(56);


    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    //setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setShowGrid(false);

    setSelectionMode(QAbstractItemView::SingleSelection);

    HeaderView *pHeaderView = new  HeaderView(Qt::Horizontal,this);
    setHorizontalHeader(pHeaderView);
    pHeaderView->setDefaultSectionSize(20);
    pHeaderView->setSortIndicatorShown(false);
    pHeaderView->setDefaultAlignment( Qt::AlignVCenter|Qt::AlignLeft);
    pHeaderView->setSectionResizeMode(0, QHeaderView::Fixed);
    pHeaderView->setSectionResizeMode(1, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(2, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(3, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(4, QHeaderView::Stretch);
    setColumnWidth(0, 20);

    connect(pHeaderView,&HeaderView::get_stateChanged,this,&TableView::header_stateChanged);
    connect(this,&TableView::clear_header_check,pHeaderView,&HeaderView::get_clear_header_check);
    connect(m_pTableModel,&TableModel::tableView_allChecked_or_allUnchecked,this,&TableView::get_tableview_allchecked);
    connect(this,&TableView::get_tableview_allchecked,pHeaderView,&HeaderView::get_checkall_signals);
    connect(this, &TableView::signal_hoverChanged, m_pItemdegegate, &ItemDelegate::slot_hoverChanged);

}
void TableView::initConnections()
{

}
void TableView::initTableView()
{

}

void TableView::setTestData()
{
    DataItem *data = new DataItem;
    data->taskId = "id";
    data->gid = "71bdc01777d598df";
    data->Ischecked = 0;
    data->fileName = "test.txt";
    data->taskId = "{8ffd889b-c0f9-4413-bf11-e98fe9ffc707}";
    data->createTime = "2020-05-22 15:39:14";
    data->Ischecked = false;
    getTableModel()->append(data);
    TableModel *dtModel = getTableModel();
    setRowHidden(dtModel->rowCount(QModelIndex()), true);
}




void TableView::reset(bool switched)
{
    QModelIndex idx= this->selectionModel()->currentIndex();
    int size=QTableView::verticalScrollBar()->value();
    QTableView::reset();

    this->selectRow(idx.row());
    if(switched)
        size=0;
    QTableView::verticalScrollBar()->setValue(size);
}

void TableView::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {

          setCurrentIndex(QModelIndex());
           QTableView::mousePressEvent(event);
           QModelIndex index=currentIndex();
           if(index.row()<0&&index.column()<0)
               return;
    }

}

TableModel* TableView::getTableModel()
{
    return m_pTableModel;
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = this->indexAt(event->pos());
    emit signal_hoverChanged(idx);
}

void TableView::leaveEvent(QEvent *event)
{
    this->reset();
    emit signal_hoverChanged(QModelIndex());
}
void TableView::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers()==Qt::ControlModifier&&event->key() == Qt::Key_C)
    {
        return;
    }
    QWidget::keyPressEvent(event);
}
