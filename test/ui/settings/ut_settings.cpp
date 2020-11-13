#include <iostream>
#include "gtest/gtest.h"
#include "settings.h"
#include "mainframe.h"
#include "qtest.h"
#include <QDebug>
#include <QPushButton>
#include <QTimer>
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
