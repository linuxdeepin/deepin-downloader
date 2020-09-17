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

#include "settings.h"
#include <dpinyin.h>
#include "global.h"
using namespace Global;
using namespace DTK_CORE_NAMESPACE;

TableModel::TableModel(int Flag, QObject *parent)
    : QAbstractTableModel(parent)
    , m_Mode(Downloading)
    , m_TableviewtabFlag(Flag)
    , m_DownloadingCount(0)
{
    connect(this, &TableModel::checkDatachange, this, &TableModel::onCheckdatachange);
    m_SortColumn = 0;
    m_SortOrder = Qt::AscendingOrder;
}

TableModel::~TableModel()
{
}

void TableModel::onCheckdatachange(int flag)
{
    int checkNum = 0;
    QList<DownloadDataItem *> activeList;
    QList<DownloadDataItem *> finishList;

    if (flag == 0) {
        for (DownloadDataItem *item : m_DataList) {
            if (m_Mode == Downloading) {
                if (item->status != Global::DownloadJobStatus::Complete) {
                    activeList.append(item);
                }
            } else {
                if (item->status == Global::DownloadJobStatus::Complete) {
                    finishList.append(item);
                }
            }
        }
        if ((m_Mode == Downloading) && (activeList.size() > 0)) {
            for (DownloadDataItem *item : activeList) {
                if (item->Ischecked) {
                    checkNum++;
                }
            }
            if (checkNum == activeList.size()) {
                emit tableviewAllcheckedOrAllunchecked(true);
            } else {
                emit tableviewAllcheckedOrAllunchecked(false);
            }
        }
        if ((m_Mode == Finished) && (finishList.size() > 0)) {
            for (DownloadDataItem *item : finishList) {
                if (item->Ischecked) {
                    checkNum++;
                }
            }
            if (checkNum == finishList.size()) {
                emit tableviewAllcheckedOrAllunchecked(true);
            } else {
                emit tableviewAllcheckedOrAllunchecked(false);
            }
        }
    } else {
        for (int i = 0; i < m_RecyleList.size(); i++) {
            DeleteDataItem *del_data = m_RecyleList.at(i);
            if (del_data->Ischecked) {
                checkNum++;
            }
        }
        if (checkNum == m_RecyleList.size()) {
            emit tableviewAllcheckedOrAllunchecked(true);
        } else {
            emit tableviewAllcheckedOrAllunchecked(false);
        }
    }
}

DownloadDataItem *TableModel::find(const QString &taskId)
{
    if (m_Map.contains(taskId)) {
        return m_Map.value(taskId);
    }
    return nullptr;
}

DeleteDataItem *TableModel::find(const QString &gid, int flag)
{
    Q_UNUSED(flag);
    if (m_Deletemap.contains(gid)) {
        return m_Deletemap.value(gid);
    }

    return nullptr;
}

void TableModel::append(DownloadDataItem *data)
{
    const int row = m_DataList.size();

    beginInsertRows(QModelIndex(), row, row);
    m_DataList.append(data);
    m_Map.insert(data->taskId, data);
    endInsertRows();
}

void TableModel::append(DeleteDataItem *data)
{
    const int row = m_RecyleList.size();

    beginInsertRows(QModelIndex(), row, row);
    m_RecyleList.append(data);
    m_Deletemap.insert(data->taskId, data);
    endInsertRows();
}

void TableModel::removeItem(DownloadDataItem *data)
{
    if (m_Map.contains(data->taskId)) {
        beginRemoveRows(QModelIndex(), m_DataList.indexOf(data), m_DataList.indexOf(data));
        m_Map.remove(data->taskId);
        m_DataList.removeOne(data);
        m_RenderList.removeOne(data);
        delete data;
        endRemoveRows();
    }
}

void TableModel::removeItem(DeleteDataItem *data)
{
    if (m_Deletemap.contains(data->taskId)) {
        beginRemoveRows(QModelIndex(), m_RecyleList.indexOf(data), m_RecyleList.indexOf(data));
        m_Deletemap.remove(data->taskId);
        m_RecyleList.removeOne(data);
        delete data;
        endRemoveRows();
    }
}

