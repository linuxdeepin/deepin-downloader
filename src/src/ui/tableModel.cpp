/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tableModel.cpp
 *
 * @brief 表格数据管理模块
 *
 * @date 2020-06-09 09:58
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

#include "tableModel.h"
#include <QDebug>
#include <QString>
#include <QStandardItemModel>

#include "global.h"
using namespace Global;

TableModel::TableModel(int Flag, QObject *parent)
    : QAbstractTableModel(parent)
    , m_iTableviewtabFlag(Flag)
    , m_iDownloadingCount(0)
{
    if(Flag == 0) {
        m_mode = Downloading;
    }
    connect(this, &TableModel::checkDatachange, this, &TableModel::getCheckDatachange);
    m_iSortColumn = 0;
    m_SortOrder = Qt::AscendingOrder;
}

TableModel::~TableModel()
{
}

void TableModel::getCheckDatachange(int flag)
{
    int check_num = 0;
    QList<DataItem *> active_list;
    QList<DataItem *> finish_list;

    if(flag == 0) {
        for(DataItem *item : m_dataList) {
            if(m_mode == Downloading) {
                if(item->status != Global::Status::Complete) {
                    active_list.append(item);
                }
            } else {
                if(item->status == Global::Status::Complete) {
                    finish_list.append(item);
                }
            }
        }
        if((m_mode == Downloading) && (active_list.size() > 0)) {
            for(DataItem *item :active_list) {
                if(item->Ischecked) {
                    check_num++;
                }
            }
            if(check_num == active_list.size()) {
                emit tableviewAllcheckedOrAllunchecked(true);
            } else {
                emit tableviewAllcheckedOrAllunchecked(false);
            }
        }
        if((m_mode == Finished) && (finish_list.size() > 0)) {
            for(DataItem *item :finish_list) {
                if(item->Ischecked) {
                    check_num++;
                }
            }
            if(check_num == finish_list.size()) {
                emit tableviewAllcheckedOrAllunchecked(true);
            } else {
                emit tableviewAllcheckedOrAllunchecked(false);
            }
        }
    } else {
        for(int i = 0; i < m_recyleList.size(); i++) {
            DelDataItem *del_data = m_recyleList.at(i);
            if(del_data->Ischecked) {
                check_num++;
            }
        }
        if(check_num == m_recyleList.size()) {
            emit tableviewAllcheckedOrAllunchecked(true);
        } else {
            emit tableviewAllcheckedOrAllunchecked(false);
        }
    }
}

DataItem * TableModel::find(const QString &gid)
{
    if(m_map.contains(gid)) {
        return m_map.value(gid);
    }
    return nullptr;
}

DelDataItem * TableModel::find(const QString &gid, int flag)
{
    if(m_Deletemap.contains(gid)) {
        return m_Deletemap.value(gid);
    }

    return nullptr;
}

void TableModel::append(DataItem *data)
{
    const int row = m_dataList.size();

    beginInsertRows(QModelIndex(), row, row);
    m_dataList.append(data);
    m_map.insert(data->taskId, data);
    endInsertRows();
}

void TableModel::append(DelDataItem *data)
{
    const int row = m_recyleList.size();

    beginInsertRows(QModelIndex(), row, row);
    m_recyleList.append(data);
    m_Deletemap.insert(data->taskId, data);
    endInsertRows();
}

void TableModel::removeItem(DataItem *data)
{
    if(m_map.contains(data->taskId)) {
        beginRemoveRows(QModelIndex(), m_dataList.indexOf(data), m_dataList.indexOf(data));
        m_map.remove(data->taskId);
        m_dataList.removeOne(data);
        m_renderList.removeOne(data);
        delete data;
        endRemoveRows();
    }
}

void TableModel::removeItem(DelDataItem *data)
{
    if(m_Deletemap.contains(data->taskId)) {
        beginRemoveRows(QModelIndex(), m_recyleList.indexOf(data), m_recyleList.indexOf(data));
        m_Deletemap.remove(data->taskId);
        m_recyleList.removeOne(data);
        delete data;
        endRemoveRows();
    }
}

