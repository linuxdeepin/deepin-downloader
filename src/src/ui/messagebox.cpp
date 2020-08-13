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

MessageBox::MessageBox(DDialog *parent) : DDialog(parent)
{
        setMaximumWidth(600);
}

void MessageBox::setWarings(QString warningMsg,QString surebtntext,QString cancelbtntext,int sameurlCount,QList<QString> sameUrlList)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));

    addLabel(warningMsg);
    addSpacing(10);
    if(sameurlCount!=0)
    {
        DTextEdit *urlText = new DTextEdit(this);
        urlText->setReadOnly(true);
        urlText->setFixedSize(QSize(454,154));

        QPalette pal;
        pal.setColor(QPalette::Base, QColor(0,0,0,20));
        urlText->setPalette(pal);
        for(int i=0;i<sameUrlList.size();i++){
            urlText->append(sameUrlList.at(i));
        }
        addContent(urlText);
    }
    if(cancelbtntext!="")
    {
        addButton(cancelbtntext);

    }
    addButton(surebtntext);
    connect(this,&MessageBox::buttonClicked,this,
            [=]()
            {
                close();
    });
}

void MessageBox::setRedownload(const QString sameUrl, QString fileName, QString type)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));

    addLabel(tr("Task exist. Download again?"));
    addSpacing(10);
    DTextEdit *urlText = new DTextEdit(this);
    urlText->setReadOnly(true);
    urlText->setFixedSize(QSize(454,154));

    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0,0,0,20));
    urlText->setPalette(pal);
    urlText->append(sameUrl);
    addContent(urlText);
    addButton(tr("Cancel"));
    addButton(tr("Ok"));
    connect(this,&MessageBox::buttonClicked,this,
            [=](int index)
            {
            if(index == 1){
                emit reDownload(sameUrl, fileName, type);
            }
            close();
    });
}

void MessageBox::setRedownload(const QString sameUrl)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));

    addLabel(tr("Task exist. Do you want to delete the downloaded files and download again?"));
    addSpacing(10);
    DTextEdit *urlText = new DTextEdit(this);
    urlText->setReadOnly(true);
    urlText->setFixedSize(QSize(454,154));
    urlText->setText(sameUrl);
    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0,0,0,20));
    addContent(urlText);
    addButton(tr("Cancel"));
    addButton(tr("Ok"));
}

void MessageBox::setUnusual(const QString &taskId)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("Warning"));

    addLabel(tr("Download Error. "));
    addSpacing(10);
    addButton(tr("Download again"));
    addButton(tr("Delete task"));
    connect(this,&MessageBox::buttonClicked,this,
            [=](int index)
            {
        emit unusualConfirm(index,taskId);
        close();
            });
}
void MessageBox::setDelete(bool permanentl, bool checked)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    m_DeleteFlag=permanentl;

    if(permanentl){
        QString show_title=tr("Are you sure you want to delete this download task permanently?");
        setTitle(show_title);
        addSpacing(10);
        QString show_msg=tr("Local files will be deleted at the same time.");
        addLabel(show_msg);
    }
    else{
        QString showTitle = tr("Are you sure you want to delete this download task?");
        setTitle(showTitle);
        addSpacing(10);
        if(checked){
            addCheckbox(tr("Delete local files"), true);
        }
        else{
            addCheckbox(tr("Delete local files"));
        }
    }
    addSpacing(10);
    addButton(tr("Cancel"));
    if(permanentl)
    {
        addButton(tr("Permanently delete"),true,ButtonType::ButtonWarning);
    }
    else {
        addButton(tr("Delete"),true,ButtonType::ButtonWarning);

    }
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::onDeleteBtnClicked);
}
void MessageBox::setClear()
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    QString show_title=tr("Are you sure you want to delete all tasks in the trash?");
    addLabel(show_title);
    addSpacing(10);
    addCheckbox(tr("Delete local files"));
    addButton(tr("Cancel"));
    addButton(tr("Empty"));
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::onClearBtnClicked);
}
void MessageBox::setReName(QString title, QString surebtntext, QString cancelbtntext, QString oldname)
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setCloseButtonVisible(false);
    setTitle(tr("Rename"));
    m_NewnameLineedit = new DLineEdit();
    m_NewnameLineedit->setText(oldname);
    m_NewnameLineedit->setFixedWidth(400);
    addContent(m_NewnameLineedit,Qt::AlignHCenter);
    addSpacing(20);
    QWidget *button_box= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(button_box);
    QPushButton *cancel_button = new QPushButton(button_box);
    cancel_button->setText(cancelbtntext);
    connect(cancel_button,&DPushButton::clicked,this,
            [=]()
            {
                close();
            });
    layout->addWidget(cancel_button);
    m_RenameSureButton = new QPushButton(button_box);
    m_RenameSureButton->setText(surebtntext);
    connect(m_RenameSureButton,&DPushButton::clicked,this,&MessageBox::onRenameSureBtnClicked);
    layout->addWidget(m_RenameSureButton);
    addContent(button_box);
    connect(m_NewnameLineedit,&DLineEdit::textChanged,this,&MessageBox::onRenamelineeditChanged);
}

