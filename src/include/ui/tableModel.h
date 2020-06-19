/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file tableModel.h
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

#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H

#include <QAbstractTableModel>

namespace Global {
    struct DataItem;
    struct DelDataItem;
}

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
    void append(Global::DataItem *data);

    /**
     * @brief 排序
     * @param column: 列
     * @param order: 排序方式
     */
    virtual void sort(int column, Qt::SortOrder order);

    /**
     * @brief 在回收站列表添加一列
     * @param data: 数据
     */
    void append(Global::DelDataItem *data);

    /**
     * @brief 在正在下载列表删除一列
     * @param data: 数据
     */
    void removeItem(Global::DataItem *data);

    /**
     * @brief 在回收站列表删除一列
     * @param data: 数据
     */
    void removeItem(Global::DelDataItem *data);

    /**
     * @brief 清空下载列表
     */
    void removeItems();

    /**
     * @brief 清空回收站列表
     */
    void removeItems(bool isrecycle);

    /**
     * @brief 切换到正在下载列表
     */
    void switchDownloadingMode();

    /**
     * @brief 切换到已完成列表
     */
    void switchFinishedMode();

    /**
     * @brief 根据id查找正在下载item
     * @param gid: id
     * @return 查找到的item数据
     */
    Global::DataItem* find(const QString &gid);

    /**
     * @brief 根据id查找回收站item
     * @param gid: id
     * @return 查找到的item数据
     */
    Global::DelDataItem* find(const QString &gid, int flag);

    /**
     * @brief 获取正在下载列表
     * @return item列表
     */
    const QList<Global::DataItem *>    dataList();

    /**
     * @brief 获取已完成下载列表
     * @return item列表
     */
    const QList<Global::DataItem *>    renderList();

    /**
     * @brief 获取回收站下载列表
     * @return item列表
     */
    const QList<Global::DelDataItem *> recyleList();

    /**
     * @brief 获取列表展示模式
     * @return 模式
     */
    int getTablemodelMode();

    /**
     * @brief 获取列表展示模式
     * @return 模式map
     */
    QMap<QString, Global::DataItem *>  getTableModelMap();

    /**
     * @brief 获取正在下载的个数
     * @return 正在下载的个数
     */
    int DownloadingCount();

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

private:
    void sortDownload(int column, Qt::SortOrder order);
    void sortRecycle(int column, Qt::SortOrder order);
    double formatFileSize(QString str);
signals:
    /**
     * @brief 改变选中
     * @param flag: 模式
     */
    void checkDatachange(int flag);
    /**
     * @brief 全选或者全不选
     * @param checked: 选中状态
     */
    void tableviewAllcheckedOrAllunchecked(bool checked);
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
    void getCheckDatachange(int flag);
private:
    QList<Global::DataItem *> m_dataList;
    QList<Global::DataItem *> m_renderList; //切换到正在下载列表就保存正在下载的数据，切换到已完成列表就保存已完成数据
    QMap<QString, Global::DataItem *> m_map;
    Mode m_mode;
    int m_iTableviewtabFlag;
    int m_iDownloadingCount;
    QMap<QString, Global::DelDataItem *> m_Deletemap;
    QList<Global::DelDataItem *> m_recyleList;
};

#endif // TABLEVIEWMODEL_H