void TableModel::removeItems()
{
    beginRemoveRows(QModelIndex(), 0, m_dataList.size());
    qDeleteAll(m_dataList.begin(), m_dataList.end());
    m_dataList.clear();
    m_map.clear();
    endRemoveRows();
}

void TableModel::removeItems(bool isrecycle)
{
    beginRemoveRows(QModelIndex(), 0, m_recyleList.size());
    qDeleteAll(m_recyleList.begin(), m_recyleList.end());
    m_recyleList.clear();
    m_Deletemap.clear();
    endRemoveRows();
}

void TableModel::switchDownloadingMode()
{
    m_mode = Downloading;
    m_renderList.clear();

    for(DataItem *item : m_dataList) {
        if((item->status == Global::Status::Active) || (item->status == Global::Status::Paused) ||
           (item->status == Global::Status::Waiting) || (item->status == Global::Status::Lastincomplete) ||
           (item->status == Global::Status::Error)) {
            m_renderList.append(item);
        }
    }
    sortDownload(m_iSortColumn,m_SortOrder);
}

void TableModel::switchFinishedMode()
{
    m_mode = Finished;
    m_renderList.clear();

    for(DataItem *item : m_dataList) {
        if(item->status == Global::Status::Complete) {
            m_renderList.append(item);
        }
    }
    sortDownload(m_iSortColumn,m_SortOrder);
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(m_iTableviewtabFlag == 0) {
        return m_renderList.size();
    }
    if(m_iTableviewtabFlag == 1) {
        return m_recyleList.size();
    }
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 5;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    const DataItem *data = nullptr;
    const DelDataItem *deldata = new DelDataItem();
    QChar sizeSepChar;


    if(m_iTableviewtabFlag == 0) {
        if((m_renderList.size() > 0) && (m_renderList.size() > row)) {
            data = m_renderList.at(row);
            sizeSepChar = (!data->totalLength.isEmpty()) ? '/' : ' ';
        } else {
            return QVariant();
        }
    } else {
        deldata = m_recyleList.at(row);
    }

    bool Ischecked;
    QString fileName;
    QString savePath;
    QString gid;
    QString url;
    QString deleteTime;
    switch(role) {
        case TableModel::Ischecked:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->Ischecked ? Qt::Checked : Qt::Unchecked;
            } else {
                return deldata->Ischecked ? Qt::Checked : Qt::Unchecked;
            }
        }

        case TableModel::FileName:
        {
            if(m_iTableviewtabFlag == 1) {
                return deldata->fileName;
            }
            if(m_iTableviewtabFlag == 0) {
                return data->fileName;
            }
            break;
        }

        case TableModel::Size:
        {
            if(m_iTableviewtabFlag == 0) {
                // return QString("%1%2%3
                //  %4").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength).arg(data->percent);
                return QString("%1%2%3 ").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength);
            } else {
                return deldata->totalLength;
            }
        }

        case TableModel::Speed:
        {
            if(m_iTableviewtabFlag == 0) {
                return (data->status != Global::Status::Paused) ? data->speed : "0kb/s";
            }
            break;
        }

        case TableModel::Time:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->time;
            } else {
                return deldata->deleteTime;
            }
        }

        case TableModel::Status:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->status;
            } else {
                return deldata->status;
            }
        }

        case TableModel::GID:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->gid;
            } else {
                return deldata->gid;
            }
        }

        case TableModel::taskId:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->taskId;
            } else {
                return deldata->taskId;
            }
        }
    case TableModel::createTime:
    {
        if(m_iTableviewtabFlag == 0) {
            return data->createTime;
        } else {
            return deldata->deleteTime;
        }
    }

        case TableModel::Percent:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->percent;
            }
            break;
        }

        case TableModel::TotalLength:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->total;
            } else if(m_iTableviewtabFlag == 1) {
                return deldata->totalLength;
            }
            break;
        }

        case TableModel::SavePath:
        {
            if(m_iTableviewtabFlag == 0) {
                return data->savePath;
            } else {
                return deldata->savePath;
            }
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(role == Qt::BackgroundRole) {
        return QBrush(QColor(Qt::white));
    }

    if(orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(section) {
    case 0:
        return tr("");

    case 1:
        return tr("Name");

    case 2:
        return tr("Size");

    case 3:
        if((m_mode == Downloading) && (m_iTableviewtabFlag == 0)) {
            return tr("Status");
        } else if((m_mode == Finished) && (m_iTableviewtabFlag == 0)) {
            return tr("Time finished");
        } else {
            return tr("Time deleted");
        }

    case 4:
        if((m_mode == Downloading) && (m_iTableviewtabFlag == 0)) {
            return tr("Status");
        } else if((m_mode == Finished) && (m_iTableviewtabFlag == 0)) {
            return tr("Time finished");
        } else {
            return tr("Time deleted");
        }
    }

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()){
        return QAbstractItemModel::flags(index);
    }
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;//ItemIsEnabled,表明这一项可以使用，ItemIsSelectable 表明这一项可以选中。

    if (index.column() == 1)//增加第二列的控制选项。
      flags |= Qt::ItemIsUserCheckable|Qt::ItemIsEditable;
    return flags;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) {
        return false;
    }

    int nColumn = index.column();
    const int row = index.row();

    DataItem *data = new DataItem;
    DelDataItem *deldata = new DelDataItem;

    if(m_iTableviewtabFlag == 0) {
        data = m_renderList.at(row);
    } else {
        deldata = m_recyleList.at(row);
    }

    switch(role) {
        case TableModel::Ischecked:
        {
            if(nColumn == 0) {
                if(m_iTableviewtabFlag == 0) {
                    data->Ischecked = value.toBool();
                    m_renderList.replace(index.row(), data);
                } else {
                    deldata->Ischecked = value.toBool();
                    m_recyleList.replace(index.row(), deldata);
                }

                // emit dataChanged(index,index);
                emit checkDatachange(m_iTableviewtabFlag);
                emit signalCheckChange(value.toBool(), m_iTableviewtabFlag);
                return true;
            }
            break;
        }
        case TableModel::FileName:
        {
            if(nColumn == 1) {
                if(m_iTableviewtabFlag == 0) {
                    data->fileName = value.toString();
                    m_renderList.replace(index.row(), data);
                } else {
                    deldata->fileName = value.toString();
                    m_recyleList.replace(index.row(), deldata);
                }
                return true;
            }
            break;
        }
        case TableModel::SavePath:
        {
            if(m_iTableviewtabFlag == 0) {
                data->savePath = value.toString();
                m_renderList.replace(index.row(), data);
            } else {
                deldata->savePath = value.toString();
                m_recyleList.replace(index.row(), deldata);
            }
            break;
        }

        default:
            return false;
    }
    return false;
}

