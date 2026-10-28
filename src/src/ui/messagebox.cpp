// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file messagebox.cpp
 *
 * @brief 提示窗口类
 *
 * @date 2020-06-09 10:52
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

#include "messagebox.h"
#include "settings.h"
#include <QPushButton>
#include "ddlog.h"

MessageBox::MessageBox(QWidget *parent)
    : DDialog(parent)
{
    qCDebug(downloaderLog) << "MessageBox constructor";
    setObjectName("messageBox");
    setMaximumWidth(600);
}

void MessageBox::setWarings(QString warningMsg, QString surebtntext, QString cancelbtntext, int sameurlCount, QList<QString> sameUrlList)
{
    qCDebug(downloaderLog) << "setWarings called with sameurlCount:" << sameurlCount;

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));
    setObjectName("messageBox");

    addLabel(warningMsg);
    addSpacing(10);
    if (sameurlCount != 0) {
        DTextEdit *urlText = new DTextEdit(this);
        urlText->setReadOnly(true);
        urlText->setFixedSize(QSize(454, 154));

        QPalette pal;
        pal.setColor(QPalette::Base, QColor(0, 0, 0, 20));
        urlText->setPalette(pal);
        for (int i = 0; i < sameUrlList.size(); i++) {
            urlText->append(sameUrlList.at(i));
        }
        addContent(urlText);
    }
    if (!cancelbtntext.isEmpty()) {
        QAbstractButton *btn1 = getButton(addButton(cancelbtntext));
        btn1->setObjectName("cancel");
        btn1->setAccessibleName("cancel");
        QAbstractButton *btn2 = getButton(addButton(surebtntext, true, ButtonType::ButtonWarning));
        btn2->setObjectName("Confirm");
        btn2->setAccessibleName("Confirm");
    } else {
        QAbstractButton *btn2 = getButton(addButton(surebtntext, true, ButtonType::ButtonWarning));
        btn2->setObjectName("OK");
        btn2->setObjectName("OK");
    }

    connect(this, &MessageBox::buttonClicked, this,
            [=]() {
                close();
            });
    qCDebug(downloaderLog) << "[MessageBox] setWarings function ended";
}

void MessageBox::setRedownload(const QString sameUrl, bool ret, bool isShowRedownload)
{
    qCDebug(downloaderLog) << "setRedownload called with ret:" << ret << "isShowRedownload:" << isShowRedownload;

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    if (ret) {
        qCDebug(downloaderLog) << "[MessageBox] ret is true, setting title to Download Again";
        setTitle(tr("Download Again"));
        addLabel(tr("Do you want to delete the downloaded files and download again?"));
    } else {
        qCDebug(downloaderLog) << "[MessageBox] ret is false, setting title to These tasks already exist";
        setTitle(tr("These tasks already exist"));
        if (!isShowRedownload) {
            addLabel(tr("Delete the downloaded files and download again?"));
        }
    }
    addSpacing(10);
    DTextEdit *urlText = new DTextEdit(this);
    urlText->setReadOnly(true);
    urlText->setFixedSize(QSize(454, 154));
    urlText->setText(sameUrl);
    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0, 0, 0, 20));
    addContent(urlText);

    if (isShowRedownload) {
        qCDebug(downloaderLog) << "[MessageBox] isShowRedownload is true, adding OK button";
        QAbstractButton *btn1 = getButton(addButton(tr("OK")));
        btn1->setObjectName("OK");
        btn1->setAccessibleName("OK");
    } else {
        qCDebug(downloaderLog) << "[MessageBox] isShowRedownload is false, adding Cancel button";
        QAbstractButton *btn1 = getButton(addButton(tr("Cancel")));
        btn1->setObjectName("cancel");
        btn1->setAccessibleName("cancele");
        QAbstractButton *btn2 = getButton(addButton(tr("Download Again"), true, ButtonType::ButtonWarning));
        btn2->setObjectName("redownload");
        btn2->setAccessibleName("redownload");
    }
    qCDebug(downloaderLog) << "[MessageBox] setRedownload function ended";
}

