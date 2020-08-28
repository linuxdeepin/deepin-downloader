/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file Diagnostictool.h
 *
 * @brief 下载诊断工具
 *
 * @date 2020-08-24 17:10
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

#ifndef DIAGNOSTICTOOL_H
#define DIAGNOSTICTOOL_H

#include <DDialog>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>

DWIDGET_USE_NAMESPACE

class QTableView;
class DiagnosticModel;

/**
 * @class DiagnosticTool
 * @brief 下载诊断工具
 */
class DiagnosticTool : public DDialog
{
    Q_OBJECT
public:
    explicit DiagnosticTool(DDialog *parent = nullptr);

    void initUI();

private slots:
    void startDiagnostic();

private:
    QTableView *m_Tableview;
    DiagnosticModel *m_Model;
};

/**
 * @class DiagnosticModel
 * @brief 诊断管理模块
 */
class DiagnosticModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     */
    DiagnosticModel(QObject *parent = nullptr);

    ~DiagnosticModel();

    void appendData(bool b);

    void clearData();

    /**
     * @brief 父类虚函数，设置数据
     * @param ndex: 索引
     * @param value: 数据
     * @param role: 角色
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    /**
     * @brief 获取行数
     * @param index: 索引
     * @return 行数
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief 获取列数
     * @param index: 索引
     * @return 列数
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief 获取数据
     * @param index: 索引
     * @param role: 角色
     * @return 数据
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief 获取表头
     * @param role: 角色
     * @return 表头数据
     */
    //QVariant headerData(int ection, Qt::Orientation orientation, int role) const override;

    //Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<bool> m_DiagnosticStatusList;
};

/**
 * @class ItemDelegate
 * @brief 列表代理类
*/
class DiagnosticDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DiagnosticDelegate(QObject *parent = nullptr, int Flag = 0);
    ~DiagnosticDelegate();

    /**
     * @brief 绘图事件
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif //DIAGNOSTICTOOL_H
