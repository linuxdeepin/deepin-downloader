#include <iostream>
#include "gtest/gtest.h"
#include "settings.h"
#include "mainframe.h"
#include "qtest.h"
#include <QDebug>
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
    MainFrame *m = new MainFrame;
    m->show();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    DMenu *menu = bar->menu();
    for (int i = 0; i < menu->children().size(); i++) {
        qDebug() << menu->children().at(i)->objectName();
    }
    QTimer::singleShot(200, this, [=]() {
        QTimer::singleShot(200, this, [=]() {
            QWidget *w = QApplication::activeWindow();
            w->close();
            EXPECT_TRUE(true);
        });
        QTest::mouseClick(menu, Qt::LeftButton, Qt::MetaModifier, QPoint(10, 10));
    });
    menu->exec();
}

TEST_F(ut_Settings, diagnostictool)
{
    MainFrame *m = new MainFrame;
    m->show();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    DMenu *menu = bar->menu();
    for (int i = 0; i < menu->children().size(); i++) {
        qDebug() << menu->children().at(i)->objectName();
    }
    QTimer::singleShot(200, this, [=]() {
        QTimer::singleShot(200, this, [=]() {
            QWidget *w = QApplication::activeWindow();
            w->close();
            EXPECT_TRUE(true);
        });
        QTest::mouseClick(menu, Qt::LeftButton, Qt::MetaModifier, QPoint(10, 70));
    });
    menu->exec();
    EXPECT_TRUE(true);
}
