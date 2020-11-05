#include <iostream>
#include "gtest/gtest.h"
#include "createtaskwidget.h"
#include "mainframe.h"

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
    //    c->setUrl("11");
    //    qDebug()<<"222";
    EXPECT_TRUE(true);
    //    qDebug()<<"33";
}
