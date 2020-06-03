#include "btinfodelegate.h"

#include "btinfodialog.h"

#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QFileIconProvider>
#include <QTemporaryFile>

BtInfoDelegate::BtInfoDelegate(DDialog *dialog)
{
    this->m_dialog = dialog;
    this->m_checkBtn = new QCheckBox;
}

BtInfoDelegate::~BtInfoDelegate()
{
    delete m_checkBtn;
}

void BtInfoDelegate::setHoverColor(QBrush c)
{
    this->hoverColor = c;
}

void BtInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (index.row() == this->hoverRow) {
        painter->fillRect(option.rect, this->hoverColor); //QColor(0,0,0,13)QColor(255,255,255,26)
    } else {
        if (index.row() % 2 == 1) {
            painter->fillRect(option.rect, option.palette.base());
        } else {
            painter->fillRect(option.rect, option.palette.alternateBase());
        }
    }
    painter->restore();
    painter->save();

    if (index.column() == 0) {
        QStyleOptionButton checkBoxStyle;
        checkBoxStyle.state = index.data().toString() == "1" ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyle.state |= QStyle::State_Enabled;
        checkBoxStyle.rect = option.rect;
        checkBoxStyle.rect.setX(option.rect.x() + 5); //option.rect.width() / 2 - 6
        checkBoxStyle.rect.setWidth(15);

        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter, m_checkBtn);

        QString ext = index.model()->data(index.model()->index(index.row(), 2)).toString();
        QFileIconProvider prov;
        QString tempFilePath = QDir::tempPath() + QDir::separator() + QCoreApplication::applicationName() + "_temp.";
        QFileInfo fi(tempFilePath + ext);
        QIcon icon = prov.icon(fi);
        if (icon.isNull()) {
            icon = prov.icon(QFileIconProvider::File);
        }
        QPixmap pic = icon.pixmap(20, 20);
        painter->drawPixmap(option.rect.x() + 32, option.rect.y() + 13, pic);

        painter->setPen(Qt::darkGray);
        QString text = painter->fontMetrics().elidedText(index.model()->data(index.model()->index(index.row(), 1)).toString(), Qt::ElideRight, option.rect.width() - 55);
        painter->drawText(option.rect.x() + 55, option.rect.y() + 28, text);
    } else {
        painter->setPen(Qt::darkGray);
        QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);
        painter->drawText(option.rect.x() + 5, option.rect.y() + 28, text);
    }
    painter->restore();
}

bool BtInfoDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (index.column() == 0) {
        QRect rect(option.rect);
        rect.setX(10);
        rect.setWidth(15);
        if (event->type() == QEvent::MouseButtonPress
            && mouseEvent->button() == Qt::LeftButton
            && rect.contains(mouseEvent->pos())) {
            QString v = index.data().toString();
            model->setData(index, QVariant(v == "1" ? "0" : "1"), Qt::EditRole);


            ((BtInfoDialog*)m_dialog)->updateSelectedInfo();

            return false;
        }
    }
    return false;
}

void BtInfoDelegate::slot_hoverChanged(const QModelIndex &index)
{
    hoverRow = index.row();
}
