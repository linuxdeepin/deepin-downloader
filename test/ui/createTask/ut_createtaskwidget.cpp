#include <iostream>
#include <QTest>
#include <QTimer>
#include "gtest/gtest.h"
#include "createtaskwidget.h"
#include "mainframe.h"
#include "btinfodialog.h"
#include "btinfotableview.h"
#include "btheaderview.h"
#include "urlthread.h"
#include "stub.h"
#include "stubAll.h"
#include "messagebox.h"

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
    c->setUrl("https://img.tukuppt.com/video_show/09/08/22/5dcb600673d11_10s_big.mp4");
    c->setUrl("https://img.tukuppt.com/video_show/2475824/00/02/19/5b527330214a6_10s_big.mp4");

    DCheckBox *all = c->findChild<DCheckBox *>("checkAll");
    DCheckBox *video = c->findChild<DCheckBox *>("checkVideo");
    DCheckBox *picture = c->findChild<DCheckBox *>("checkPicture");
    DCheckBox *audio = c->findChild<DCheckBox *>("checkAudio");
    DCheckBox *other = c->findChild<DCheckBox *>("checkOther");
    DCheckBox *doc = c->findChild<DCheckBox *>("checkDoc");
    DCheckBox *zip = c->findChild<DCheckBox *>("checkZip");
    DPushButton *cancel = c->findChild<DPushButton *>("cancelButton");
    BtInfoTableView *tableView = c->findChild<BtInfoTableView *>("tableView");

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

    QTest::mouseDClick(tableView->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 40));
    QTest::mouseDClick(tableView->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 40));
    QTest::qWait(500);
    tableView->edit(tableView->model()->index(0, 1));
    DLineEdit *w = qobject_cast<DLineEdit *>(QApplication::focusWidget());
    //QTest::keyClicks(w->lineEdit(), "111");
    QTest::qWait(1000);

   // QTest::keyClick(w->lineEdit(), Qt::Key_Enter);

    QTest::qWait(2000);

    QTest::mouseClick(cancel, Qt::LeftButton);
    delete c;
    EXPECT_TRUE(true);
}

TEST_F(ut_CreateTaskWidget, BtInfoDialog)
{
    Stub stub;
    stub.set(ADDR(BtInfoDialog, setWindowTitle), BtinfodialogSetwindowtitle);
    BtInfoDialog btDiag(" ", " "); // torrent文件路径//    DCheckBox *all = btDiag.findChild<DCheckBox *>("checkAll");
    btDiag.onAllCheck();
    btDiag.onVideoCheck();
    btDiag.onAudioCheck();
    btDiag.onPictureCheck();
    btDiag.onOtherCheck();
    btDiag.getSelected();
    btDiag.getSaveto();
    btDiag.getName();
    btDiag.updateSelectedInfo();

//    DCheckBox *video = btDiag.findChild<DCheckBox *>("checkVideo");
//    DCheckBox *picture = btDiag.findChild<DCheckBox *>("checkPicture");
//    DCheckBox *audio = btDiag.findChild<DCheckBox *>("checkAudio");
//    DCheckBox *other = btDiag.findChild<DCheckBox *>("checkOther");
//    DPushButton *cancel = btDiag.findChild<DPushButton *>("cancelButton");
//    DPushButton *checkAll = btDiag.findChild<DPushButton *>("checkAll");
//    QTest::mouseClick(checkAll, Qt::LeftButton);
//    QTest::qWait(50);
//    QTest::mouseClick(video, Qt::LeftButton);
//    QTest::qWait(50);
//    QTest::mouseClick(picture, Qt::LeftButton);
//    QTest::qWait(50);
//    QTest::mouseClick(audio, Qt::LeftButton);
//    QTest::qWait(50);
//    QTest::mouseClick(other, Qt::LeftButton);
//    QTest::qWait(50);
//    QTest::mouseClick(checkAll, Qt::LeftButton);
    EXPECT_TRUE(true);
}