void MessageBox::setUnusual(const QString &taskId, QString taskList)
{
    qCDebug(downloaderLog) << "setUnusual called for taskId:" << taskId;

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));

    addLabel(tr("Download error"));
    addSpacing(10);
    DTextEdit *urlText = new DTextEdit(this);
    urlText->setReadOnly(true);
    urlText->setFixedSize(QSize(454, 154));
    urlText->setText(taskList);
    addContent(urlText);
    //if(taskList.split("\n").count() <= 1) {
    QAbstractButton *btn = getButton(addButton(tr("Download Again")));
    btn->setObjectName("DownloadAgain");
    btn->setAccessibleName("DownloadAgain");
    //}
    QAbstractButton *btn1 = getButton(addButton(tr("Delete All")));
    btn1->setObjectName("DeleteAll");
    btn1->setAccessibleName("DeleteAll");
    connect(this, &MessageBox::buttonClicked, this,
            [=](int index) {
                emit unusualConfirm(index, taskId);
                close();
            });
    qCDebug(downloaderLog) << "[MessageBox] setUnusual function ended";
}

void MessageBox::setDelete(bool permanentl, bool checked)
{
    qCDebug(downloaderLog) << "setDelete called with permanent:" << permanentl << "checked:" << checked;

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    m_DeleteFlag = permanentl;

    if (permanentl) {
        qCDebug(downloaderLog) << "[MessageBox] permanentl is true, setting title to Are you sure you want to delete this download task permanently?";
        QString show_title = tr("Are you sure you want to delete this download task permanently?");
        setTitle(show_title);
        addSpacing(10);
        QString show_msg = tr("Local files will be deleted at the same time.");
        addLabel(show_msg);
    } else {
        qCDebug(downloaderLog) << "[MessageBox] permanentl is false, setting title to Are you sure you want to delete this download task?";
        QString showTitle = tr("Are you sure you want to delete this download task?");
        setTitle(showTitle);
        addSpacing(10);
        if (checked) {
            qCDebug(downloaderLog) << "[MessageBox] checked is true, adding Delete local files checkbox";
            addCheckbox(tr("Delete local files"), true);
        } else {
            qCDebug(downloaderLog) << "[MessageBox] checked is false, adding Delete local files checkbox";
            addCheckbox(tr("Delete local files"));
        }
    }
    addSpacing(10);

    QAbstractButton *btn = getButton(addButton(tr("Cancel")));
    btn->setObjectName("cancel");
    btn->setAccessibleName("cancel");
    if (permanentl) {
        qCDebug(downloaderLog) << "[MessageBox] permanentl is true, adding Permanently Delete button";
        QAbstractButton *btn = getButton(addButton(tr("Permanently Delete"), true, ButtonType::ButtonWarning));
        btn->setObjectName("delete");
        btn->setAccessibleName("delete");
    } else {
        qCDebug(downloaderLog) << "[MessageBox] permanentl is false, adding Delete button";
        QAbstractButton *btn = getButton(addButton(tr("Delete"), true, ButtonType::ButtonWarning));
        btn->setObjectName("delete");
        btn->setAccessibleName("delete");
    }
    connect(this, &MessageBox::buttonClicked, this, &MessageBox::onDeleteBtnClicked);
    qCDebug(downloaderLog) << "[MessageBox] setDelete function ended";
}
void MessageBox::setClear()
{
    qCDebug(downloaderLog) << "[MessageBox] setClear function started";

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    QString show_title = tr("Are you sure you want to delete all tasks in the trash?");
    addLabel(show_title);
    addSpacing(10);
    addCheckbox(tr("Delete local files"));
    QAbstractButton *btn1 = getButton(addButton(tr("Cancel")));
    btn1->setObjectName("cancel");
    btn1->setAccessibleName("cancel");
    QAbstractButton *btn2 = getButton(addButton(tr("Empty"), true, ButtonType::ButtonWarning));
    btn2->setObjectName("empty");
    btn2->setAccessibleName("empty");
    connect(this, &MessageBox::buttonClicked, this, &MessageBox::onClearBtnClicked);
    qCDebug(downloaderLog) << "[MessageBox] setClear function ended";
}

