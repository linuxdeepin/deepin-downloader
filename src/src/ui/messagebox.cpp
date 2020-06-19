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

}

void MessageBox::setWarings(QString warningMsg,QString surebtntext,QString cancelbtntext,int sameurlCount,QList<QString> sameUrlList)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    this->setTitle(tr("Warning"));

    this->addLabel(warningMsg);
    this->addSpacing(10);
    if(sameurlCount!=0)
    {
        DTextEdit *urlText = new DTextEdit(this);
        urlText->setReadOnly(true);
        urlText->setFixedSize(QSize(454,154));

        QPalette pal;
        pal.setColor(QPalette::Base, QColor(0,0,0,20));
        urlText->setPalette(pal);
        for(int i=0;i<sameUrlList.size();i++)
        {
            urlText->append(sameUrlList.at(i));
        }
        this->addContent(urlText);
    }
    if(cancelbtntext!="")
    {
        this->addButton(cancelbtntext);

    }
    this->addButton(surebtntext);
    connect(this,&MessageBox::buttonClicked,this,
            [=]()
            {
                this->close();
    });
}

void MessageBox::setRedownload(QList<QString> sameUrlList)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    this->setTitle(tr("Warning"));

    this->addLabel("Task exist. Download again?");
    this->addSpacing(10);
    DTextEdit *urlText = new DTextEdit(this);
    urlText->setReadOnly(true);
    urlText->setFixedSize(QSize(454,154));

    QPalette pal;
    pal.setColor(QPalette::Base, QColor(0,0,0,20));
    urlText->setPalette(pal);
    for(int i=0;i<sameUrlList.size();i++)
    {
        urlText->append(sameUrlList.at(i));
    }
    this->addContent(urlText);
    addButton("Ok");
    addButton("Cancel");
    connect(this,&MessageBox::buttonClicked,this,
            [=]()
            {
                emit reDownloadSig();
    });
}
void MessageBox::setDelete(bool permanentl)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    m_bDeleteFlag=permanentl;

    if(permanentl)
    {
        QString show_title=tr("Are you sure you want to delete this download task permanently?");
        this->setTitle(show_title);
        this->addSpacing(10);
        QString show_msg=tr("Local files will be deleted at the same time.");
        this->addLabel(show_msg);
    }
    else
    {
        QString showTitle = tr("Are you sure you want to delete this download task?");
        this->setTitle(showTitle);
        this->addSpacing(10);
        this->addCheckbox(tr("Delete local files"));
    }
    this->addSpacing(10);
    this->addButton(tr("Cancel"));
    if(permanentl)
    {
        this->addButton(tr("Permanently delete"),true,ButtonType::ButtonWarning);
    }
    else {
        this->addButton(tr("Delete"),true,ButtonType::ButtonWarning);

    }
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::deleteBtn);
}
void MessageBox::setClear()
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    QString show_title=tr("Are you sure you want to delete all tasks in the trash?");
    this->addLabel(show_title);
    this->addSpacing(10);
    this->addCheckbox(tr("Delete local files"));
    this->addButton(tr("Cancel"));
    this->addButton(tr("Empty"));
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::clearBtn);
}
void MessageBox::setReName(QString title, QString surebtntext, QString cancelbtntext, QString oldname)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    this->setCloseButtonVisible(false);
    this->setTitle(tr("Rename"));
    m_pNewnameLineedit = new DLineEdit();
    m_pNewnameLineedit->setText(oldname);
    m_pNewnameLineedit->setFixedWidth(400);
    this->addContent(m_pNewnameLineedit,Qt::AlignHCenter);
    this->addSpacing(20);
    QWidget *button_box= new QWidget(this);
    QHBoxLayout *layout=new QHBoxLayout(button_box);
    QPushButton *cancel_button = new QPushButton(button_box);
    cancel_button->setText(cancelbtntext);
    connect(cancel_button,&DPushButton::clicked,this,
            [=]()
            {
                this->close();
            });
    layout->addWidget(cancel_button);
    m_pRenameSureButton = new QPushButton(button_box);
    m_pRenameSureButton->setText(surebtntext);
    connect(m_pRenameSureButton,&DPushButton::clicked,this,&MessageBox::renameSureBtn);
    layout->addWidget(m_pRenameSureButton);
    this->addContent(button_box);
    connect(m_pNewnameLineedit,&DLineEdit::textChanged,this,&MessageBox::getRenamelineeditChanged);
}

