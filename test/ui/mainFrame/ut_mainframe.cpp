#include <iostream>
#include "gtest/gtest.h"
#include <qtest.h>
#include <QDebug>
#include "mainframe.h"
#include "createtaskwidget.h"
#include <QTimer>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
class ut_MainFreme : public ::testing::Test
    , public QObject
{
protected:
    ut_MainFreme()
    {
    }

    virtual ~ut_MainFreme()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_MainFreme, addask)
{
    MainFrame *m = new MainFrame;
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("newTaskBtn");
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QWidgetList l = QApplication::topLevelWidgets();
        foreach (QWidget *w, l) {
            if (w->objectName() == "newTaskWidget") {
                DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
                if (sureBtn->isEnabled()) {
                    QTest::mouseClick(sureBtn, Qt::LeftButton);
                    timer->stop();
                    EXPECT_TRUE(true);
                }
            }
        }
    });
    timer->start(1000);

    QTimer::singleShot(100, this, [=]() {
        QWidgetList l = QApplication::topLevelWidgets();
        foreach (QWidget *w, l) {
            if (w->objectName() == "newTaskWidget") {
                DTextEdit *edit = w->findChild<DTextEdit *>("textUrl");
                edit->setText("http://download.qt.io/archive/qt/4.1/qt-mac-opensource-src-4.1.5.tar.gz");
            }
        }
    });
    QTest::mouseClick(btn, Qt::LeftButton);
}

TEST_F(ut_MainFreme, downloadTask)
{
    MainFrame *m = new MainFrame;
    QDBusInterface iface("com.downloader.service",
                         "/downloader/path",
                         "local.downloader.MainFrame",
                         QDBusConnection::sessionBus());
    iface.asyncCall("OpenBt", "/home/sanhei/Documents/123@.torrent");
}

//TEST_F(ut_MainFreme, unpauseTask)
//{
//    MainFrame *m = new MainFrame;
//    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
//    QWidget *box = bar->findChild<QWidget *>("toolBox");
//    DIconButton *btn = box->findChild<DIconButton *>("newTaskBtn");
//    DTextEdit *edit = m->getTaskWidget()->findChild<DTextEdit *>("textUrl");
//    DPushButton *sureBtn = m->getTaskWidget()->findChild<DPushButton *>("sureBtn");
//    edit->setText("http://download.qt.io/archive/qt/4.1/qt-mac-opensource-src-4.1.5.tar.gz");
//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, this, [=]() {
//        if (sureBtn->isEnabled()) {
//            QTest::mouseClick(sureBtn, Qt::LeftButton);
//            timer->stop();
//            EXPECT_TRUE(true);
//        }
//    });
//    timer->start(1000);
//    QTest::mouseClick(btn, Qt::LeftButton);
//}