int  TableModel::getTablemodelMode()
{
    return m_mode;
}

const QList<DataItem *> TableModel::dataList()
{
    return m_dataList;
}

const QList<DataItem *> TableModel::renderList()
{
    return m_renderList;
}

const QList<DelDataItem *> TableModel::recyleList()
{
    return m_recyleList;
}

QMap<QString, DataItem *> TableModel::getTableModelMap()
{
    return m_map;
}

int TableModel::DownloadingCount()
{
    if(Downloading ==  m_mode){
        return m_renderList.count();
    } else{
        return m_dataList.count() - m_renderList.count();
    }
}

typedef bool (*LessThan)(const QPair<QVariant, int> &left,
                         const QPair<QVariant, int> &right);

bool itemLessThan(const QPair<QVariant, int> &left,
                  const QPair<QVariant, int> &right)
{
    return left.first < right.first;
}

bool itemGreaterThan(const QPair<QVariant, int> &left,
                     const QPair<QVariant, int> &right)
{
    return right.first < left.first;
}

void TableModel::sort(int column, Qt::SortOrder order)
{
//    if(0 == column){
//        return;
//    }
    if(0 == m_iTableviewtabFlag){
        m_iSortColumn = column;
        m_SortOrder = order;
        sortDownload(column, order);
    } else if(1 == m_iTableviewtabFlag){
        sortRecycle(column, order);
    }

    emit layoutChanged();
}

