/**
* @file tableModel.cpp
* @brief 表格数据管理模块
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:41
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "tableModel.h"
#include <QDebug>

TableModel::TableModel(int Flag,QObject *parent)
    : QAbstractTableModel(parent)
{
    TableViewTab_Flag=Flag;
    if(Flag==0)
    m_mode =Downloading;
    connect(this,&TableModel::check_dataChange,this,&TableModel::get_check_dataChange);
}

TableModel::~TableModel()
{
}

void TableModel::get_check_dataChange(int flag)
{
    int check_num=0;
    QList<DataItem *> active_list;
    QList<DataItem *> finish_list;
    if(flag==0)
    {
         for (DataItem *item : m_dataList)
         {
             if(m_mode==Downloading)
             {
                 if (item->status != Global::Status::Complete)
                 {
                    active_list.append(item);

                 }
             }
             else
             {
                 if(item->status==Global::Status::Complete)
                    finish_list.append(item);
             }
        }
         if(m_mode==Downloading &&active_list.size()>0)
         {
             for(DataItem *item :active_list)
             {
                 if(item->Ischecked)
                 {
                     check_num++;
                 }
             }
             if(check_num==active_list.size())
             {
                 emit tableView_allChecked_or_allUnchecked(true);
             }
             else {
                 emit tableView_allChecked_or_allUnchecked(false);

             }
         }
         if(m_mode==Finished &&finish_list.size()>0)
         {
             for(DataItem *item :finish_list)
             {
                 if(item->Ischecked)
                 {
                     check_num++;
                 }
             }
             if(check_num==finish_list.size())
             {
                 emit tableView_allChecked_or_allUnchecked(true);
             }
             else {
                 emit tableView_allChecked_or_allUnchecked(false);

             }
         }
    }
    else
    {
        for(int i=0;i<m_recyleList.size();i++)
        {
            DelDataItem *del_data=m_recyleList.at(i);
            if(del_data->Ischecked)
            {
                check_num++;
            }

        }
        if(check_num==m_recyleList.size())
        {
            emit tableView_allChecked_or_allUnchecked(true);
        }
        else {
            emit tableView_allChecked_or_allUnchecked(false);
        }
    }
}
DataItem * TableModel::find(const QString &gid)
{
    if (m_map.contains(gid))
    {
        return m_map.value(gid);
    }
    return nullptr;
}
DelDataItem * TableModel::find(const QString &gid,int flag)
{

    if (m_Deletemap.contains(gid))
    {
       return m_Deletemap.value(gid);
    }

    return nullptr;
}

void TableModel::append(DataItem *data)
{
    const int row = m_dataList.size();
    beginInsertRows(QModelIndex(), row, row);
    m_dataList.append(data);
    m_map.insert(data->taskId,data);
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
        if (m_map.contains(data->taskId))
        {
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
    if (m_Deletemap.contains(data->taskId))
    {
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

    for (DataItem *item : m_dataList) {
        if (item->status == Global::Status::Active||item->status==Global::Status::Paused||item->status==Global::Status::Waiting||item->status==Global::Status::Lastincomplete||item->status==Global::Status::Error) {
            m_renderList.append(item);
        }
    }
}

void TableModel::switchFinishedMode()
{
    m_mode=Finished;
    m_renderList.clear();

    for (DataItem *item : m_dataList) {
        if (item->status == Global::Status::Complete) {
            m_renderList.append(item);
        }
    }
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(TableViewTab_Flag==0)

        return m_renderList.size();
    if(TableViewTab_Flag==1)
    {
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
    const DataItem *data= new DataItem();
    const DelDataItem *deldata= new DelDataItem();
     QChar sizeSepChar;


    if(TableViewTab_Flag==0)
    {

        if(m_renderList.size()>0 && m_renderList.size() >= row)
        {
         data = m_renderList.at(row);
        sizeSepChar = (!data->totalLength.isEmpty()) ? '/' : ' ';
        }
        else {
            return QVariant();
        }

    }
    else
    {
         deldata=m_recyleList.at(row);

    }
    if(data==nullptr)
    {
        return QVariant();
    }

    bool Ischecked ;
    QString fileName;
    QString savePath;
    QString gid;
    QString url;
    QString deleteTime;
    switch (role)
    {

        case TableModel::Ischecked:
        {
             if(TableViewTab_Flag==0)
                return data->Ischecked ? Qt::Checked : Qt::Unchecked;
             else
             {
                 return  deldata->Ischecked ? Qt::Checked: Qt::Unchecked;
             }
        }
        case TableModel::FileName:
        {
            if(TableViewTab_Flag==1)
            {
                return  deldata->fileName;
            }
            if(TableViewTab_Flag==0)
                return data->fileName;
        }
        case TableModel::Size:
        {
            if(TableViewTab_Flag==0)
                //return QString("%1%2%3  %4").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength).arg(data->percent);
                return QString("%1%2%3 ").arg(data->completedLength).arg(sizeSepChar).arg(data->totalLength);

            else
            {
                return deldata->totalLength;
            }
        }
        case TableModel::Speed:
        {
            if(TableViewTab_Flag==0)
                return (data->status != Global::Status::Paused) ? data->speed : "0kb/s";
        }
        case TableModel::Time:
        {
            if(TableViewTab_Flag==0)
                return data->time;
            else
            {
                return  deldata->deleteTime;
            }
        }
        case TableModel::Status:
        {
            if(TableViewTab_Flag==0)
                return data->status;
            else
            {
                return  deldata->status;
            }
        }
        case TableModel::GID:
        {
            if(TableViewTab_Flag==0)
                return data->gid;
            else
            {
                return deldata->gid;
            }
        }
        case TableModel::Percent:
            if(TableViewTab_Flag==0)
                return data->percent;
        case TableModel::TotalLength:
            if(TableViewTab_Flag==0)
                return data->total;
        case TableModel::SavePath:
        {
            if(TableViewTab_Flag==0)
                return  data->savePath;
            else
            {
                return  deldata->savePath;
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

    if(role == Qt::BackgroundRole)
    {
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
        if(m_mode==Downloading&&TableViewTab_Flag==0)
            return tr("Status");
        else if(m_mode==Finished&&TableViewTab_Flag==0)
            return tr("Time finished");
        else
            return  tr("Time deleted");
    case 4:
        if(m_mode==Downloading&&TableViewTab_Flag==0)
            return tr("Status");
        else if(m_mode==Finished&&TableViewTab_Flag==0)
            return tr("Time finished");
        else
            return  tr("Time deleted");
    }

    return QVariant();
}


 bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
     if (!index.isValid())
        return false;

     int nColumn = index.column();
     const int row = index.row();

     DataItem *data= new DataItem;
     DelDataItem *deldata=new DelDataItem;

     if(TableViewTab_Flag==0)
     {
        data= m_renderList.at(row);
     }
     else
     {
         deldata= m_recyleList.at(row);
     }

    switch (role)
    {
    case TableModel::Ischecked:
    {
        if (nColumn == 0)
        {
            if(TableViewTab_Flag==0)
            {
                data->Ischecked = value.toBool();
                m_renderList.replace(index.row(), data);
            }
            else
            {
                deldata->Ischecked = value.toBool();
                m_recyleList.replace(index.row(), deldata);
            }
           // emit dataChanged(index,index);
            emit check_dataChange(TableViewTab_Flag);
            emit signal_check_change(value.toBool(), TableViewTab_Flag);
            return true;
        }
    }
    default:
        return false;
    }
    return false;
}
 int  TableModel::get_tablemodel_mode()
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
      return  m_recyleList;
  }
  QMap<QString, DataItem *> TableModel::get_table_model_map()
  {
      return  m_map;

  }
