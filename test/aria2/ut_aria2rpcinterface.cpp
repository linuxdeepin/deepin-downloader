#include <iostream>
#include "gtest/gtest.h"
#include "aria2rpcinterface.h"

class ut_aria2Test : public ::testing::Test
{
protected:
    ut_aria2Test()
    {
    }

    virtual ~ut_aria2Test()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_aria2Test, init)
{
    bool ret = Aria2RPCInterface::instance()->init();
    EXPECT_TRUE(ret);
}

//TEST_F(ut_aria2Test, addUri)
//{
//    QString uri;
//    QMap<QString, QVariant> opt;
//    QString id;
//    bool ret = Aria2RPCInterface::instance()->addUri(uri, opt, id);
//    EXPECT_TRUE(ret);
//}

TEST_F(ut_aria2Test, addTorrent)
{
    QString uri;
    QMap<QString, QVariant> opt;
    QString id;
    bool ret = Aria2RPCInterface::instance()->addTorrent(uri, opt, id);
    EXPECT_TRUE(ret);
}

TEST_F(ut_aria2Test, addMetalink)
{
    QString uri;
    QMap<QString, QVariant> opt;
    QString id;
    bool ret = Aria2RPCInterface::instance()->addMetalink(uri, opt, id);
    EXPECT_TRUE(ret);
}