void MessageBox::setExit()
{
    setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    setTitle(tr("You want to"));
    addSpacing(10);
    addRadioGroup(tr("Exit"), tr("Minimize to System Tray"));
    addSpacing(10);
    addCheckbox(tr("Don't ask again"));
    this->addButton(tr("Cancel"));
    this->addButton(tr("Confirm"));

    connect(this,&MessageBox::buttonClicked,this,&MessageBox::ExitBtn);
}

void MessageBox::addLabel(QString text)
{
    DLabel * title= new DLabel(this);
    title->setText(text);
    this->addContent(title,Qt::AlignHCenter);
}

void MessageBox::addRadioGroup(QString quitText, QString minText)
{
    int status = Settings::getInstance()->getCloseMainWindowSelected();
    m_pButtonQuit = new  DRadioButton(quitText);
    m_pButtonMin = new  DRadioButton(minText);
    addContent(m_pButtonQuit,Qt::AlignLeft);
    addSpacing(5);
    addContent(m_pButtonMin,Qt::AlignLeft);
    if(status){
        m_pButtonQuit->setChecked(true);
        m_pButtonMin->setChecked(false);
    } else {
        m_pButtonMin->setChecked(true);
        m_pButtonQuit->setChecked(false);
    }
    connect(m_pButtonQuit, &DRadioButton::clicked, this,
            [=]()
            {
                m_pButtonMin->setChecked(false);
            });
    connect(m_pButtonMin, &DRadioButton::clicked, this,
            [=]()
            {
                m_pButtonQuit->setChecked(false);
            });
}

void MessageBox::addCheckbox(QString checkboxText)
{
    m_pCheckBox = new DCheckBox(this);
    m_pCheckBox->setText(checkboxText);
    this->addContent(m_pCheckBox,Qt::AlignHCenter);
}

void MessageBox::getRenamelineeditChanged(const QString &text)
{

    QString real_name= QString(text).left(text.lastIndexOf('.'));

    if(!text.isEmpty()&&!real_name.isEmpty())
    {
        m_pRenameSureButton->setEnabled(true);
    }
    else {
        m_pRenameSureButton->setEnabled(false);
    }
}

void MessageBox::renameSureBtn()
{
    QString newname=m_pNewnameLineedit->text();
    if(newname.contains("\\")||newname.contains("/"))
    {
        MessageBox *msg=new MessageBox();
        msg->setWarings(tr("file name can not containts '\\' or '/' "),tr("sure"));
        msg->exec();
        return;
    }
    emit RenameSig(newname);
   this->close();
}

void MessageBox::clearBtn(int index)
{
    if(index==1)
    {
        bool ischecked;
        ischecked= m_pCheckBox->isChecked();
        emit ClearrecycleSig(ischecked);
    }
    this->close();
}

void MessageBox::deleteBtn(int index)
{
    if(index==1)
    {

    QAbstractButton *button= this->getButton(index);
        button->setEnabled(false);
        if(m_bDeleteFlag)
             emit DeletedownloadSig(true,m_bDeleteFlag);
        else
        {
            bool ischecked;
            ischecked= m_pCheckBox->isChecked();
            emit DeletedownloadSig(ischecked,m_bDeleteFlag);

        }
    }
    this->close();
}

void MessageBox::ExitBtn(int index)
{
    if(index==1)
    {
        if(m_pButtonMin->isChecked()) {
            Settings::getInstance()->setCloseMainWindowSelected(0);
        } else {
            Settings::getInstance()->setCloseMainWindowSelected(1);
        }
        if(m_pCheckBox->isChecked()) {
            Settings::getInstance()->setIsShowTip(false);
        }
        emit signalCloseConfirm();
    }
    close();
}
