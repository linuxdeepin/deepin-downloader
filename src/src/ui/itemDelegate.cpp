/**
* @file itemDelegate.cpp
* @brief 列表代理类
* @author zhaoyue  <zhaoyue@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 09:43
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#include "itemDelegate.h"

#include <QStandardItemModel>
#include <QStyleOptionProgressBar>
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QIcon>
#include <QProgressBar>
#include <QMouseEvent>

#include <DCheckBox>
#include <DGuiApplicationHelper>

#include "tableView.h"
#include "tableModel.h"

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

ItemDelegate::ItemDelegate(QObject *parent, int Flag)
    : QStyledItemDelegate(parent)
{
    Table_Flag = Flag;
    //progressbar = new QProgressBar;
    bg = new QPixmap(":/icons/bar-bg.png");
    front = new QPixmap(":/icons/bar-front.png");
}

ItemDelegate::~ItemDelegate()
{
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row() == this->hoverRow) {
        painter->fillRect(option.rect, Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameBorder()); //QColor(0,0,0,13)QColor(255,255,255,26)
    }
    const QRect rect(option.rect);
    const int column(index.column());
    const bool isSelected = option.state & QStyle::State_Selected;

    QFont font;
    font.setPointSize(11);
    painter->setFont(font);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QColor("#414D68"));
    if (isSelected) {
        painter->setPen(QColor("#FFFFFF"));
        painter->setBrush(QBrush("#0081FF"));
    }
    if (column == 0) {
        painter->fillRect(rect, Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().window());
        //        if(DGuiApplicationHelper::instance()->themeType()==2)
        //        {
        //            painter->fillRect(rect,QColor("#262626"));

        //        }
        //        else if(DGuiApplicationHelper::instance()->themeType()==1)
        //        {
        //            painter->fillRect(rect,QColor(248,248,248));
        //        }
    }
    const QRect textRect = rect.marginsRemoved(QMargins(10, 2, 0, 0));
    if (column == 0) {
        QStyleOptionButton checkBoxStyle;
        checkBoxStyle.state = index.data(TableModel::Ischecked).toBool() ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyle.state |= QStyle::State_Enabled;
        checkBoxStyle.rect = option.rect;
        checkBoxStyle.rect.setX(option.rect.x() + 5);
        checkBoxStyle.rect.setWidth(20);
        DCheckBox *check_btn = new DCheckBox;
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter, check_btn);
    }
    if (column == 1) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(QColor("#FFFFFF"));
            painter->setBrush(QBrush("#0081FF"));
            painter->drawRect(rect);
        }

        QRect rect = textRect;

        int x = rect.x();
        int y = 0;
        if (Table_Flag == 0) {
            if (rect.height() < 30)
                y = rect.y() + 3;
            else
                y = rect.y() + 15;
        } else {
            y = rect.y() + 3;
        }
        QString filetype = index.data(TableModel::FileName).toString();
        QString file_save_path = index.data(TableModel::SavePath).toString();
        QFileInfo fileInfo(file_save_path);
        QPixmap pic;
        if (file_save_path.contains(filetype) && fileInfo.isDir()) {
            QIcon icon = QIcon::fromTheme("folder");
            pic = icon.pixmap(20, 20);
        } else {
            QFileIconProvider iconProvider;
            QFileInfo filenameInfo(file_save_path + filetype);
            QIcon icon = iconProvider.icon(filenameInfo);
            if (icon.isNull()) {
                icon = iconProvider.icon(QFileIconProvider::IconType::File);
            }
            pic = icon.pixmap(20, 20);
        }

        painter->drawPixmap(x, y, pic);
        const QRect rect_text = rect.marginsRemoved(QMargins(25, 2, 0, 5));
        QString name = painter->fontMetrics().elidedText(index.data(TableModel::FileName).toString(), Qt::ElideRight, textRect.width() - 10);
        painter->drawText(rect_text, Qt::AlignVCenter | Qt::AlignLeft, name);
    } else if (column == 2) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(QColor("#FFFFFF"));
            painter->setBrush(QBrush("#0081FF"));
            painter->drawRect(rect);
        }
        const QString size = index.data(TableModel::Size).toString();
        painter->drawText(rect.marginsRemoved(QMargins(5, 2, 0, 2)), Qt::AlignVCenter | Qt::AlignLeft, size);

    } else if (column == 3) {
        if (Table_Flag == 0) {
            QFont font;
            font.setPointSize(10);
            //                if(DGuiApplicationHelper::instance()->themeType()==2)
            //                {
            //                    painter->setPen(QColor("#C0C6D4"));
            //                }
            //                else if(DGuiApplicationHelper::instance()->themeType()==1)
            //                {
            //painter->setPen(QColor("#414D68"));
            painter->setPen(Qt::red);
            painter->setPen(QColor("#8AA1B4"));

            //                }
            // painter->setPen(QColor("8AA1B4"));
            if (isSelected) {
                painter->setPen(QColor("#FFFFFF"));
                painter->setBrush(QBrush("#0081FF"));
                painter->drawRect(rect);
            }
            painter->setFont(font);
            QRect sizeRect = textRect;
            sizeRect.setTop(sizeRect.top() + 10);
            sizeRect.setHeight(sizeRect.height() / 4 - 2);
            QRect barRect = sizeRect;
            barRect.setTop(sizeRect.bottom() + 10);
            barRect.setWidth(barRect.width() - 10);
            barRect.setHeight(20);

            QStyleOptionViewItem viewOption(option);
            initStyleOption(&viewOption, index);
            if (index.data(TableModel::Status) == 2 || index.data(TableModel::Status) == 6) {
                const QString pauseText = painter->fontMetrics().elidedText(tr("paused"), Qt::ElideRight, textRect.width() - 10);
                painter->drawText(barRect, Qt::AlignBottom | Qt::AlignLeft, pauseText);

            } else if (index.data(TableModel::Status) == 3) {
                QFont font;
                font.setPointSize(10);
                painter->setFont(font);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(QColor("#FF6347"));
                const QRect rect_text = textRect.marginsRemoved(QMargins(5, 2, 0, 5));
                const QString errorText = painter->fontMetrics().elidedText(tr("download error"), Qt::ElideRight, rect_text.width() - 10);
                painter->drawText(rect_text, Qt::AlignVCenter | Qt::AlignLeft, errorText);
                return;
            } else {
                const QString sizeText = painter->fontMetrics().elidedText(" " + index.data(TableModel::Percent).toString() + "%    " + index.data(TableModel::Speed).toString() + "   " + tr("Surplus ") + index.data(TableModel::Time).toString(), Qt::ElideRight, textRect.width() - 10);
                painter->drawText(barRect, Qt::AlignBottom | Qt::AlignLeft, sizeText);
            }

            /* QStyleOptionProgressBar *optionBar = new QStyleOptionProgressBar;
                optionBar->initFrom(option.widget);
                optionBar->rect = sizeRect;
                optionBar->minimum = 0;
                optionBar->maximum = 100;
                optionBar->progress = index.data(TableViewModel::Percent).toInt();
                QApplication::style()->drawControl(QStyle::CE_ProgressBar, optionBar, painter, progressbar);*/
            QRect s1(0, 0, 3, this->bg->height());
            QRect t1(sizeRect.x(), sizeRect.y(), 3, this->bg->height());
            painter->drawPixmap(t1, *this->bg, s1);
            //bg m
            QRect s2(this->bg->width() - 3, 0, 3, this->bg->height());
            QRect t2(sizeRect.x() + sizeRect.width() - 16, sizeRect.y(), 3, this->bg->height());
            painter->drawPixmap(t2, *this->bg, s2);
            //bg t
            QRect s3(3, 0, this->bg->width() - 6, this->bg->height());
            QRect t3(sizeRect.x() + 3, sizeRect.y(), sizeRect.width() - 19, this->bg->height());
            painter->drawPixmap(t3, *this->bg, s3);

            float p = index.data(TableModel::Percent).toFloat() / 100.0f;
            int w = static_cast<int>((sizeRect.width() - 16) * p); //(int)((sizeRect.width() - 16) * p);

            if (w <= 3) {
                QRect s(sizeRect.x(), sizeRect.y(), w, this->front->height());
                QRect f(0, 0, 3, this->front->height());
                painter->drawPixmap(s, *this->front, f);
            } else if (w > 3 && w <= sizeRect.width() - 10) {
                //front h
                QRect s(sizeRect.x(), sizeRect.y(), 3, this->front->height());
                QRect f(0, 0, 3, this->front->height());
                painter->drawPixmap(s, *this->front, f);

                //front m
                QRect fs3(sizeRect.x() + 3, sizeRect.y(), w - 3, this->front->height());
                QRect ft3(3, 0, this->front->width() - 6, this->front->height());
                painter->drawPixmap(fs3, *this->front, ft3);
                //front td
                QRect s4(sizeRect.x() + w, sizeRect.y(), 3, this->front->height());
                QRect f4(front->width() - 3, 0, 3, this->front->height());
                painter->drawPixmap(s4, *this->front, f4);
            }
        }

    } else if (column == 4) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(QColor("#FFFFFF"));
            painter->setBrush(QBrush("#0081FF"));
            painter->drawRect(rect);
        }
        const QString time = index.data(TableModel::Time).toString();
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, time);
    }
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(-1, 50);
}

bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    bool ret = true;
    const int column(index.column());

    if (column == 0) {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        QRect rect(option.rect);
        rect.setWidth(25);
        rect.setX(rect.x() + 10);
        if ((event->type() == QEvent::MouseButtonPress) && rect.contains(mouseEvent->pos())) {
            QVariant value;
            value = model->data(index, TableModel::Ischecked);
            Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                                        ? Qt::Unchecked
                                        : Qt::Checked);
            ret = model->setData(index, state, TableModel::Ischecked); //取反后存入模型
        } else {
            ret = QStyledItemDelegate::editorEvent(event, model, option, index);
        }
    } else {
        ret = QStyledItemDelegate::editorEvent(event, model, option, index);
    }
    return ret;
}
void ItemDelegate::slot_hoverChanged(const QModelIndex &index)
{
    hoverRow = index.row();
}
