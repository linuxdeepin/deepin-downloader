#include <iostream>
#include <QTest>
#include <QTimer>
#include "gtest/gtest.h"
#include "createtaskwidget.h"
#include "mainframe.h"
#include "btinfodialog.h"
#include "btinfotableview.h"

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
    CreateTaskWidget *c = new CreateTaskWidget;
    c->show();
    c->setUrl("https://img.tukuppt.com/video_show/09/08/22/5dcb600673d11_10s_big.mp4");
    c->setUrl("https://img.tukuppt.com/video_show/2475824/00/02/19/5b527330214a6_10s_big.mp4");

    DCheckBox *all = c->findChild<DCheckBox *>("checkAll");
    DCheckBox * video = c->findChild<DCheckBox *>("checkVideo");
    DCheckBox * picture = c->findChild<DCheckBox *>("checkPicture");
    DCheckBox * audio = c->findChild<DCheckBox *>("checkAudio");
    DCheckBox * other = c->findChild<DCheckBox *>("checkOther");
    DCheckBox * doc = c->findChild<DCheckBox *>("checkDoc");
    DCheckBox * zip = c->findChild<DCheckBox *>("checkZip");
    DPushButton * cancel = c->findChild<DPushButton *>("cancelButton");
    BtInfoTableView * tableView = c->findChild<BtInfoTableView *>("tableView");


    QTest::mouseClick(all, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(video, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(picture, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(audio, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(other, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(doc, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(zip, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(all, Qt::LeftButton);

    QTest::qWait(1000);

    QTest::mouseDClick(tableView->viewport(),Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100,40));
    QTest::mouseDClick(tableView->viewport(),Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100,40));
    QTest::qWait(500);
    tableView->edit(tableView->model()->index(0,1));
    DLineEdit * w = qobject_cast<DLineEdit *>(QApplication::focusWidget());
    QTest::keyClicks(w->lineEdit(),"111");
    QTest::qWait(1000);

    QTest::keyClick(w->lineEdit(), Qt::Key_Enter);

    QTest::qWait(2000);

    QTest::mouseClick(cancel, Qt::LeftButton);
    delete c;
    EXPECT_TRUE(true);
}

TEST_F(ut_CreateTaskWidget, BtInfoDialog)
{
    BtInfoDialog btDiag(" ", " "); // torrent文件路径
    DCheckBox *all = btDiag.findChild<DCheckBox *>("checkAll");
    DCheckBox * video = btDiag.findChild<DCheckBox *>("checkVideo");
    DCheckBox * picture = btDiag.findChild<DCheckBox *>("checkPicture");
    DCheckBox * audio = btDiag.findChild<DCheckBox *>("checkAudio");
    DCheckBox * other = btDiag.findChild<DCheckBox *>("checkOther");
    DPushButton * cancel = btDiag.findChild<DPushButton *>("cancelButton");
    QTest::mouseClick(all, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(video, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(picture, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(audio, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(other, Qt::LeftButton);
    QTest::qWait(50);
    QTest::mouseClick(all, Qt::LeftButton);
    EXPECT_TRUE(true);
}
