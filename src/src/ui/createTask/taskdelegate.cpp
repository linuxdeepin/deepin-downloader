/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file btinfodelegate.cpp
 *
 * @brief bt窗口中表格item代理类
 *
 * @date 2020-06-09 10:49
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
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

#include "taskdelegate.h"
#include "createtaskwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include <QEvent>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QCheckBox>
#include <QStandardItem>
#include <QDir>
#include <QThread>
#include <DAlertControl>

TaskDelegate::TaskDelegate(DDialog *dialog)
{
    m_dialog = dialog;
    m_checkBtn = new QCheckBox;
    m_curName.clear();
}

TaskDelegate::~TaskDelegate()
{
    delete m_checkBtn;
}

void TaskDelegate::setHoverColor(QBrush c)
{
    m_hoverColor = c;
}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QString size = index.model()->data(index.model()->index(index.row(), 3)).toString();
    if (index.row() == m_hoverRow && !size.isEmpty()) {
        if (index.row() % 2 == 1) {
            //painter->fillRect(option.rect, QColor(0, 0, 0, 0));
            painter->fillRect(option.rect, QColor(231, 231, 231));
        } else {
            //painter->fillRect(option.rect, m_hoverColor); //QColor(0,0,0,13)QColor(255,255,255,26)
            painter->fillRect(option.rect, QColor(214, 214, 214));
        }
    } else {
        if (index.row() % 2 == 1) {
           // painter->fillRect(option.rect, QColor(0, 0, 0, 20));
            painter->fillRect(option.rect, QColor(238, 238, 238));
        } else {
           // painter->fillRect(option.rect, QColor(0, 0, 0, 35));
            painter->fillRect(option.rect, QColor(225, 225, 225));
        }
    }


//    if (size.isEmpty()) {
//        if (index.row() % 2 == 1) {
//           // painter->fillRect(option.rect, QColor(0, 0, 0, 20));
//            painter->fillRect(option.rect, QColor(238, 238, 238));
//        } else {
//           // painter->fillRect(option.rect, QColor(0, 0, 0, 35));
//            painter->fillRect(option.rect, QColor(225, 225, 225));
//        }
//    }

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

        QPixmap shadowPic = QIcon(":icons/icon/shadow.png").pixmap(20, 20);
        if(size.isEmpty()){
            painter->drawPixmap(option.rect.x() + 33, option.rect.y() + 13, shadowPic);
        }

        painter->setPen(Qt::darkGray);

        painter->setPen(size.isEmpty() ? QColor(65, 77, 104, 70) : QColor(65,77,104));

        QString text = painter->fontMetrics().elidedText(index.model()->data(index.model()->index(index.row(), 1)).toString(), Qt::ElideRight, option.rect.width() - 55);
        painter->drawText(option.rect.x() + 55, option.rect.y() + 28, text);

    } else {
        painter->setPen(Qt::darkGray);
        painter->setPen(size.isEmpty() ? QColor(65, 77, 104, 70) : QColor(65,77,104));
        QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);
        painter->drawText(option.rect.x() + 5, option.rect.y() + 28, text);
    }
    painter->restore();
}

bool TaskDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    QString size = index.model()->data(index.model()->index(index.row(), 3)).toString();
    if (size.isEmpty()) {
        return false;
    }

    if (index.column() == 0) {
        QRect rect(option.rect);
        rect.setX(10);
        rect.setWidth(15);
        if (event->type() == QEvent::MouseButtonPress
            && mouseEvent->button() == Qt::LeftButton
            && rect.contains(mouseEvent->pos())) {
            QString v = index.data().toString();
            model->setData(index, QVariant(v == "1" ? "0" : "1"), Qt::EditRole);

            ((CreateTaskWidget *)m_dialog)->updateSelectedInfo();
            return false;
        } else if (event->type() == QEvent::MouseButtonDblClick
                   && !rect.contains(mouseEvent->pos())) {
            auto a = index.model()->data(index.model()->index(index.row(), 0));
            DLineEdit *pEdit = new DLineEdit();
            pEdit->setGeometry(50, 10, 10, 10);
        }
    }

    return true;
}

QWidget *TaskDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    if (index.column() != 0) {
        return nullptr;
    }
    QString size = index.model()->data(index.model()->index(index.row(), 3)).toString();
    if (size.isEmpty()) {
        return nullptr;
    }
    DLineEdit *pEdit = new DLineEdit(parent);
    QRegExp regx("[^\\\\/\':\\*\\?\"<>|#%]+"); //屏蔽特殊字符
    QValidator *validator = new QRegExpValidator(regx, pEdit);
    pEdit->lineEdit()->setValidator(validator);
    pEdit->lineEdit()->setMaxLength(83);
    connect(pEdit, &DLineEdit::textChanged, this, [=](QString filename) {
        DLineEdit *pEdit = qobject_cast<DLineEdit *>(sender());
        if (pEdit == nullptr) {
            return;
        }
        setModelData(pEdit, nullptr, index);

        QString curName;
        DAlertControl *alertControl = new DAlertControl(pEdit, pEdit);
        QString typeName = filename + "." + index.model()->data(index.model()->index(index.row(), 2)).toString();
        for (int i = 0; i < index.model()->rowCount(); i++) {
            curName = index.model()->data(index.model()->index(i, 1)).toString() + "." + index.model()->data(index.model()->index(i, 2)).toString();
            if (curName == typeName && i != index.row()) {
                pEdit->showAlertMessage(tr("Duplicate name!"),
                                        pEdit->parentWidget()->parentWidget(), -1);
                alertControl->setMessageAlignment(Qt::AlignLeft);
                break;
            } else {
                pEdit->hideAlertMessage();
            }
        }
    });
    connect(pEdit, &DLineEdit::returnPressed, this, [=]() {
        DLineEdit *pEdit = qobject_cast<DLineEdit *>(sender());
        if (pEdit == nullptr) {
            return;
        }
        setModelData(pEdit, nullptr, index);
    });



    pEdit->setGeometry(150, 10, 50, 10);
    pEdit->setGeometry(0, 0, 0, 0);

    QString FilePath = index.data().toString();
    return pEdit;
}

void TaskDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DLineEdit *pEdit = qobject_cast<DLineEdit *>(editor);
    QString str = index.model()->data(index.model()->index(index.row(), 1)).toString();
    m_curName = str;
    pEdit->setText(str);
}

void TaskDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    DLineEdit *pEdit = qobject_cast<DLineEdit *>(editor);
    if (pEdit == nullptr) {
        return;
    }
    QString str = pEdit->text();
    int row = index.row();
    if (str.isEmpty()) {
        ((CreateTaskWidget *)m_dialog)->setUrlName(row, m_curName);
        return;
    }
    QString curName;
    QString typeName = str + "." + index.model()->data(index.model()->index(index.row(), 2)).toString();
    for (int i = 0; i < index.model()->rowCount(); i++) {
        curName = index.model()->data(index.model()->index(i, 1)).toString() + "." + index.model()->data(index.model()->index(i, 2)).toString();
        if (curName == typeName) {
            ((CreateTaskWidget *)m_dialog)->setUrlName(row, m_curName);
            return;
        }
    }

    ((CreateTaskWidget *)m_dialog)->setUrlName(row, str);
}

void TaskDelegate::onhoverChanged(const QModelIndex &index)
{
    m_hoverRow = index.row();
}