void MessageBox::setExit()
{
    qCDebug(downloaderLog) << "[MessageBox] setExit function started";

    setIcon(QIcon(":/icons/icon/downloader5.svg"));

    setTitle(tr("Please choose your action"));
    addSpacing(10);
    addRadioGroup(tr("Exit"), tr("Minimize to system tray"));
    addSpacing(10);
    addCheckbox(tr("Do not ask again"));
    addButton(tr("Cancel"));
    addButton(tr("Confirm"));

    connect(this, &MessageBox::buttonClicked, this, &MessageBox::onExitBtnClicked);
    qCDebug(downloaderLog) << "[MessageBox] setExit function ended";
}

void MessageBox::addLabel(QString text)
{
    qCDebug(downloaderLog) << "[MessageBox] addLabel function started with text:" << text;

    DLabel *title = new DLabel(this);
    title->setAccessibleName("messageBoxTittle");
    title->setText(text);
    addContent(title, Qt::AlignHCenter);
    qCDebug(downloaderLog) << "[MessageBox] addLabel function ended";
}

void MessageBox::addRadioGroup(QString quitText, QString minText)
{
    qCDebug(downloaderLog) << "[MessageBox] addRadioGroup function started with quitText:" << quitText << "minText:" << minText;

    int status = Settings::getInstance()->getCloseMainWindowSelected();
    m_ButtonQuit = new DRadioButton(quitText);
    m_ButtonMin = new DRadioButton(minText);
    addContent(m_ButtonQuit, Qt::AlignLeft);
    addSpacing(5);
    addContent(m_ButtonMin, Qt::AlignLeft);
    if (status) {
        m_ButtonQuit->setChecked(true);
        m_ButtonMin->setChecked(false);
    } else {
        m_ButtonMin->setChecked(true);
        m_ButtonQuit->setChecked(false);
    }
    connect(m_ButtonQuit, &DRadioButton::clicked, this,
            [=]() {
                m_ButtonMin->setChecked(false);
            });
    connect(m_ButtonMin, &DRadioButton::clicked, this,
            [=]() {
                m_ButtonQuit->setChecked(false);
            });
    qCDebug(downloaderLog) << "[MessageBox] addRadioGroup function ended";
}

void MessageBox::addCheckbox(QString checkboxText, bool checked)
{
    qCDebug(downloaderLog) << "[MessageBox] addCheckbox function started with text:" << checkboxText << "checked:" << checked;

    m_CheckBox = new DCheckBox(this);
    m_CheckBox->setText(checkboxText);
    m_CheckBox->setAccessibleName("CheckBox");
    if (checked) {
        m_CheckBox->setCheckState(Qt::Checked);
    }
    addContent(m_CheckBox, Qt::AlignLeft);
    qCDebug(downloaderLog) << "[MessageBox] addCheckbox function ended";
}

void MessageBox::onRenamelineeditChanged(const QString &text)
{
    qCDebug(downloaderLog) << "[MessageBox] onRenamelineeditChanged function started with text:" << text;
    QString real_name = QString(text).left(text.lastIndexOf('.'));

    if (!text.isEmpty() && !real_name.isEmpty()) {
        m_RenameSureButton->setEnabled(true);
    } else {
        m_RenameSureButton->setEnabled(false);
    }
    qCDebug(downloaderLog) << "[MessageBox] onRenamelineeditChanged function ended";
}

void MessageBox::onRenameSureBtnClicked()
{
    qCDebug(downloaderLog) << "[MessageBox] onRenameSureBtnClicked function started";
    QString newname = m_NewnameLineedit->text();
    if (newname.contains("\\") || newname.contains("/")) {
        MessageBox *msg = new MessageBox();
        msg->setWarings(tr("The file name cannot contain a backslash (\\) or slash (/)"), tr("OK"));
        msg->exec();
        qCDebug(downloaderLog) << "[MessageBox] onRenameSureBtnClicked function ended (invalid filename)";
        return;
    }
    emit Rename(newname);
    close();
    qCDebug(downloaderLog) << "[MessageBox] onRenameSureBtnClicked function ended";
}