void TableModel::removeItems()
{
    beginRemoveRows(QModelIndex(), 0, m_DataList.size());
    qDeleteAll(m_DataList.begin(), m_DataList.end());
    m_DataList.clear();
    m_Map.clear();
    endRemoveRows();
}

void TableModel::removeRecycleItems()
{
    beginRemoveRows(QModelIndex(), 0, m_RecyleList.size());
    qDeleteAll(m_RecyleList.begin(), m_RecyleList.end());
    m_RecyleList.clear();
    m_Deletemap.clear();
    endRemoveRows();
}

void TableModel::switchDownloadingMode()
{
    m_Mode = Downloading;
    m_RenderList.clear();

    for (DownloadDataItem *item : m_DataList) {
        if ((item->status == Global::DownloadJobStatus::Active) || (item->status == Global::DownloadJobStatus::Paused) || (item->status == Global::DownloadJobStatus::Waiting) || (item->status == Global::DownloadJobStatus::Lastincomplete) || (item->status == Global::DownloadJobStatus::Error)) {
            m_RenderList.append(item);
        }
    }
    sortDownload(m_SortColumn, m_SortOrder);
}

void TableModel::switchFinishedMode()
{
    m_Mode = Finished;
    m_RenderList.clear();

    for (DownloadDataItem *item : m_DataList) {
        if (item->status == Global::DownloadJobStatus::Complete) {
            m_RenderList.append(item);
        }
    }
    sortDownload(m_SortColumn, m_SortOrder);
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_TableviewtabFlag == 0) {
        return m_RenderList.size();
    } else {
        return m_RecyleList.size();
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
    const DownloadDataItem *data = nullptr;
    const DeleteDataItem *deldata = nullptr;
    QChar sizeSepChar;

    if (m_TableviewtabFlag == 0) {
        if ((m_RenderList.size() > 0) && (m_RenderList.size() > row)) {
            data = m_RenderList.at(row);
            sizeSepChar = (!data->totalLength.isEmpty()) ? '/' : ' ';
        } else {
            return QVariant();
        }
    } else {
        deldata = m_RecyleList.at(row);
    }

    QString fileName;
    QString savePath;
    QString gid;
    QString url;
    QString deleteTime;
    switch (role) {
    case TableModel::Ischecked: {
        if (m_TableviewtabFlag == 0) {
            return data->Ischecked ? Qt::Checked : Qt::Unchecked;
        } else {
            return deldata->Ischecked ? Qt::Checked : Qt::Unchecked;
        }
    }

    case TableModel::FileName: {
        if (m_TableviewtabFlag == 1) {
            return deldata->fileName;
        }
        if (m_TableviewtabFlag == 0) {
            return data->fileName;
        }
        break;
    }

    case TableModel::Size: {
        if (m_TableviewtabFlag == 0) {
            // return QString("%1%2%3
            //  %4").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength).arg(data->percent);
            if (m_Mode == Downloading) {
                return QString("%1%2%3 ").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength);
            } else {
                return QString("%1 ").arg(data->totalLength);
            }

        } else {
            return deldata->totalLength;
        }
    }
    case TableModel::Speed: {
        if (m_TableviewtabFlag == 0) {
            return data->speed; // (data->status != Global::DownloadJobStatus::Paused) ? data->speed : "0KB/s";
        }
        break;
    }

    case TableModel::Time: {
        if (m_TableviewtabFlag == 0) {
            return data->time;
        } else {
            return deldata->deleteTime;
        }
    }

    case TableModel::Status: {
        if (m_TableviewtabFlag == 0) {
            return data->status;
        } else {
            return deldata->status;
        }
    }

    case TableModel::GID: {
        if (m_TableviewtabFlag == 0) {
            return data->gid;
        } else {
            return deldata->gid;
        }
    }

    case TableModel::taskId: {
        if (m_TableviewtabFlag == 0) {
            return data->taskId;
        } else {
            return deldata->taskId;
        }
    }
    case TableModel::createTime: {
        if (m_TableviewtabFlag == 0) {
            return data->createTime;
        } else {
            return deldata->deleteTime;
        }
    }

    case TableModel::Percent: {
        if (m_TableviewtabFlag == 0) {
            return data->percent;
        }
        break;
    }

    case TableModel::TotalLength: {
        if (m_TableviewtabFlag == 0) {
            return data->totalLength;
        } else if (m_TableviewtabFlag == 1) {
            return deldata->totalLength;
        }
        break;
    }

    case TableModel::SavePath: {
        if (m_TableviewtabFlag == 0) {
            return data->savePath;
        } else {
            return deldata->savePath;
        }
    }
    case TableModel::IsHide: {
        if (m_TableviewtabFlag == 0) {
            return data->IsHide;
        } else {
            return deldata->IsHide;
        }
    }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (role == Qt::BackgroundRole) {
        return QBrush(QColor(Qt::white));
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return tr("");

    case 1:
        return tr("Name");

    case 2:
        return tr("Size");

    case 3:
        if ((m_Mode == Downloading) && (m_TableviewtabFlag == 0)) {
            return tr("Status");
        } else if ((m_Mode == Finished) && (m_TableviewtabFlag == 0)) {
            return tr("Time finished");
        } else {
            return tr("Time deleted");
        }

    case 4:
        if ((m_Mode == Downloading) && (m_TableviewtabFlag == 0)) {
            return tr("Status");
        } else if ((m_Mode == Finished) && (m_TableviewtabFlag == 0)) {
            return tr("Time finished");
        } else {
            return tr("Time deleted");
        }
    }

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable; //ItemIsEnabled,表明这一项可以使用，ItemIsSelectable 表明这一项可以选中。

    if (index.column() == 1) //增加第二列的控制选项。
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    return flags;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    int nColumn = index.column();
    const int row = index.row();

    DownloadDataItem *data = nullptr;
    DeleteDataItem *deldata = nullptr;

    if (m_TableviewtabFlag == 0) {
        data = m_RenderList.at(row);
    } else {
        deldata = m_RecyleList.at(row);
    }

    switch (role) {
    case TableModel::Ischecked: {
        if (nColumn == 0) {
            if (m_TableviewtabFlag == 0) {
                data->Ischecked = value.toBool();
                m_RenderList.replace(index.row(), data);
            } else {
                deldata->Ischecked = value.toBool();
                m_RecyleList.replace(index.row(), deldata);
            }

            // emit dataChanged(index,index);
            emit checkDatachange(m_TableviewtabFlag);
            emit CheckChange(value.toBool(), m_TableviewtabFlag);
            return true;
        }
        break;
    }
    case TableModel::FileName: {
        if (nColumn == 1) {
            if (m_TableviewtabFlag == 0) {
                data->fileName = value.toString();
                m_RenderList.replace(index.row(), data);
            } else {
                deldata->fileName = value.toString();
                m_RecyleList.replace(index.row(), deldata);
            }
            return true;
        }
        break;
    }
    case TableModel::SavePath: {
        if (m_TableviewtabFlag == 0) {
            data->savePath = value.toString();
            m_RenderList.replace(index.row(), data);
        } else {
            deldata->savePath = value.toString();
            m_RecyleList.replace(index.row(), deldata);
        }
        break;
    }
    case TableModel::IsHide: {
        if (m_TableviewtabFlag == 0) {
            data->IsHide = value.toBool();
            m_RenderList.replace(index.row(), data);
        } else {
            deldata->IsHide = value.toBool();
            m_RecyleList.replace(index.row(), deldata);
        }
        break;
    }

    default:
        return false;
    }
    return false;
}

