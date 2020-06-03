#ifndef BTHEADERVIEW_H
#define BTHEADERVIEW_H

#include <QObject>
#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionButton>
#include <QLabel>

static int m_curWidget = 0;
class headerView : public QHeaderView
{
    Q_OBJECT
public:
    explicit headerView(Qt::Orientation orientation, QWidget *parent = 0);


protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    QRect checkBoxRect(const QRect &sourceRect) const;

};

#endif // BTHEADERVIEW_H