void TableModel::sortDownload(int column, Qt::SortOrder order)
{
    QVector<QPair<QVariant, int> > sortable;
    QVector<int> unsortable;
    int role = 0;
    switch(column){
        case 0:
            role = TableModel::createTime;
            break;
        case 1:
            role = TableModel::FileName;
            break;
        case 2:
            if(Downloading == m_mode){
                role = TableModel::Speed;
            } else {
                role = TableModel::TotalLength;
            }
            break;
        case 3:
            if(Downloading == m_mode){
                role = TableModel::Status;
            }
            break;
        case 4:
            if(Finished == m_mode){
                role = TableModel::Time;
            }
            break;
    }
    double num = -1;
    for(int row = 0; row < rowCount(); ++row) {
        QVariant itm = data(this->index(row, column), role);
        if(role == TableModel::Size){
            num = formatFileSize(itm.toString());
            sortable.append(QPair<QVariant, int>(num, row));
        } else {
            if(!itm.isNull()) {
                sortable.append(QPair<QVariant, int>(itm, row));
            } else   {
                unsortable.append(row);
            }
        }
    }

    LessThan compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan);
    std::stable_sort(sortable.begin(), sortable.end(), compare);

    QList<DataItem*> sortData;
    emit layoutAboutToBeChanged();
    int  nSwapCount = rowCount();
    for(int i = 0; i < nSwapCount; i++) {
        int r = (i < sortable.count()
                 ? sortable.at(i).second
                 : unsortable.at(i - sortable.count()));
        sortData.append(m_renderList[r]);
    }
    m_renderList = sortData;
    emit layoutChanged();
}

void TableModel::sortRecycle(int column, Qt::SortOrder order)
{
    QVector<QPair<QVariant, int> > sortable;
    QVector<int> unsortable;

    sortable.reserve(rowCount());
    unsortable.reserve(rowCount());

    int role = 0;
    switch(column){
    case 1:
        role = TableModel::FileName;
        break;
    case 2:
        role = TableModel::TotalLength;
        break;
    case 4:
        role = TableModel::Time;
        break;
    }
    double num = -1;
    for(int row = 0; row < rowCount(); ++row) {
        QVariant itm = data(this->index(row, column), role);
        if(role == TableModel::TotalLength){
            num = formatFileSize(itm.toString());
            sortable.append(QPair<QVariant, int>(num, row));
        } else{
            if(!itm.isNull()) {
                sortable.append(QPair<QVariant, int>(itm, row));
            } else   {
                unsortable.append(row);
            }
        }
    }
    LessThan compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan);
    std::stable_sort(sortable.begin(), sortable.end(), compare);

    QList<DelDataItem*> sortData;
    emit layoutAboutToBeChanged();
    int  nSwapCount = rowCount();
    for(int i = 0; i < nSwapCount; i++) {
        int r = (i < sortable.count()
                 ? sortable.at(i).second
                 : unsortable.at(i - sortable.count()));
        sortData.append(m_recyleList[r]);
    }
    m_recyleList = sortData;
    emit layoutChanged();
}

double TableModel::formatFileSize(QString str)
{
    double num = -1;
    QString number = str.remove(str.length() - 2, 2);
    num = number.toDouble();
    if(str.contains("KB")){

        num = num * 1024;
    } else if(str.contains("MB")){

        num = num * 1024 * 1024;
    } else if(str.contains("GB")){

        num = num * 1024 * 1024 * 1024;
    }

    return num;
}