void MessageBox::onClearBtnClicked(int index)
{
    qCDebug(downloaderLog) << "[MessageBox] onClearBtnClicked function started with index:" << index;

    if (index == 1) {
        bool ischecked;
        ischecked = m_CheckBox->isChecked();
        emit Clearrecycle(ischecked);
    }
    close();
    qCDebug(downloaderLog) << "[MessageBox] onClearBtnClicked function ended";
}

void MessageBox::onDeleteBtnClicked(int index)
{
    qCDebug(downloaderLog) << "[MessageBox] onDeleteBtnClicked function started with index:" << index << "m_DeleteFlag:" << m_DeleteFlag;

    if (index == 1) {
        qCDebug(downloaderLog) << "[MessageBox] index is 1, calling Deletedownload";
        QAbstractButton *button = getButton(index);
        button->setEnabled(false);
        if (m_DeleteFlag)
            emit Deletedownload(true, m_DeleteFlag);
        else {
            qCDebug(downloaderLog) << "[MessageBox] m_DeleteFlag is false, calling Deletedownload with ischecked";
            bool ischecked;
            ischecked = m_CheckBox->isChecked();
            emit Deletedownload(ischecked, m_DeleteFlag);
        }
    }
    close();
    qCDebug(downloaderLog) << "[MessageBox] onDeleteBtnClicked function ended";
}

void MessageBox::onExitBtnClicked(int index)
{
    qCDebug(downloaderLog) << "[MessageBox] onExitBtnClicked function started with index:" << index;

    if (index == 1) {
        qCDebug(downloaderLog) << "[MessageBox] index is 1, calling closeConfirm";
        if (m_CheckBox->isChecked()) {
            qCDebug(downloaderLog) << "[MessageBox] m_CheckBox is checked, calling setIsShowTip";
            Settings::getInstance()->setIsShowTip(false);
            if (m_ButtonMin->isChecked()) {
                qCDebug(downloaderLog) << "[MessageBox] m_ButtonMin is checked, calling setCloseMainWindowSelected";
                Settings::getInstance()->setCloseMainWindowSelected(0);
            } else {
                qCDebug(downloaderLog) << "[MessageBox] m_ButtonMin is not checked, calling setCloseMainWindowSelected";
                Settings::getInstance()->setCloseMainWindowSelected(1);
            }
        }
        emit closeConfirm(m_ButtonMin->isChecked() ? 0 : 1);
    }
    close();
    qCDebug(downloaderLog) << "[MessageBox] onExitBtnClicked function ended";
}

void MessageBox::setFolderDenied()
{
    qCDebug(downloaderLog) << "[MessageBox] setFolderDenied function started";

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Permission denied"));

    addLabel(tr("Please try another folder"));
    addSpacing(10);
    addButton(tr("OK"));
    connect(this, &MessageBox::buttonClicked, this,
            [=]() {
                close();
    });
    qCDebug(downloaderLog) << "[MessageBox] setFolderDenied function ended";
}

void MessageBox::setFolderNotExists()
{
    qCDebug(downloaderLog) << "[MessageBox] setFolderNotExists called";

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Path Error"));

    addLabel(tr("The storage folder does not exist, please check and try again"));
    addSpacing(10);
    addButton(tr("OK"));
    connect(this, &MessageBox::buttonClicked, this,
            [=]() {
                close();
    });
    qCDebug(downloaderLog) << "[MessageBox] setFolderDenied function ended";
}

void MessageBox::setNetWorkError(QString warningMsg)
{
    qCDebug(downloaderLog) << "[MessageBox] setNetWorkError function started with warningMsg:" << warningMsg;

    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

   // setTitle(warningMsg);
    setObjectName("messageBox");

    addLabel(warningMsg);
    addSpacing(10);
    addButton(tr("OK"));
//        QAbstractButton *btn2 = getButton(addButton(surebtntext, true, ButtonType::ButtonWarning));
//        btn2->setObjectName("OK");

    connect(this, &MessageBox::buttonClicked, this,
            [=]() {
                close();
            });
    qCDebug(downloaderLog) << "[MessageBox] setNetWorkError function ended";
}