void MessageBox::setExit()
{
    setIcon(QIcon(":/icons/icon/downloader5.svg"));

    setTitle(tr("You want to"));
    addSpacing(10);
    addRadioGroup(tr("Exit"), tr("Minimize to System Tray"));
    addSpacing(10);
    addCheckbox(tr("Don't ask again"));
    addButton(tr("Cancel"));
    addButton(tr("Confirm"));

    connect(this,&MessageBox::buttonClicked,this,&MessageBox::onExitBtnClicked);
}

void MessageBox::addLabel(QString text)
{
    DLabel * title= new DLabel(this);
    title->setText(text);
    addContent(title,Qt::AlignHCenter);
}

void MessageBox::addRadioGroup(QString quitText, QString minText)
{
    int status = Settings::getInstance()->getCloseMainWindowSelected();
    m_ButtonQuit = new  DRadioButton(quitText);
    m_ButtonMin = new  DRadioButton(minText);
    addContent(m_ButtonQuit,Qt::AlignLeft);
    addSpacing(5);
    addContent(m_ButtonMin,Qt::AlignLeft);
    if(status){
        m_ButtonQuit->setChecked(true);
        m_ButtonMin->setChecked(false);
    } else {
        m_ButtonMin->setChecked(true);
        m_ButtonQuit->setChecked(false);
    }
    connect(m_ButtonQuit, &DRadioButton::clicked, this,
            [=]()
            {
                m_ButtonMin->setChecked(false);
            });
    connect(m_ButtonMin, &DRadioButton::clicked, this,
            [=]()
            {
                m_ButtonQuit->setChecked(false);
            });
}

void MessageBox::addCheckbox(QString checkboxText, bool checked)
{
    m_CheckBox = new DCheckBox(this);
    m_CheckBox->setText(checkboxText);
    if(checked){
        m_CheckBox->setCheckState(Qt::Checked);
    }
    addContent(m_CheckBox,Qt::AlignHCenter);
}

void MessageBox::onRenamelineeditChanged(const QString &text)
{

    QString real_name= QString(text).left(text.lastIndexOf('.'));

    if(!text.isEmpty()&&!real_name.isEmpty()){
        m_RenameSureButton->setEnabled(true);
    }
    else {
        m_RenameSureButton->setEnabled(false);
    }
}

void MessageBox::onRenameSureBtnClicked()
{
    QString newname=m_NewnameLineedit->text();
    if(newname.contains("\\")||newname.contains("/")) {
        MessageBox *msg=new MessageBox();
        msg->setWarings(tr("file name can not containts '\\' or '/' "),tr("sure"));
        msg->exec();
        return;
    }
    emit Rename(newname);
   close();
}

void MessageBox::onClearBtnClicked(int index)
{
    if(index==1){
        bool ischecked;
        ischecked= m_CheckBox->isChecked();
        emit Clearrecycle(ischecked);
    }
    close();
}

void MessageBox::onDeleteBtnClicked(int index)
{
    if(index==1)
    {

    QAbstractButton *button= getButton(index);
        button->setEnabled(false);
        if(m_DeleteFlag)
             emit Deletedownload(true,m_DeleteFlag);
        else{
            bool ischecked;
            ischecked= m_CheckBox->isChecked();
            emit Deletedownload(ischecked,m_DeleteFlag);
        }
    }
    close();
}

void MessageBox::onExitBtnClicked(int index)
{
    if(index==1) {
        if(m_ButtonMin->isChecked()) {
            Settings::getInstance()->setCloseMainWindowSelected(0);
        } else {
            Settings::getInstance()->setCloseMainWindowSelected(1);
        }
        if(m_CheckBox->isChecked()) {
            Settings::getInstance()->setIsShowTip(false);
        }
        emit closeConfirm();
    }
    close();
}
