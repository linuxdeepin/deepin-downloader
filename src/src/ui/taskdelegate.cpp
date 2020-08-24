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
#include <QDir>

TaskDelegate::TaskDelegate(DDialog *dialog)
{
    m_dialog = dialog;
    m_checkBtn = new QCheckBox;



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

    if(index.row() == m_hoverRow) {
        painter->fillRect(option.rect, m_hoverColor);//QColor(0,0,0,13)QColor(255,255,255,26)
    }
    else {
        if(index.row() % 2 == 1) {
            painter->fillRect(option.rect, option.palette.base());
        }
        else {
            painter->fillRect(option.rect, option.palette.alternateBase());
        }
    }

    QString size= index.model()->data(index.model()->index(index.row(), 3)).toString();
    if(size == "") {
        painter->fillRect(option.rect, QColor(0,0,0,10));//QColor(0,0,0,13)QColor(255,255,255,26)
        painter->setOpacity(1);
    }

    painter->restore();
    painter->save();

    if(index.column() == 0) {
        QStyleOptionButton checkBoxStyle;
        checkBoxStyle.state = index.data().toString() == "1" ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyle.state |= QStyle::State_Enabled;
        checkBoxStyle.rect = option.rect;
        checkBoxStyle.rect.setX(option.rect.x() + 5);//option.rect.width() / 2 - 6
        checkBoxStyle.rect.setWidth(15);


        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter, m_checkBtn);

        QString ext = index.model()->data(index.model()->index(index.row(), 2)).toString();
        QFileIconProvider prov;
        QString tempFilePath = QDir::tempPath() + QDir::separator() + QCoreApplication::applicationName() + "_temp.";
        QFileInfo fi(tempFilePath + ext);
        QIcon icon = prov.icon(fi);

        if(icon.isNull()) {
            icon = prov.icon(QFileIconProvider::File);
        }
        QPixmap pic = icon.pixmap(20, 20);

        painter->drawPixmap(option.rect.x() + 32, option.rect.y() + 13, pic);

        painter->setPen(Qt::darkGray);

        painter->setPen(size == ""? QColor(224,32,32,40) :Qt::darkGray);

        QString text = painter->fontMetrics().elidedText(index.model()->data(index.model()->index(index.row(), 1)).toString(), Qt::ElideRight, option.rect.width() - 55);
        painter->drawText(option.rect.x() + 55, option.rect.y() + 28, text);


    }
    else {
        painter->setPen(Qt::darkGray);
        painter->setPen(size == ""? QColor(224,32,32,40) :Qt::darkGray);
        QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);
        painter->drawText(option.rect.x() + 5, option.rect.y() + 28, text);
    }
    painter->restore();
}

bool TaskDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    QString size= index.model()->data(index.model()->index(index.row(), 3)).toString();
    if(size == "") {

            return false;
    }

    if(index.column() == 0) {
        QRect rect(option.rect);
        rect.setX(10);
        rect.setWidth(15);
        if(event->type() == QEvent::MouseButtonPress
                && mouseEvent->button() == Qt::LeftButton
                && rect.contains(mouseEvent->pos())) {
            QString v = index.data().toString();
            model->setData(index, QVariant(v == "1" ? "0" : "1"), Qt::EditRole);

            ((CreateTaskWidget*)m_dialog)->updateSelectedInfo();
            return false;
        }
        else if (event->type() == QEvent::MouseButtonDblClick
                 && !rect.contains(mouseEvent->pos())) {
            int a =10;

        }
    }

    return false;
}

QWidget *TaskDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    Q_UNUSED(option);

//    static bool  firstInside = true;
//    firstInside = true;
//    DLineEdit *pEdit = new DLineEdit(parent);
//    connect(pEdit, &DLineEdit::textChanged, this, [=](QString filename){
//        DLineEdit *pEdit=qobject_cast<DLineEdit *>(sender());
//        QString FilePath = index.data(TableModel::SavePath).toString();
//        FilePath = FilePath.left(FilePath.lastIndexOf("/") + 1);
//        FilePath = FilePath + filename;
//        QFileInfo file(FilePath);
//        if(file.isFile() && !firstInside){
//            pEdit->showAlertMessage(tr("Duplicate name!"), -1);
//        } else {
//            pEdit->hideAlertMessage();
//        }
//        firstInside = false;
//    });
//    pEdit->resize(parent->size());
//    QString FilePath = index.data(TableModel::SavePath).toString();
//    return pEdit;
    return  NULL;
}

void TaskDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
//    DLineEdit *pEdit =qobject_cast<DLineEdit *>(editor);
//    QString str = index.data(TableModel::FileName).toString();

//    QMimeDatabase db;
//    QString mime = db.suffixForFileName(str);
//    str = str.left(str.size() - mime.size() -1);
//    pEdit->setText(str);
}

void TaskDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
//    DLineEdit *pEdit =qobject_cast<DLineEdit *>(editor);
//    QString str = index.data(TableModel::FileName).toString();
//    QMimeDatabase db;
//    QString mime = db.suffixForFileName(str);
//    QString fileName = pEdit->text() + "." + mime;

//    QString FilePath = index.data(TableModel::SavePath).toString();
//    FilePath = FilePath.left(FilePath.lastIndexOf("/") + 1);
//    FilePath = FilePath + fileName;
//    if(!QFileInfo::exists(FilePath)){
//        QFile::rename(index.data(TableModel::SavePath).toString(), FilePath);
//        model->setData(index, fileName, TableModel::FileName);
//        model->setData(index, FilePath, TableModel::SavePath);
//        Task task;
//        DBInstance::getTaskByID(index.data(TableModel::taskId).toString(),task);
//        task.downloadPath = FilePath;
//        task.downloadFilename = fileName;
//        DBInstance::updateTaskByID(task);
//    }
}


void TaskDelegate::onhoverChanged(const QModelIndex &index)
{
    m_hoverRow = index.row();
}




