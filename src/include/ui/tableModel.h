/**
* @file tableModel.h
* @brief 表格数据管理模块
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:41
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H

#include "global.h"
#include <QAbstractTableModel>

using namespace Global;

/**
 * @class TableModel
 * @brief 表格数据管理模块
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(int      Flag,
               QObject *parent = nullptr);
    /**
     * @brief 列表模式
     */
    enum Mode {
        Downloading = 0, Finished
    };
    /**
     * @brief 数据角色
     */
    enum DataRole {
        Ischecked = 0, Speed, Size, FileName, Time, Status, GID, Percent, TotalLength, SavePath
    };


    /**
     * @brief 父类虚函数，设置数据
     * @param ndex: 索引
     * @param value: 数据
     * @param role: 角色
     */
    bool setData(const QModelIndex &ndex,
                 const QVariant    &value,
                 int                role);
    /**
     * @brief 在正在下载列表添加一列
     * @param data: 数据
     */
    void append(DataItem *data);
    /**
     * @brief 在回收站列表添加一列
     * @param data: 数据
     */
    void append(DelDataItem *data);
    /**
     * @brief 在正在下载列表删除一列
     * @param data: 数据
     */
    void removeItem(DataItem *data);
    /**
     * @brief 在回收站列表删除一列
     * @param data: 数据
     */
    void removeItem(DelDataItem *data);
    /**
     * @brief 清空下载列表
     */
    void removeItems();
    /**
     * @brief 清空回收站列表
     */
    void removeItems(bool isrecycle);
    /**
     * @brief 切换下载状态
     */
    void switchDownloadingMode();
    /**
     * @brief 切换完成状态
     */
    void switchFinishedMode();
    /**
     * @brief 根据id查找正在下载item
     * @param gid: id
     * @return 查找到的item数据
     */
    DataItem* find(const QString &gid);
    /**
     * @brief 根据id查找回收站item
     * @param gid: id
     * @return 查找到的item数据
     */
    DelDataItem* find(const QString &gid, int flag);
    /**
     * @brief 获取正在下载列表
     * @return item列表
     */
    const QList<DataItem *>    dataList();
    /**
     * @brief 获取已完成下载列表
     * @return item列表
     */
    const QList<DataItem *>    renderList();
    /**
     * @brief 获取回收站下载列表
     * @return item列表
     */
    const QList<DelDataItem *> recyleList();
    /**
     * @brief 获取列表展示模式
     * @return 模式
     */
    int get_tablemodel_mode();
    /**
     * @brief 获取列表展示模式
     * @return 模式map
     */
    QMap<QString, DataItem *>  get_table_model_map();
    /**
     * @brief 获取行数
     * @param index: 索引
     * @return 行数
     */
    int rowCount(const QModelIndex &parent) const;
    /**
     * @brief 获取列数
     * @param index: 索引
     * @return 列数
     */
    int columnCount(const QModelIndex &parent) const;
    /**
     * @brief 获取数据
     * @param index: 索引
     * @param role: 角色
     * @return 数据
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * @brief 获取表头
     * @param role: 角色
     * @return 表头数据
     */
    QVariant headerData(int ection, Qt::Orientation orientation, int role) const;
    ~TableModel();

signals:
    /**
     * @brief 改变选中
     * @param flag: 模式
     */
    void check_dataChange(int flag);
    /**
     * @brief 全选或者全不选
     * @param checked: 选中状态
     */
    void tableView_allChecked_or_allUnchecked(bool checked);
    /**
     * @brief 选中或者不选中
     * @param flag: 模式
     */
    void signalCheckChange(bool checked, int  flag);
private slots:
    /**
     * @brief 获取选中改变
     * @param flag: 模式
     */
    void get_check_dataChange(int flag);
private:
    QList<DataItem *> m_dataList;
    QList<DataItem *> m_renderList;
    QMap<QString, DataItem *> m_map;
    Mode m_mode;
    int TableViewTab_Flag;

    QMap<QString, DelDataItem *> m_Deletemap;
    QList<DelDataItem *> m_recyleList;
};

#endif // TABLEVIEWMODEL_H
