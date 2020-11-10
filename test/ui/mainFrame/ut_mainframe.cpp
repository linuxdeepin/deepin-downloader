#include "gtest/gtest.h"

#include <iostream>
#include <qtest.h>
#include <QDebug>
#include <QTimer>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>

#include "mainframe.h"
#include "createtaskwidget.h"
#include "tableView.h"
#include "tableModel.h"
#include "global.h"

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
    MainFrame *m = MainFrame::instance();
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
                edit->setText("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz");
            }
        }
    });
    QTest::mouseClick(btn, Qt::LeftButton);
}

TEST_F(ut_MainFreme, addBtTask)
{
    //    QTimer::singleShot(500, this, [=]() {
    //        QWidgetList l = QApplication::topLevelWidgets();
    //        foreach (QWidget *w, l) {
    //            if (w->objectName() == "btInfoDialog") {
    //                DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
    //                QTest::mouseClick(sureBtn, Qt::LeftButton);
    //                EXPECT_TRUE(true);
    //            }
    //        }
    //    });
    //    QDBusInterface iface("com.downloader.service",
    //                         "/downloader/path",
    //                         "local.downloader.MainFrame",
    //                         QDBusConnection::sessionBus());
    //    iface.asyncCall("OpenBt", "/home/sanhei/Documents/123@.torrent");
}

TEST_F(ut_MainFreme, pauseTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("pauseDownloadBtn");
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->renderList().at(0)->Ischecked = true;
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, unpauseTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("startDownloadBtn");
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->renderList().at(0)->Ischecked = true;
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(5000);
}

TEST_F(ut_MainFreme, onContextMenu)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("startDownloadBtn");
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->renderList().at(0)->Ischecked = true;
    QTest::mouseClick(table, Qt::RightButton, Qt::MetaModifier, QPoint(70, 70));
    QTest::qWait(5000);
}
