
#include <iostream>
#include "gtest/gtest.h"
#include "func.h"
#include "messagebox.h"
#include <QObject>
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
}

TEST_F(ut_messageBox, onDeleteBtnClicked)
{
    MessageBox msg;
    msg.onDeleteBtnClicked(0);
}

TEST_F(ut_messageBox, onExitBtnClicked)
{
    MessageBox msg;
    msg.onExitBtnClicked(0);
}

TEST_F(ut_messageBox, setWarings)
{
    MessageBox msg;
    msg.setWarings("warring", "warring");
}

TEST_F(ut_messageBox, setRedownload)
{
    MessageBox msg;
    msg.setRedownload("");
}

TEST_F(ut_messageBox, setDelete)
{
    MessageBox msg;
    msg.setDelete(true);
}
//TEST_F(ut_messageBox, onRenamelineeditChanged)
//{
//    MessageBox msg;
//    msg.onRenamelineeditChanged("text");
//}

TEST_F(ut_messageBox, setClear)
{
    MessageBox msg;
    msg.setClear();
}