int TableModel::getTablemodelMode()
{
    return m_Mode;
}

const QList<DownloadDataItem *> &TableModel::dataList()
{
    return m_DataList;
}

const QList<DownloadDataItem *> &TableModel::renderList()
{
    return m_RenderList;
}

const QList<DeleteDataItem *> &TableModel::recyleList()
{
    return m_RecyleList;
}

QMap<QString, DownloadDataItem *> &TableModel::getTableModelMap()
{
    return m_Map;
}

int TableModel::DownloadingCount()
{
    if (Downloading == m_Mode) {
        return m_RenderList.count();
    } else {
        return m_DataList.count() - m_RenderList.count();
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
    if (0 == column) {
        return;
    }
    if (0 == m_TableviewtabFlag) {
        m_SortColumn = column;
        m_SortOrder = order;
        sortDownload(column, order);
    } else if (1 == m_TableviewtabFlag) {
        sortRecycle(column, order);
    }
}

void TableModel::sortDownload(int column, Qt::SortOrder order)
{
    if (Finished == m_Mode && column == 3) {
        return;
    }
    QVector<QPair<QVariant, int>> sortable;
    QVector<int> unsortable;
    int role = 0;
    switch (column) {
    case 0:
        role = TableModel::createTime;
        break;
    case 1:
        role = TableModel::FileName;
        break;
    case 2:
        if (Downloading == m_Mode) {
            role = TableModel::TotalLength;
        } else {
            role = TableModel::TotalLength;
        }
        break;
    case 3:
        if (Downloading == m_Mode) {
            role = TableModel::Status;
        }
        break;
    case 4:
        if (Finished == m_Mode) {
            role = TableModel::Time;
        }
        break;
    }
    if (Settings::getInstance()->getAutoSortBySpeed()) {
        role = TableModel::Speed;
    }
    double num = -1;
    int hideCount = 0;
    for (int row = 0; row < rowCount(); ++row) {
        QVariant itm = DTK_NAMESPACE::Core::Chinese2Pinyin(data(index(row, column), role).toString());
        if (role == TableModel::Size || role == TableModel::TotalLength) {
            num = formatFileSize(itm.toString());
            sortable.append(QPair<QVariant, int>(num, row));
        } else {
            if (!itm.isNull()) {
                sortable.append(QPair<QVariant, int>(itm, row));
            } else {
                unsortable.append(row);
            }
        }
    }
    if (Settings::getInstance()->getAutoSortBySpeed()) {
        LessThan compare = &itemGreaterThan;
        std::stable_sort(sortable.begin(), sortable.end(), compare);
    } else {
        LessThan compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan);
        std::stable_sort(sortable.begin(), sortable.end(), compare);
    }

    QList<DownloadDataItem *> sortData;
    emit layoutAboutToBeChanged();
    int nSwapCount = rowCount() - hideCount;
    for (int i = 0; i < nSwapCount; i++) {
        int r = (i < sortable.count()
                     ? sortable.at(i).second
                     : unsortable.at(i - sortable.count()));
        sortData.append(m_RenderList[r]);
    }
    m_RenderList = sortData;
    emit layoutChanged();
}

