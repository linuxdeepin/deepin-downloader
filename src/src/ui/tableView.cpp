

#include "tableView.h"
#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHeaderView>
#include <QModelIndex>

TableView::TableView(int Flag)
    :QTableView ()
{
    m_iTableFlag=Flag;
    initUI();
    setTestData();
}
void TableView::initUI()
{
    //this->setLineWidth(0);
    this->setFrameShape(QFrame::NoFrame);
    this->setMinimumWidth(636);
    this->setMouseTracking(true);

    //this->setMaximumWidth(2000);
    this->verticalHeader()->hide();
    //this->verticalHeader()->setDefaultSectionSize(56);


    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    //setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setShowGrid(false);

    setSelectionMode(QAbstractItemView::SingleSelection);


}
void TableView::initConnections()
{

}
void TableView::initTableView()
{

}

void TableView::setTestData()
{

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
