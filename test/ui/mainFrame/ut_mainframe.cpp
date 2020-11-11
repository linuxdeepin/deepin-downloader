#include "gtest/gtest.h"

#include <iostream>
#include <qtest.h>
#include <QDebug>
#include <QTimer>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QPoint>

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

//小任务，可以快速下载完，方便测试已完成列表。
TEST_F(ut_MainFreme, addHttpFastTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("newTaskBtn");
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
            if (sureBtn->isEnabled()) {
                QTest::mouseClick(sureBtn, Qt::LeftButton);
                timer->stop();
            }
        }
    });
    timer->start(1000);

    QTimer::singleShot(1000, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DTextEdit *edit = w->findChild<DTextEdit *>("textUrl");
            edit->setText("https://img.tukuppt.com/video_show/7165162/00/19/39/5f06cfe424c38_10s_big.mp4");
        }
    });
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, addHttpTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("newTaskBtn");
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
            if (sureBtn->isEnabled()) {
                QTest::mouseClick(sureBtn, Qt::LeftButton);
                timer->stop();
            }
        }
    });
    timer->start(1000);

    QTimer::singleShot(1000, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DTextEdit *edit = w->findChild<DTextEdit *>("textUrl");
            edit->setText("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz");
        }
    });
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, addRepeatHttpTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("newTaskBtn");
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
            if (sureBtn->isEnabled()) {
                QTimer::singleShot(1000, this, [=]() {
                    QWidget *ww = QApplication::activeWindow();
                    DPushButton *redownloadBtn = ww->findChild<DPushButton *>("redownload");
                    QTest::mouseClick(redownloadBtn, Qt::LeftButton);
                });

                QTest::mouseClick(sureBtn, Qt::LeftButton);
                timer->stop();
            }
        }
    });
    timer->start(1000);

    QTimer::singleShot(100, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "newTaskWidget") {
            DTextEdit *edit = w->findChild<DTextEdit *>("textUrl");
            edit->setText("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz");
        }
    });
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, addBtTask)
{
    QTimer::singleShot(500, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "btInfoDialog") {
            DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
            QTest::mouseClick(sureBtn, Qt::LeftButton);
            EXPECT_TRUE(true);
        }
    });
    QDBusInterface iface("com.downloader.service",
                         "/downloader/path",
                         "local.downloader.MainFrame",
                         QDBusConnection::sessionBus());
    iface.asyncCall("OpenBt", "/home/sanhei/Documents/123@.torrent");
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, addRepeatBtTask)
{
    QTimer::singleShot(500, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        if (w->objectName() == "btInfoDialog") {
            DPushButton *sureBtn = w->findChild<DPushButton *>("sureBtn");
            QTimer::singleShot(1000, this, [=]() {
                QWidget *ww = QApplication::activeWindow();
                DPushButton *redownloadBtn = ww->findChild<DPushButton *>("redownload");
                QTest::mouseClick(redownloadBtn, Qt::LeftButton);
            });
            QTest::mouseClick(sureBtn, Qt::LeftButton);
            EXPECT_TRUE(true);
        }
    });
    QDBusInterface iface("com.downloader.service",
                         "/downloader/path",
                         "local.downloader.MainFrame",
                         QDBusConnection::sessionBus());
    iface.asyncCall("OpenBt", "/home/sanhei/Documents/123@.torrent");
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, pauseTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("pauseDownloadBtn");
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    QTest::qWait(500);
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
    QRect rect = table->visualRect(model->index(0, 2));
    QTest::mouseClick(table->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
    QTest::qWait(500);
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, onContextMenu)
{
    MainFrame *m = MainFrame::instance();
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QTest::qWait(200);
    QRect rect = table->visualRect(model->index(1, 2));

    QTimer::singleShot(500, this, [=]() {
        QWidgetList w = QApplication::topLevelWidgets();
        for (int i = 0; i < w.count(); i++) {
            if (w.at(i)->objectName() == "tableMenu") {
                QPoint point = w.at(i)->rect().center();
                QTest::mouseClick(w.at(i), Qt::LeftButton, Qt::KeyboardModifiers(),
                                  QPoint(point.x(), point.y() + 30));
            }
        }
    });

    QTest::mouseClick(table->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
    QTest::mouseClick(table->viewport(), Qt::RightButton, Qt::KeyboardModifiers(), rect.center());
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, changeList)
{
    MainFrame *m = MainFrame::instance();
    DListView *list = m->findChild<DListView *>("leftList");
    TableModel *model = static_cast<TableModel *>(list->model());
    QTest::qWait(200);
    QRect rect = list->visualRect(model->index(1, 0));

    QTest::mouseClick(list->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, rename)
{
    MainFrame *m = MainFrame::instance();
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QTest::qWait(200);
    QRect rect = table->visualRect(model->index(0, 2));

    QTimer::singleShot(500, this, [=]() {
        QWidgetList w = QApplication::topLevelWidgets();
        //qDebug() << "aaaaaaaaaaaaaaaaa::  ";
        for (int i = 0; i < w.count(); i++) {
            // qDebug() << "w: " << w.at(i)->objectName();
            if (w.at(i)->objectName() == "tableMenu") {
                //QAction *a = w.at(i)->findChild<QAction *>("rename");
                //qDebug() << "aaaaaaaaaaaaaaaaa::  " << a->objectName();
                QPoint point = w.at(i)->rect().center();
                QTest::mouseClick(w.at(i), Qt::LeftButton, Qt::KeyboardModifiers(),
                                  QPoint(point.x(), point.y() - 30));
                QTest::qWait(500);
                DLineEdit *w = qobject_cast<DLineEdit *>(QApplication::focusWidget());
                QTest::keyClick(w->lineEdit(), Qt::Key_Backspace);
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), Qt::Key_Backspace);
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), Qt::Key_Backspace);
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), Qt::Key_Backspace);
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), Qt::Key_Backspace);
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 't');
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 'e');
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 's');
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 't');
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 'O');
                QTest::qWait(100);
                QTest::keyClick(w->lineEdit(), 'K');
                QTest::qWait(500);
            }
        }
    });

    QTest::mouseClick(table->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
    QTest::mouseClick(table->viewport(), Qt::RightButton, Qt::KeyboardModifiers(), rect.center());
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, deleteTask)
{
    MainFrame *m = MainFrame::instance();
    DTitlebar *bar = m->findChild<DTitlebar *>("titlebar");
    QWidget *box = bar->findChild<QWidget *>("toolBox");
    DIconButton *btn = box->findChild<DIconButton *>("deleteBtn");
    TableView *table = m->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QRect rect = table->visualRect(model->index(0, 2));
    QTest::mouseClick(table->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
    QTest::qWait(500);

    QTimer::singleShot(500, this, [=]() {
        QWidget *w = QApplication::activeWindow();
        DPushButton *sureBtn = w->findChild<DPushButton *>("delete");
        QTest::mouseClick(sureBtn, Qt::LeftButton);
        EXPECT_TRUE(true);
    });

    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::qWait(2000);
}