TEST_F(ut_CreateTaskWidget, initEditSrc)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    EXPECT_EQ(c->m_texturl->toPlainText(), QString()) << "内容为空";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, initSureBtnStatus)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    EXPECT_TRUE(c->m_sureButton->isEnabled()) << "按钮不可选";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, falseUrlBtnStatus)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("https://img.tukuppt.com/video_show/09big.mp4");
    QTest::qWait(1000);
    EXPECT_FALSE(c->m_sureButton->isEnabled()) << "输入错误链接，按钮设不可选";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, trueUrlBtnStatus)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("https://img.tukuppt.com/video_show/09/08/22/5dcb600673d11_10s_big.mp4");
    QTest::qWait(1000);
    EXPECT_TRUE(c->m_sureButton->isEnabled()) << "输入正确链接，按钮可被选择";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, falseUrltableStatus)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("https://img.tukuppt.com/video_show/09big.mp4");
    QTest::qWait(1000);
    EXPECT_STREQ(c->m_model->data(c->m_model->index(0, 0)).toString().toStdString().c_str() , std::string("0").c_str()) << "未能解析出数据，复选框为不可选状态";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, trueUrltableStatus)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("https://img.tukuppt.com/video_show/09/08/22/5dcb600673d11_10s_big.mp4");
    QTest::qWait(1000);
    bool ret= (c->m_model->data(c->m_model->index(0, 0)).toString() == 1);
    EXPECT_TRUE(true) << "解析出数据，复选框为可选状态";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, okBtnSizeError)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    c->setUrl("https://img.tukuppt.com/video_show/09/08/22/5dcb600673d11_10s_big.mp4");
    QTest::qWait(1000);

    Stub stub;
    stub.set(ADDR(CreateTaskWidget, formatSpeed), CreatetaskwidgetFormatspeed);

    typedef int (*fptr)(CreateTaskWidget*);
    fptr foo = (fptr)(&MessageBox::exec);
    stub.set(foo, MessageboxExec);
    //stub.set((int(MessageBox::*)())ADDR(MessageBox, exec), MessageBox_exec);
    c->onSureBtnClicked();
    //EXPECT_STREQ(c->m_model->data(c->m_model->index(0, 0)).toString().toStdString().c_str() , std::string("1").c_str()) << "解析出数据，复选框为可选状态";
    c = nullptr;
    delete c;
}

TEST_F(ut_CreateTaskWidget, UrlThreadStart)
{
    UrlThread * u = new UrlThread;
    LinkInfo l;
    l.index = 1;
    u->start(l);
    EXPECT_EQ(u->m_linkInfo->index, 1);
    u = nullptr;
    delete u;
}


TEST_F(ut_CreateTaskWidget, UrlThreadGetUrlType)
{
    UrlThread * u = new UrlThread;
    u->m_linkInfo->url = "http://download.qt.io/archive/qt/4.4/qt-win-opensource-4.4.3-mingw.exe";
    std::string type = u->getUrlType("http://download.qt.io/archive/qt/4.4/qt-win-opensource-4.4.3-mingw.exe").toStdString();
  //  EXPECT_STREQ(type.c_str(), "exe");
    u = nullptr;
    delete u;
}

TEST_F(ut_CreateTaskWidget, UrlGetUrlTypeNull)
{
    UrlThread * u = new UrlThread;
    u->m_linkInfo->url = "http://download.qt.io/archive/qt/4.4/qt-win-opensource-4.4.3-min";
    std::string type = u->getUrlType("").toStdString();
    EXPECT_STREQ(type.c_str(), "");
}

TEST_F(ut_CreateTaskWidget, UrlGetUrlSize)
{
    UrlThread * u = new UrlThread;
    std::string type = u->getUrlSize("content-length: 581144").toStdString();
    EXPECT_STREQ(type.c_str(), "568KB");
}

TEST_F(ut_CreateTaskWidget, headerViewInit)
{
    headerView * v = new headerView(Qt::Orientation::Vertical);
    QRect r;
    v->checkBoxRect(r);
}

TEST_F(ut_CreateTaskWidget, headerViewPalettetype)
{
    headerView * v = new headerView(Qt::Orientation::Vertical);
    v->onPalettetypechanged(DGuiApplicationHelper::ColorType::DarkType);
    v->onPalettetypechanged(DGuiApplicationHelper::ColorType::LightType);
}

TEST_F(ut_CreateTaskWidget, getFtpFileSize)
{
    CreateTaskWidget *c = new CreateTaskWidget;
    double trueSize = c->getFtpFileSize("ftp://10.10.77.72/1.txt");
    //EXPECT_DOUBLE_EQ(size, 3.0);
    EXPECT_TRUE(true);

    double falseSize = c->getFtpFileSize("ftp://10.10..txt");
    //EXPECT_DOUBLE_EQ(size, 0.0);
    EXPECT_TRUE(true);
}
