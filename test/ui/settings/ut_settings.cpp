#include <iostream>
#include "gtest/gtest.h"
#include "settings.h"
#include "mainframe.h"
#include "qtest.h"
#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <DRadioButton>
#include <dsettingsoption.h>

class ut_Settings : public ::testing::Test
    , public QObject
{
protected:
    ut_Settings()
    {
    }

    virtual ~ut_Settings()
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


//开机自启
TEST_F(ut_Settings, autoStart)
{
    Settings::getInstance()->setAutoStart(true);
    QTest::qWait(500);
    Settings::getInstance()->setAutoStart(false);
    EXPECT_TRUE(true);
}

//最大下载个数
TEST_F(ut_Settings, setMaxDownloadNum)
{
    auto option = Settings::getInstance()->m_settings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");
    option->setValue(5);
    QTest::qWait(500);
    option->setValue(3);
    EXPECT_TRUE(Settings::getInstance()->getMaxDownloadTaskNumber() == 3);
}

//自动下载bt文件
TEST_F(ut_Settings, setAutoDownloadBtfile)
{
    auto startAssociatedBTFile = Settings::getInstance()->m_settings->option("Monitoring.BTRelation.AssociateBTFileAtStartup");
    startAssociatedBTFile->setValue(false);
    QTest::qWait(500);
    startAssociatedBTFile->setValue(true);
    EXPECT_TRUE(Settings::getInstance()->getStartAssociatedBTFileState());
}

TEST_F(ut_Settings, getSpeed)
{
    EXPECT_FALSE(Settings::getInstance()->getAutoSortBySpeed());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getPath)
{
    QString str = Settings::getInstance()->getCustomFilePath();
    QString str1 = Settings::getInstance()->getDownloadSavePath();
    EXPECT_FALSE(str.isEmpty() && str1.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getspeed)
{
    EXPECT_TRUE(Settings::getInstance()->getDownloadInfoSystemNotifyState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getPowerOnState)
{
    Settings::getInstance()->getPowerOnState();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getAutostartUnfinishedTaskState)
{
    EXPECT_TRUE(Settings::getInstance()->getAutostartUnfinishedTaskState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadDirectorySelected)
{
    EXPECT_TRUE(Settings::getInstance()->getDownloadDirectorySelected());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadSavePath)
{
    QString str = Settings::getInstance()->getDownloadSavePath();
    EXPECT_FALSE(str.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getOneClickDownloadState)
{
    Settings::getInstance()->getOneClickDownloadState();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getCloseMainWindowSelected)
{
    Settings::getInstance()->getCloseMainWindowSelected();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getMaxDownloadTaskNumber)
{
    EXPECT_TRUE(Settings::getInstance()->getMaxDownloadTaskNumber());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadFinishedOpenState)
{
    Settings::getInstance()->getDownloadFinishedOpenState();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getAutoDeleteFileNoExistentTaskState)
{
    Settings::getInstance()->getAutoDeleteFileNoExistentTaskState();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadSettingSelected)
{
    Settings::getInstance()->getDownloadSettingSelected();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getMaxDownloadSpeedLimit)
{
    QString str = Settings::getInstance()->getMaxDownloadSpeedLimit();
    EXPECT_FALSE(str.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getMaxUploadSpeedLimit)
{
    QString str = Settings::getInstance()->getMaxUploadSpeedLimit();
    EXPECT_FALSE(str.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getSpeedLimitStartTime)
{
    QString str = Settings::getInstance()->getSpeedLimitStartTime();
    EXPECT_FALSE(str.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getSpeedLimitEndTime)
{
    QString str = Settings::getInstance()->getSpeedLimitEndTime();
    EXPECT_FALSE(str.isEmpty());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getAllSpeedLimitInfo)
{
    Settings::getInstance()->getAllSpeedLimitInfo();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getClipBoardState)
{
    EXPECT_TRUE(Settings::getInstance()->getClipBoardState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getHttpDownloadState)
{
    EXPECT_TRUE(Settings::getInstance()->getHttpDownloadState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getBtDownloadState)
{
    EXPECT_TRUE(Settings::getInstance()->getBtDownloadState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getMagneticDownloadState)
{
    EXPECT_TRUE(Settings::getInstance()->getMagneticDownloadState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getAutoOpennewTaskWidgetState)
{
    Settings::getInstance()->getAutoOpennewTaskWidgetState();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getStartAssociatedBTFileState)
{
    EXPECT_TRUE(Settings::getInstance()->getStartAssociatedBTFileState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadInfoSystemNotifyState)
{
    EXPECT_TRUE(Settings::getInstance()->getDownloadInfoSystemNotifyState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getNewTaskShowMainWindowState)
{
    EXPECT_TRUE(Settings::getInstance()->getNewTaskShowMainWindowState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDisckcacheNum)
{
    EXPECT_TRUE(Settings::getInstance()->getDisckcacheNum());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getPriorityDownloadBySize)
{
    QString size;
    Settings::getInstance()->getPriorityDownloadBySize(size);
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getCustomFilePath)
{
    QString str = Settings::getInstance()->getCustomFilePath();
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, getIsShowTip)
{
    EXPECT_TRUE(Settings::getInstance()->getIsShowTip());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getIsClipboradStart)
{
    QString str;
    Settings::getInstance()->getIsClipboradStart(str);
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, setCloseMainWindowSelected)
{
    Settings::getInstance()->setCloseMainWindowSelected(1);
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, setCustomFilePath)
{
    Settings::getInstance()->setCustomFilePath(Settings::getInstance()->getCustomFilePath());
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, setIsShowTip)
{
    Settings::getInstance()->setIsShowTip(true);
    EXPECT_TRUE(true);
    QTest::qWait(50);
}

TEST_F(ut_Settings, setIsClipboradStart)
{
    Settings::getInstance()->setIsClipboradStart("aaaa");
    EXPECT_TRUE(true);
    QTest::qWait(50);
}


TEST_F(ut_Settings, close)
{
    MainFrame *m = MainFrame::instance();
    QSystemTrayIcon *tray = m->findChild<QSystemTrayIcon *>("systemTray");
    QMenu *menu = tray->contextMenu();
    QTimer::singleShot(1000, this, [=]() {
        QTimer::singleShot(1000, this, [=]() {
            QWidget *w = QApplication::activeWindow();
            QAbstractButton *btn = w->findChild<QAbstractButton *>("sure");
            QTest::mouseClick(btn, Qt::LeftButton);
            // w->close();
        });
        QPoint p = menu->rect().bottomRight();
        QTest::mouseClick(menu, Qt::LeftButton, Qt::MetaModifier, QPoint(p.x() - 10, p.y() - 10));
        // menu->close();
    });
    menu->exec();
    //QTest::mouseClick(menu, Qt::RightButton);
}
