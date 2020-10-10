/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file itemDelegate.cpp
 *
 * @brief 列表代理类
 *
 * @date 2020-06-09 10:00
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

#include "itemDelegate.h"

#include <DCheckBox>
#include <DLineEdit>
#include <DApplication>

#include <QStandardItemModel>
#include <QStyleOptionProgressBar>
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QLineEdit>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QIcon>
#include <QProgressBar>
#include <QMouseEvent>
#include <QItemEditorFactory>
#include <QMimeType>
#include <QMimeDatabase>

#include "tableView.h"
#include "tableModel.h"
#include "global.h"
#include "../database/dbinstance.h"

ItemDelegate::ItemDelegate(QObject *parent, int Flag)
    : QStyledItemDelegate(parent)
    , m_HoverRow(-1)
{
    m_TableFlag = Flag;
    m_IsFirstInside = true;
    // progressbar = new QProgressBar;
    m_BgImage = new QPixmap(":/icons/icon/bar-bg.png");
    m_Front = new QPixmap(":/icons/icon/bar-front.png");

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
            this, &ItemDelegate::onPalettetypechanged);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
            this, &ItemDelegate::onPalettetypechanged);
}

ItemDelegate::~ItemDelegate()
{
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row() == m_HoverRow) {
        painter->fillRect(option.rect, Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameBorder()); //
            // QColor(0,0,0,13)QColor(255,255,255,26)
    }
    if (index.row() % 2 != 0) {
        painter->fillRect(option.rect, QBrush(QColor(0, 0, 0, 8))); //
            // QColor(0,0,0,13)QColor(255,255,255,26)
    } else {
        painter->fillRect(option.rect, QBrush(QColor(255, 255, 255, 10))); //
    }
    const QRect rect(option.rect);
    const int column(index.column());
    const bool isSelected = index.data(TableModel::Ischecked).toBool(); //option.state & QStyle::State_Selected;

    QFont font;
    // font.setPointSize(11);
    painter->setFont(font);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QColor("#414D68"));
    //    if(isSelected) {
    //        painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
    //        painter->setBrush(QBrush(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight()));
    //        painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().WindowText);
    //    }
    //    if(column == 0) {
    //        painter->fillRect(rect, Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().window());
    //    }
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
    } else if (column == 1) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
            painter->setBrush(QBrush(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight()));
            painter->drawRoundRect(rect.x(), rect.y(), 25, rect.height(), 25, 25);
            painter->drawRect(rect.x() + 15, rect.y(), rect.width() - 15, rect.height());
            painter->setPen(QColor("#FFFFFF"));
            QItemEditorFactory::setDefaultFactory(new QItemEditorFactory);
        }

        QRect localRect = textRect;

        int x = localRect.x();
        int y = localRect.y() + 12;
        QString filetype = index.data(TableModel::FileName).toString();
        QString fileSavePath = index.data(TableModel::SavePath).toString();
        QFileInfo fileInfo(fileSavePath);
        QPixmap pic;
        if (fileSavePath.contains(filetype) && fileInfo.isDir()) {
            QIcon icon = QIcon::fromTheme("folder");
            pic = icon.pixmap(20, 20);
        } else {
            QFileIconProvider iconProvider;
            QFileInfo filenameInfo(fileSavePath + filetype);
            QIcon icon = iconProvider.icon(filenameInfo);
            if (icon.isNull()) {
                icon = iconProvider.icon(QFileIconProvider::IconType::File);
            }
            pic = icon.pixmap(20, 20);
        }

        painter->drawPixmap(x, y, pic);

        const QString path = index.data(TableModel::SavePath).toString();
        int status = index.data(TableModel::Status).toInt();
        if ((!QFileInfo::exists(path)) && (status == Global::DownloadJobStatus::Complete || status == Global::DownloadJobStatus::Removed)) { //文件不存在的任务，添加提示
            QPixmap errorPic = QIcon(":icons/icon/error.svg").pixmap(12, 12);
            painter->drawPixmap(x + 10, y + 10, errorPic);
        }

        const QRect rectText = localRect.marginsRemoved(QMargins(25, 2, 0, 5));
        QString name = painter->fontMetrics().elidedText(index.data(TableModel::FileName).toString(),
                                                         Qt::ElideMiddle,
                                                         textRect.width() - 20);
        painter->drawText(rectText, Qt::AlignVCenter | Qt::AlignLeft, name);
    } else if (column == 2) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
            painter->setBrush(QBrush(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight()));
            painter->drawRect(rect);
            painter->setPen(QColor("#FFFFFF"));
        }
        const QString size = index.data(TableModel::Size).toString();
        painter->drawText(rect.marginsRemoved(QMargins(5, 2, 0, 2)), Qt::AlignVCenter | Qt::AlignLeft, size);
    } else if (column == 3) {
        if (m_TableFlag == 0) {
            QFont font;
            font.setPointSize(10);
            painter->setPen(Qt::red);
            painter->setPen(QColor("#8AA1B4"));

            if (isSelected) {
                painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
                painter->setBrush(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight());
                painter->drawRoundRect(rect.x() + rect.width() - 25, rect.y(), 25, rect.height(), 25, 25);
                painter->drawRect(rect.x(), rect.y(), rect.width() - 15, rect.height());
                painter->setPen(QColor("#FFFFFF"));

                m_BgImage->load(":/icons/icon/progressbar_bg.png");
                m_Front->load(":/icons/icon/progressbar_fg.png");
            } else {
                m_Front->load(":/icons/icon/bar-bg.png");
                m_Front->load(":/icons/icon/bar-front.png");
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
            if ((index.data(TableModel::Status) == Global::Paused) || (index.data(TableModel::Status) == Global::Lastincomplete)) {
                const QString pauseText = painter->fontMetrics().elidedText(tr("Paused"),
                                                                            Qt::ElideRight,
                                                                            textRect.width() - 10);
                painter->drawText(barRect, Qt::AlignBottom | Qt::AlignLeft, pauseText);
            } else if (index.data(TableModel::Status) == Global::Error) {
                QFont font;
                font.setPointSize(10);
                painter->setFont(font);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(QColor("#9a2f2f"));
                const QRect rect_text = textRect.marginsRemoved(QMargins(5, 2, 0, 5));
                const QString errorText = painter->fontMetrics().elidedText(tr("Failed"),
                                                                            Qt::ElideRight,
                                                                            rect_text.width() - 10);
                painter->drawText(rect_text, Qt::AlignVCenter | Qt::AlignLeft, errorText);
                return;
            } else if (index.data(TableModel::Status) == Global::Waiting) {
                QFont font;
                font.setPointSize(10);
                painter->setFont(font);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(QColor("#8AA1B4"));
                const QRect rect_text = textRect.marginsRemoved(QMargins(5, 2, 0, 5));
                const QString wattingText = painter->fontMetrics().elidedText(tr("Watting"),
                                                                              Qt::ElideRight,
                                                                              rect_text.width() - 10);
                painter->drawText(rect_text, Qt::AlignVCenter | Qt::AlignLeft, wattingText);
                return;
            } else {
                QString speed = index.data(TableModel::Speed).toString();
                if (speed.left(2).toInt() < 0) {
                    speed = "0KB/s";
                }
                QString str = " " + index.data(TableModel::Percent).toString()
                              + "%    " + speed
                              + "   " + tr("Time left ")
                              + index.data(TableModel::Time).toString();
                const QString sizeText = painter->fontMetrics().elidedText(str,
                                                                           Qt::ElideRight,
                                                                           textRect.width() - 10);
                painter->drawText(barRect, Qt::AlignBottom | Qt::AlignLeft, sizeText);
            }

            QRect s1(0, 0, 3, m_BgImage->height());
            QRect t1(sizeRect.x(), sizeRect.y(), 3, m_BgImage->height());
            painter->drawPixmap(t1, *m_BgImage, s1);

            // bg m
            QRect s2(m_BgImage->width() - 3, 0, 3, m_BgImage->height());
            QRect t2(sizeRect.x() + sizeRect.width() - 16, sizeRect.y(), 3, m_BgImage->height());
            painter->drawPixmap(t2, *m_BgImage, s2);

            // bg t
            QRect s3(3, 0, m_BgImage->width() - 6, m_BgImage->height());
            QRect t3(sizeRect.x() + 3, sizeRect.y(), sizeRect.width() - 19, m_BgImage->height());
            painter->drawPixmap(t3, *m_BgImage, s3);

            float p = index.data(TableModel::Percent).toFloat() / 100.0f;
            int w = static_cast<int>((sizeRect.width() - 16) * p);

            if (w <= 3) {
                QRect s(sizeRect.x(), sizeRect.y(), w, m_Front->height());
                QRect f(0, 0, 3, m_Front->height());
                painter->drawPixmap(s, *m_Front, f);
            } else if ((w > 3) && (w <= sizeRect.width() - 10)) {
                // front h
                QRect s(sizeRect.x(), sizeRect.y(), 3, m_Front->height());
                QRect f(0, 0, 3, m_Front->height());
                painter->drawPixmap(s, *m_Front, f);

                // front m
                QRect fs3(sizeRect.x() + 3, sizeRect.y(), w - 3, m_Front->height());
                QRect ft3(3, 0, m_Front->width() - 6, m_Front->height());
                painter->drawPixmap(fs3, *m_Front, ft3);

                // front td
                QRect s4(sizeRect.x() + w, sizeRect.y(), 3, m_Front->height());
                QRect f4(m_Front->width() - 3, 0, 3, m_Front->height());
                painter->drawPixmap(s4, *m_Front, f4);
            }
        }
    } else if (column == 4) {
        if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 2) {
            painter->setPen(QColor("#C0C6D4"));
        } else if (Dtk::Gui::DGuiApplicationHelper::instance()->themeType() == 1) {
            painter->setPen(QColor("#414D68"));
        }
        if (isSelected) {
            painter->setPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color());
            painter->setBrush(QBrush(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight()));
            painter->drawRoundRect(rect.x() + rect.width() - 25, rect.y(), 25, rect.height(), 25, 25);
            painter->drawRect(rect.x(), rect.y(), rect.width() - 15, rect.height());
            painter->setPen(QColor("#FFFFFF"));
        }
        const QString time = index.data(TableModel::Time).toString();
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, time);

    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    switch (index.column()) {
    case 0:
        return QSize(40, 50);
    case 1:
        return QSize(248, 50);
    case 2:
        return QSize(114, 50);
    case 3:
        return QSize(304, 50);
    case 4:
        return QSize(304, 50);
    }
    return QSize(-1, 50);
}

bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                               const QStyleOptionViewItem &option, const QModelIndex &index)
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
            ret = model->setData(index, state, TableModel::Ischecked); // 取反后存入模型
        } else {
            ret = QStyledItemDelegate::editorEvent(event, model, option, index);
        }
    } else {
        ret = QStyledItemDelegate::editorEvent(event, model, option, index);
    }
    return ret;
}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    Q_UNUSED(option);

    static bool firstInside = true;
    firstInside = true;
    DLineEdit *pEdit = new DLineEdit(parent);
    QRegExp regx("[^\\\\/\':\\*\\?\"<>|#%]+"); //屏蔽特殊字符
    QValidator *validator = new QRegExpValidator(regx, pEdit);
    pEdit->lineEdit()->setValidator(validator);
    pEdit->lineEdit()->setMaxLength(83);
    connect(pEdit, &DLineEdit::textChanged, this, [=](QString filename) {
        DLineEdit *pEdit = qobject_cast<DLineEdit *>(sender());
        QString str = index.data(TableModel::FileName).toString();
        QMimeDatabase db;
        QString mime = db.suffixForFileName(str);
        QString filePath = index.data(TableModel::SavePath).toString();
        filePath = filePath.left(filePath.lastIndexOf("/") + 1);
        filePath = filePath + filename + "." + mime;
        QFileInfo file(filePath);

        int hasSameFile = false;
        for (int i = 0; i < index.model()->rowCount(); i++) {
            QModelIndex idx = index.sibling(i, index.column());
            QString path = idx.data(TableModel::SavePath).toString();
            if (filePath == path && (filePath != index.data(TableModel::SavePath).toString())) {
                hasSameFile = true;
                break;
            }
        }

        if ((file.isFile() && (filePath != index.data(TableModel::SavePath).toString())) || (hasSameFile)) {
            pEdit->showAlertMessage(tr("Duplicate name!"), -1);
        } else {
            pEdit->hideAlertMessage();
        }
        firstInside = false;
    });

    connect(pEdit, &DLineEdit::destroyed, this, [=]() {
        //setModelData(pEdit, index.model(), index);
    });
    pEdit->resize(parent->size());
    QString FilePath = index.data(TableModel::SavePath).toString();
    return pEdit;
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DLineEdit *pEdit = qobject_cast<DLineEdit *>(editor);
    QString str = index.data(TableModel::FileName).toString();

    QMimeDatabase db;
    QString mime = db.suffixForFileName(str);
    str = str.left(str.size() - mime.size() - 1);
    pEdit->setText(str);
}

