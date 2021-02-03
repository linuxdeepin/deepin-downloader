#include <iostream>
#include "gtest/gtest.h"
#include "aria2rpcinterface.h"
#include <qtest.h>

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
    Aria2RPCInterface::instance()->init();
    EXPECT_TRUE(true);
}

TEST_F(ut_aria2Test, addUri)
{
    QString uri = "http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz";
    QMap<QString, QVariant> opt;
    opt.insert("test", "test");
    QString id = "test";
    bool ret = Aria2RPCInterface::instance()->addUri(uri, opt, id);
    EXPECT_TRUE(ret);
}

TEST_F(ut_aria2Test, addUriEmpty)
{
    QString uri;
    QMap<QString, QVariant> opt;
    QString id;
    bool ret = Aria2RPCInterface::instance()->addUri(uri, opt, id);
    EXPECT_FALSE(ret);
}

TEST_F(ut_aria2Test, addTorrent)
{
    QString uri = "/home/sanhei/Documents/123@.torrent";
    QMap<QString, QVariant> opt;
    opt.insert("test", "test");
    QString id = "test";
    bool ret = Aria2RPCInterface::instance()->addTorrent(uri, opt, id);
    EXPECT_TRUE(ret);
}

TEST_F(ut_aria2Test, addTorrentEmpty)
{
    QString uri;
    QMap<QString, QVariant> opt;
    QString id;
    bool ret = Aria2RPCInterface::instance()->addTorrent(uri, opt, id);
    EXPECT_FALSE(ret);
}

TEST_F(ut_aria2Test, addMetalink)
{
    QString uri = "test";
    QMap<QString, QVariant> opt;
    opt.insert("test", "test");
    QString id = "test";
    bool ret = Aria2RPCInterface::instance()->addMetalink(uri, opt, id);
    EXPECT_TRUE(ret);
}

TEST_F(ut_aria2Test, addMetalinkEmpty)
{
    QString uri;
    QMap<QString, QVariant> opt;
    QString id;
    bool ret = Aria2RPCInterface::instance()->addMetalink(uri, opt, id);
    EXPECT_FALSE(ret);
}

TEST_F(ut_aria2Test, getBtInfo)
{
    QString torrentPath;
    Aria2cBtInfo btInfo = Aria2RPCInterface::instance()->getBtInfo(torrentPath);
    EXPECT_TRUE(btInfo.name.isEmpty());
}

TEST_F(ut_aria2Test, tellStatus)
{
    QString gid;
    QStringList keys;
    QString id;
    bool b = Aria2RPCInterface::instance()->tellStatus(gid, keys, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, pause)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->pause(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, forcePause)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->forcePause(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, pauseAll)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->pauseAll(gid);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, forcePauseAll)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->forcePauseAll(gid);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, unpause)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->unpause(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, unpauseAll)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->unpauseAll(gid);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, remove)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->remove(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, forceRemove)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->forceRemove(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, removeDownloadResult)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->removeDownloadResult(gid);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, getFiles)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->getFiles(gid, id);
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, getGlobalSatat)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->getGlobalSatat();
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, getGlobalOption)
{
    QString gid;
    QString id;
    bool b = Aria2RPCInterface::instance()->getGlobalOption();
    EXPECT_TRUE(b);
}

TEST_F(ut_aria2Test, modifyConfigFile)
{
    QString configItem;
    QString value;
    Aria2RPCInterface::instance()->modifyConfigFile(configItem, value);
    EXPECT_TRUE(true);
}

TEST_F(ut_aria2Test, getCapacityFreeByte)
{
    QString path;
    long size = Aria2RPCInterface::instance()->getCapacityFreeByte(path);
    EXPECT_TRUE(true);
    QTest::qSleep(300);
}
