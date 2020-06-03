#include "btheaderview.h"

headerView::headerView(Qt::Orientation orientation, QWidget *parent):
    QHeaderView(orientation, parent)
{

}


void headerView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex)const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    QStyleOptionButton option;
    option.rect = checkBoxRect(rect);
    style()->drawControl(QStyle::CE_Header, &option, painter);

    QPixmap _p;
    _p.load(":/icons/icon/headerIcon.png");
    style()->drawItemPixmap(painter, option.rect, logicalIndex, _p);
}

QRect headerView::checkBoxRect(const QRect &sourceRect) const
{
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                 &checkBoxStyleOption);
    m_curWidget += sourceRect.width();
    QPoint checkBoxPoint(m_curWidget-20, 14);

    if(m_curWidget > 400)
    {
        m_curWidget= 0;
    }
    return QRect(checkBoxPoint, checkBoxRect.size());
}