void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    DLineEdit *pEdit = qobject_cast<DLineEdit *>(editor);
    QString str = index.data(TableModel::FileName).toString();
    QMimeDatabase db;
    QString mime = db.suffixForFileName(str);
    QString fileName = pEdit->text() + "." + mime;

    QString filePath = index.data(TableModel::SavePath).toString();
    filePath = filePath.left(filePath.lastIndexOf("/") + 1);
    filePath = filePath + fileName;
    if (!QFileInfo::exists(filePath) && !pEdit->text().isEmpty()) {
        for (int i = 0; i < index.model()->rowCount(); i++) {
            QModelIndex idx = index.sibling(i, index.column());
            QString path = idx.data(TableModel::SavePath).toString();
            if (filePath == path && (filePath != index.data(TableModel::SavePath).toString())) {
                return;
            }
        }

        QFile::rename(index.data(TableModel::SavePath).toString(), filePath);
        model->setData(index, fileName, TableModel::FileName);
        model->setData(index, filePath, TableModel::SavePath);
        TaskInfo task;
        DBInstance::getTaskByID(index.data(TableModel::taskId).toString(), task);
        task.downloadPath = filePath;
        task.downloadFilename = fileName;
        DBInstance::updateTaskByID(task);
    }
}

void ItemDelegate::onHoverchanged(const QModelIndex &index)
{
    m_HoverRow = index.row();
}

void ItemDelegate::onPalettetypechanged(DGuiApplicationHelper::ColorType type)
{
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        m_Front->load(":/icons/icon/bar-bg.png");
        m_BgImage->load(":/icons/icon/bar-front.png");
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        m_BgImage->load(":/icons/icon/bar-bg.png");
        m_Front->load(":/icons/icon/bar-front.png");
    }
}
