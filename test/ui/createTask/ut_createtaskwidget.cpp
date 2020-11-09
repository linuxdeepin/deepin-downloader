#include <iostream>
#include "gtest/gtest.h"
#include "createtaskwidget.h"
#include "mainframe.h"
#include "btinfodialog.h"

class ut_CreateTaskWidget : public ::testing::Test
    , public QObject
{
protected:
    ut_CreateTaskWidget()
    {
    }

    virtual ~ut_CreateTaskWidget()
    {
    }
    virtual void SetUp()
    {
        //  m_createTaskWidget = new CreateTaskWidget;
    }

    virtual void TearDown()
    {
        //   delete m_createTaskWidget;
    }
    // CreateTaskWidget *m_createTaskWidget;
};

TEST_F(ut_CreateTaskWidget, init)
{
    MainFrame *m = new MainFrame;
    m->onDownloadNewUrl("", "", "", "");
    delete m;
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("11");
    //c->showNetErrorMsg();
    c->updateSelectedInfo();
    c->setUrlName(0, "11");
    EXPECT_TRUE(true);
}

TEST_F(ut_CreateTaskWidget, BtInfoDialog)
{
    QString url;
    BtInfoDialog btDiag(" ", " "); // torrent文件路径
    QMap<QString, QVariant> opt;
    QString infoName;
    QString infoHash;
    btDiag.onBtnOK();
    btDiag.getBtInfo(opt, infoName, infoHash);

    EXPECT_TRUE(true);
}
