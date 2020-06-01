#include "messagebox.h"

MessageBox::MessageBox(DDialog *parent) : DDialog(parent)
{

}

void MessageBox::setWarings(QString warningMsg,QString surebtntext,QString cancelbtntext,int sameurl_count,QList<QString> same_url_list)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    this->setTitle(tr("Warning"));

    this->addLabel(warningMsg);
    this->addSpacing(10);
    if(sameurl_count!=0)
    {
        DTextEdit *url_text = new DTextEdit(this);
        url_text->setReadOnly(true);
        url_text->setFixedSize(QSize(454,154));

        QPalette pal;
        pal.setColor(QPalette::Base, QColor(0,0,0,20));
        url_text->setPalette(pal);
        for(int i=0;i<same_url_list.size();i++)
        {
            url_text->append(same_url_list.at(i));
        }
        this->addContent(url_text);
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
void MessageBox::setDelete(bool permanentl)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    m_deleteFlag=permanentl;
    QString show_title=tr("Are you soure to delete this download task?");
    this->setTitle(show_title);
    QString show_msg=tr("After deleting,the local file will be deleted");
    //this->addLabel(show_title);
    this->addSpacing(10);
    if(permanentl)
        this->addLabel(show_msg);
    else
    {
        this->addCheckbox(tr("delete local file at the same time"));
    }
    this->addSpacing(10);
    this->addButton(tr("Cancel"));
    if(permanentl)
    {
        this->addButton(tr("Delete permanently"),true,ButtonType::ButtonWarning);
    }
    else {
        this->addButton(tr("Delete"),true,ButtonType::ButtonWarning);

    }
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::deleteBtn);
}
void MessageBox::setClear()
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));
    QString show_title=tr("Are you soure to clear all tasks in recycle bin?");
    this->addLabel(show_title);
    this->addSpacing(10);
    this->addCheckbox(tr("delete local file at the same time"));
    this->addButton(tr("Cancel"));
    this->addButton(tr("Delete"));
    connect(this,&MessageBox::buttonClicked,this,&MessageBox::clearBtn);
}
void MessageBox::setReName(QString title, QString surebtntext, QString cancelbtntext, QString oldname)
{
    this->setIcon(QIcon::fromTheme(":/icons/icon/ndm_messagebox_logo_32px.svg"));

    this->setCloseButtonVisible(false);
    this->setTitle(tr("rename"));
    newName_lineedit = new DLineEdit();
    newName_lineedit->setText(oldname);
    newName_lineedit->setFixedWidth(400);
    this->addContent(newName_lineedit,Qt::AlignHCenter);
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
    rename_sure_button = new QPushButton(button_box);
    rename_sure_button->setText(surebtntext);
    connect(rename_sure_button,&DPushButton::clicked,this,&MessageBox::renameSureBtn);
    layout->addWidget(rename_sure_button);
    this->addContent(button_box);
    connect(newName_lineedit,&DLineEdit::textChanged,this,&MessageBox::get_renameLineEdit_changed);
}

void MessageBox::addLabel(QString text)
{
    DLabel * title= new DLabel(this);
    title->setText(text);
    this->addContent(title,Qt::AlignHCenter);
}

void MessageBox::addCheckbox(QString checkboxText)
{
    m_checkbox = new DCheckBox(this);
    m_checkbox->setText(checkboxText);
    this->addContent(m_checkbox,Qt::AlignHCenter);
}

void MessageBox::get_renameLineEdit_changed(const QString &text)
{

    QString real_name= QString(text).left(text.lastIndexOf('.'));

    if(!text.isEmpty()&&!real_name.isEmpty())
    {
        rename_sure_button->setEnabled(true);
    }
    else {
        rename_sure_button->setEnabled(false);
    }
}

void MessageBox::renameSureBtn()
{
    QString newname=newName_lineedit->text();
    if(newname.contains("\\")||newname.contains("/"))
    {
        MessageBox *msg=new MessageBox();
        msg->setWarings(tr("file name can not containts '\\' or '/' "),tr("sure"));
        msg->exec();
        return;
    }
    emit ReName_sig(newname);
   this->close();
}

void MessageBox::clearBtn(int index)
{
    if(index==1)
    {
        bool ischecked;
        ischecked= m_checkbox->isChecked();
        emit ClearRecycle_sig(ischecked);
    }
    this->close();
}

void MessageBox::deleteBtn(int index)
{
    if(index==1)
    {

    QAbstractButton *button= this->getButton(index);
        button->setEnabled(false);
        if(m_deleteFlag)
             emit DeleteDownload_sig(true,m_deleteFlag);
        else
        {
            bool ischecked;
            ischecked= m_checkbox->isChecked();
            emit DeleteDownload_sig(ischecked,m_deleteFlag);

        }
    }
    this->close();
}
