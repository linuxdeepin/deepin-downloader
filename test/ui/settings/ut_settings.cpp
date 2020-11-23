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

TEST_F(ut_Settings, initSetting)
{
    MainFrame *m = MainFrame::instance();
    m->show();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    DMenu *menu = bar->menu();

    QTimer::singleShot(1000, this, [=]() {
        QTimer::singleShot(1000, this, [=]() {
            QWidget *w = QApplication::activeWindow();
            w->close();
        });
        QTest::mouseClick(menu, Qt::LeftButton, Qt::MetaModifier, QPoint(10, 10));
        menu->close();
    });
    menu->exec();
    EXPECT_TRUE(true);
}

//开机自启
TEST_F(ut_Settings, autoStart)
{
    Settings::getInstance()->setAutoStart(true);
    QTest::qWait(500);
    Settings::getInstance()->setAutoStart(false);
    EXPECT_TRUE(Settings::getInstance()->getPowerOnState());
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
    EXPECT_TRUE(Settings::getInstance()->getPowerOnState());
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
    EXPECT_TRUE(Settings::getInstance()->getOneClickDownloadState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getCloseMainWindowSelected)
{
    EXPECT_TRUE(Settings::getInstance()->getCloseMainWindowSelected());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getMaxDownloadTaskNumber)
{
    EXPECT_TRUE(Settings::getInstance()->getMaxDownloadTaskNumber());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadFinishedOpenState)
{
    EXPECT_TRUE(Settings::getInstance()->getDownloadFinishedOpenState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getAutoDeleteFileNoExistentTaskState)
{
    EXPECT_TRUE(Settings::getInstance()->getAutoDeleteFileNoExistentTaskState());
    QTest::qWait(50);
}

TEST_F(ut_Settings, getDownloadSettingSelected)
{
    EXPECT_TRUE(Settings::getInstance()->getDownloadSettingSelected());
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
    EXPECT_TRUE(Settings::getInstance()->getAutoOpennewTaskWidgetState());
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
    EXPECT_TRUE(Settings::getInstance()->getPriorityDownloadBySize(size));
    QTest::qWait(50);
}

TEST_F(ut_Settings, getCustomFilePath)
{
    QString str = Settings::getInstance()->getCustomFilePath();
    EXPECT_FALSE(str.isEmpty());
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
    EXPECT_TRUE(Settings::getInstance()->getIsClipboradStart(str));
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

TEST_F(ut_Settings, diagnostictool)
{
    MainFrame *m = MainFrame::instance();
    m->show();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    DMenu *menu = bar->menu();
    QTimer::singleShot(1000, this, [=]() {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() {
            QWidget *w = QApplication::activeWindow();
            QPushButton *btn = w->findChild<QPushButton *>("diagnoseBtn");
            if (btn->isEnabled()) {
                w->close();
                timer->stop();
                EXPECT_TRUE(true);
            }
        });
        timer->start(2000);
        QTest::mouseClick(menu, Qt::LeftButton, Qt::MetaModifier, QPoint(70, 70));
        menu->close();
    });
    menu->exec();
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
