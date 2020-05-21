
#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H

#include "global.h"
#include <QAbstractTableModel>

using namespace Global;
class TableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Mode {
       Downloading=0, Finished
    };
    TableModel(int Flag,QObject *parent = nullptr);

        enum DataRole {
            Ischecked=0, Speed, Size, FileName, Time, Status, GID, Percent, TotalLength,SavePath
        };



        ~TableModel();
       bool setData(const QModelIndex &ndex, const QVariant &value, int role);

        void append(DataItem *data);
        void removeItem(DataItem *data);
        void removeItem(DelDataItem *data);
        void removeItems();
        void removeItems(bool isrecycle);

        void switchDownloadingMode();
        void switchFinishedMode();
        DataItem *find(const QString &gid);

        const QList<DataItem *> dataList();
        const QList<DataItem *> renderList();
        int get_tablemodel_mode();

        void append(DelDataItem *data);
        DelDataItem *find(const QString &gid,int flag);
        const QList<DelDataItem *> recyleList();
        QMap<QString, DataItem *> get_table_model_map();



    public:
        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    private:
        QList<DataItem *> m_dataList;
        QList<DataItem *> m_renderList;
        QMap<QString, DataItem *> m_map;
        Mode m_mode;
        int TableViewTab_Flag;

        QMap<QString, DelDataItem *> m_Deletemap;
        QList<DelDataItem *> m_recyleList;
signals:
        void check_dataChange(int flag);
        void tableView_allChecked_or_allUnchecked(bool checked);
        void signal_check_change(bool checked, int flag);
private slots:
        void get_check_dataChange(int flag);



};

#endif // TABLEVIEWMODEL_H
