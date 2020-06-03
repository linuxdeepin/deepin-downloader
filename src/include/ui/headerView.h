/**
* @file headerView.h
* @brief 表头
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:44
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <DCheckBox>
#include <DApplication>
#include <dguiapplicationhelper.h>

DWIDGET_USE_NAMESPACE

/**
 * @class HeaderView
 * @brief 表头
 */
class HeaderView: public QHeaderView
{
    Q_OBJECT
public:
    explicit HeaderView(Qt::Orientation orientation, QWidget * parent = 0);

protected:
    /**
     * @brief 更新位置
     */
    void updateGeometries();

private:
   DCheckBox *m_headerCbx;
signals:
   /**
    * @brief 表头全选按键状态改变按钮
    */
   void getStatechanged(bool checked);
public slots:
   /**
    * @brief 清除表头选中状态
    */
   void get_clear_header_check();
   /**
    * @brief 表头选中
    */
   void get_checkall_signals(bool checked);
   /**
    * @brief 调色板改变，重新设置颜色
    */
   void get_paletteTypeChanged(DGuiApplicationHelper::ColorType type);




};

#endif // HEADERVIEW_H