void TableModel::sortRecycle(int column, Qt::SortOrder order)
{
    QVector<QPair<QVariant, int>> sortable;
    QVector<int> unsortable;

    sortable.reserve(rowCount());
    unsortable.reserve(rowCount());

    int role = 0;
    switch (column) {
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
    int hideCount = 0;
    for (int row = 0; row < rowCount(); ++row) {
        QVariant itm = DTK_NAMESPACE::Core::Chinese2Pinyin(data(index(row, column), role).toString());
        if (role == TableModel::TotalLength) {
            num = formatFileSize(itm.toString());
            sortable.append(QPair<QVariant, int>(num, row));
        } else {
            if (!itm.isNull()) {
                sortable.append(QPair<QVariant, int>(itm, row));
            } else {
                unsortable.append(row);
            }
        }
    }
    LessThan compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan);
    std::stable_sort(sortable.begin(), sortable.end(), compare);

    QList<DeleteDataItem *> sortData;
    emit layoutAboutToBeChanged();
    int nSwapCount = rowCount() - hideCount;
    for (int i = 0; i < nSwapCount; i++) {
        int r = (i < sortable.count()
                     ? sortable.at(i).second
                     : unsortable.at(i - sortable.count()));
        sortData.append(m_RecyleList[r]);
    }
    m_RecyleList = sortData;
    emit layoutChanged();
}

double TableModel::formatFileSize(QString str)
{
    double num = -1;
    QString number = str.left(str.length() - 2);
    num = number.toDouble();
    if (str.contains("KB")) {
        num = num * 1024;
    } else if (str.contains("MB")) {
        num = num * 1024 * 1024;
    } else if (str.contains("GB")) {
        num = num * 1024 * 1024 * 1024;
    }

    return num;
}
