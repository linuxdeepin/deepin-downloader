
#include <iostream>
#include "gtest/gtest.h"
#include "func.h"
#include "messagebox.h"
#include <QObject>
#include <QPushButton>
class ut_messageBox : public ::testing::Test
{
protected:
    ut_messageBox()
    {
    }

    virtual ~ut_messageBox()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_messageBox, setUnusual)
{
    MessageBox msg;
    msg.setUnusual("", "");
}

TEST_F(ut_messageBox, setExit)
{
    MessageBox msg;
    msg.setExit();
}

TEST_F(ut_messageBox, setFolderDenied)
{
    MessageBox msg;
    msg.setFolderDenied();
}

TEST_F(ut_messageBox, onClearBtnClicked)
{
    MessageBox msg;
    msg.onClearBtnClicked(0);
    msg.onClearBtnClicked(1);
}

TEST_F(ut_messageBox, onDeleteBtnClicked)
{
    MessageBox msg;
    msg.onDeleteBtnClicked(0);
    msg.setDelete(true);
    msg.m_DeleteFlag = true;
    msg.onDeleteBtnClicked(1);
    msg.m_DeleteFlag = false;
    msg.onDeleteBtnClicked(1);
}

TEST_F(ut_messageBox, onExitBtnClicked)
{
    MessageBox msg;
    msg.onExitBtnClicked(0);
    msg.addCheckbox("11");
    msg.addRadioGroup("11", "22");
    msg.m_CheckBox->setCheckState(Qt::CheckState::Checked);
    msg.m_ButtonMin->setChecked(true);
    msg.onExitBtnClicked(1);
    msg.m_CheckBox->setCheckState(Qt::CheckState::Unchecked);
    msg.onExitBtnClicked(1);
    msg.m_CheckBox->setCheckState(Qt::CheckState::Checked);
    msg.m_ButtonMin->setChecked(false);
    msg.onExitBtnClicked(1);
}

TEST_F(ut_messageBox, setWarings)
{
    MessageBox msg;
    msg.setWarings("warring", "warring");
}

TEST_F(ut_messageBox, setWarings1)
{
    MessageBox msg;
    QStringList list;
    list << "11" << "22";
    msg.setWarings("warring", "warring", "warring",2,list);
}

TEST_F(ut_messageBox, setRedownload)
{
    MessageBox msg;
    msg.setRedownload("");
    msg.setRedownload("11",true,true);
}

TEST_F(ut_messageBox, setDelete)
{
    MessageBox msg;
    msg.setDelete(true);
    msg.setDelete(false);
}
TEST_F(ut_messageBox, onRenamelineeditChanged)
{
    MessageBox msg;
    msg.m_RenameSureButton = new QPushButton();
    msg.onRenamelineeditChanged("text.ddd");
    msg.onRenamelineeditChanged("text");
}

TEST_F(ut_messageBox, setClear)
{
    MessageBox msg;
    msg.setClear();
}

TEST_F(ut_messageBox, addCheckbox)
{
    MessageBox msg;
    msg.addCheckbox("11", true);
}

TEST_F(ut_messageBox, setNetWorkError)
{
    MessageBox msg;
    msg.setNetWorkError("11");
}

