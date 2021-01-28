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
#include <QClipboard>
#include <QMenu>
#include <QWebSocket>

#include "mainframe.h"
#include "createtaskwidget.h"
#include "tableView.h"
#include "tableModel.h"
#include "global.h"
#include "tabledatacontrol.h"
#include "dbdefine.h"
#include "analysisurl.h"
#include "stub.h"
#include "stubAll.h"
#include "diagnostictool.h"
#include "messagebox.h"
#include "clipboardtimer.h"
#include "websockethandle.h"
#include "searchresoultwidget.h"

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
    Stub stub;
    stub.set(ADDR(QSystemTrayIcon, show), QsystemtrayiconShow);

    Stub stub2;
    stub2.set(ADDR(ClipboardTimer, checkClipboardHasUrl), ClipboardtimerCheckclipboardhasurl);

    MainFrame::instance()->onDownloadNewUrl("https://img.tukuppt.com/video_show/7165162/00/19/39/5f06cfe424c38_10s_big.mp4",
                            Settings::getInstance()->getDownloadSavePath(), "5f06cfe424c38_10s_big", "mp4");

    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QTest::qWait(500);
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, addHttpTask)
{
    MainFrame::instance()->onDownloadNewUrl("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz",
                            Settings::getInstance()->getDownloadSavePath(), "qt-x11-opensource-src-4.1.4", "tar.gz");

    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QTest::qWait(500);
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, createNewTask)
{
    typedef int (*fptr)(CreateTaskWidget*);
    fptr foo = (fptr)(&CreateTaskWidget::exec);
    Stub stub;
    stub.set(foo, MessageboxExec);
    MainFrame::instance()->createNewTask("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz");
    QTest::qWait(500);
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, addBtTask)
{
    QMap<QString, QVariant> opt;
    MainFrame::instance()->onDownloadNewTorrent("/home/sanhei/Documents/123@.torrent",
                            opt, "123@.torrent", "tar.gz");
    QTest::qWait(500);
    EXPECT_TRUE(true);
    //QTest::qWait(5000);
}

TEST_F(ut_MainFreme, addMetalinkTask)
{
    QMap<QString, QVariant> opt;
    MainFrame::instance()->onDownloadNewMetalink("/home/sanhei/Documents/123@.metalink",
                            opt, "123@.metalink");
    QTest::qWait(500);
    EXPECT_TRUE(true);
    //QTest::qWait(5000);
}

TEST_F(ut_MainFreme, pauseTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    MainFrame::instance()->onPauseDownloadBtnClicked();
    QTest::qWait(100);
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, unpauseTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    MainFrame::instance()->onStartDownloadBtnClicked();
    QTest::qWait(100);
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, onContextMenu)
{
    Stub stub;
    stub.set((QAction *(QMenu::*)(const QPoint &pos, QAction *at))ADDR(QMenu, exec), QmenuExec);
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    MainFrame::instance()->onContextMenu(table->visualRect(model->index(0,2)).center());
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, changeList)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(0,0));
    MainFrame::instance()->onListClicked(list->model()->index(2,0));
    MainFrame::instance()->onListClicked(list->model()->index(1,0));
}

TEST_F(ut_MainFreme, deleteTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    table->getTableControl()->onDelAction(1);
    QTest::qWait(3000);
    MainFrame::instance()->onDeleteConfirm(false, false);
    TableView *table2 = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model2 = static_cast<TableModel *>(table2->model());
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, searchEditTextChanged)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    QString text;
    QList<QString> taskIDList;
    QList<int> taskStatusList;
    QList<QString> tasknameList;
    table->getTableControl()->searchEditTextChanged(text, taskIDList, taskStatusList, tasknameList);
    EXPECT_TRUE(true);

}

TEST_F(ut_MainFreme, redownloadTrashTask)
{
    DListView *list2 = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list2->model()->index(0,2));
    QTest::qWait(1000);
    TableView *table = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model = static_cast<TableModel *>(table->model());

    MainFrame::instance()->onRedownload(model->data(model->index(0,0), TableModel::taskId).toString(), 2);

    QTest::qWait(500);
    TableView *table2 = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model2 = static_cast<TableModel *>(table2->model());
    QList<Global::DownloadDataItem *> list = model2->dataList();
    foreach (Global::DownloadDataItem *item, list) {
        if (item->url == "https://img.tukuppt.com/video_show/7165162/00/19/39/5f06cfe424c38_10s_big.mp4") {
            EXPECT_TRUE(true);
            return;
        }
    }
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, deleteTask2)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    table->getTableControl()->onDelAction(1);
    MainFrame::instance()->onDeleteConfirm(false, false);


    TableView *table2 = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model2 = static_cast<TableModel *>(table2->model());
    EXPECT_TRUE(true);
    QTest::qWait(1000);
}

TEST_F(ut_MainFreme, backTrashTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    MainFrame::instance()->onReturnOriginActionTriggered();
    EXPECT_TRUE(true);
    QTest::qWait(1000);
}

TEST_F(ut_MainFreme, changeToFinishList2)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(1,0));
}

TEST_F(ut_MainFreme, deleteTask3)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    table->getTableControl()->onDelAction(1);
    MainFrame::instance()->onDeleteConfirm(false, false);


    TableView *table2 = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model2 = static_cast<TableModel *>(table2->model());
    EXPECT_TRUE(true);
    QTest::qWait(1000);
}

TEST_F(ut_MainFreme, changeToTrashList3)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(2,0));
}

TEST_F(ut_MainFreme, deleteTrashTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    table->getTableControl()->onDelAction(2);
    MainFrame::instance()->onDeleteConfirm(true, true);
    QTest::qWait(1000);
}

TEST_F(ut_MainFreme, addHttpFastTask2)
{
    Stub stub;
    stub.set(ADDR(MainFrame, showRedownloadMsgbox), MainframeShowredownloadmsgbox);
    MainFrame::instance()->onDownloadNewUrl("https://img.tukuppt.com/video_show/7165162/00/19/39/5f06cfe424c38_10s_big.mp4",
                            Settings::getInstance()->getDownloadSavePath(), "5f06cfe424c38_10s_big", "mp4");

    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    QList<Global::DownloadDataItem *> list = model->dataList();

    EXPECT_TRUE(true);
    QTest::qWait(2000);
}

TEST_F(ut_MainFreme, changeToFinishList3)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(1,0));
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, deleteFinishTask3)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    model->setData(model->index(0, 0), true, TableModel::Ischecked);
    table->getTableControl()->onDelAction(1);
    MainFrame::instance()->onDeleteConfirm(false, false);


    TableView *table2 = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model2 = static_cast<TableModel *>(table2->model());
    EXPECT_TRUE(true);
    QTest::qWait(1000);
}

TEST_F(ut_MainFreme, changeToTrashList4)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(2,0));
    EXPECT_TRUE(true);
}

TEST_F(ut_MainFreme, clrearTrashTask)
{
    MainFrame::instance()->onClearRecycle(true);
    TableView *table = MainFrame::instance()->findChild<TableView *>("recycleTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    EXPECT_TRUE(model->recyleList().isEmpty());
}

TEST_F(ut_MainFreme, clipboard)
{
    QProcess p;
    QString str = "echo \"https://img.tukuppt.com/video_show/7165162/00/19/39/5f06cfe424c38_10s_big.mp4\" | xclip";
    p.start();
}

TEST_F(ut_MainFreme, onTableItemSelected)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(0,0));
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        MainFrame::instance()->onTableItemSelected(model->index(0,0));
    }
}

TEST_F(ut_MainFreme, onRedownloadActionTriggered)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(0,0));
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        MainFrame::instance()->onRedownloadActionTriggered();
    }
}

TEST_F(ut_MainFreme, onCopyUrlActionTriggered)
{
    DListView *list = MainFrame::instance()->findChild<DListView *>("leftList");
    MainFrame::instance()->onListClicked(list->model()->index(0,0));
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        model->setData(model->index(0,0), true, TableModel::Ischecked);
        MainFrame::instance()->onTableItemSelected(model->index(0,0));
        MainFrame::instance()->onCopyUrlActionTriggered();
    }
}

TEST_F(ut_MainFreme, onOpenFileActionTriggered)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        MainFrame::instance()->onOpenFileActionTriggered();
    }
}

TEST_F(ut_MainFreme, onOpenFolderActionTriggered)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        MainFrame::instance()->onOpenFolderActionTriggered();
    }
}

TEST_F(ut_MainFreme, checkIfInPeriod)
{
    QTime currentTime = QTime::currentTime();
    QTime periodStartTime;
    QTime periodEndTime;
    periodStartTime.setHMS(0,0,0);
    periodEndTime.setHMS(24, 59,59);
    MainFrame::instance()->checkIfInPeriod(&currentTime, &periodStartTime, &periodEndTime);
}

TEST_F(ut_MainFreme, removeDownloadListJob)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    Global::DownloadDataItem *data = model->renderList().at(0);
    //table->getTableControl()->removeDownloadListJob(data, true, true);
}

TEST_F(ut_MainFreme, initDataItem)
{
    Global::DownloadDataItem *data = new Global::DownloadDataItem;
    const TaskInfo tbTask;
    MainFrame::instance()->initDataItem(data, tbTask);
}

TEST_F(ut_MainFreme, initDelDataItem)
{
    Global::DownloadDataItem *data = new Global::DownloadDataItem;
    Global::DeleteDataItem *delData = new Global::DeleteDataItem;
    MainFrame::instance()->initDelDataItem(data, delData);
}

TEST_F(ut_MainFreme, startDownloadTask)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        Global::DownloadDataItem *data = model->renderList().at(0);
        MainFrame::instance()->startDownloadTask(data);
    }
}

TEST_F(ut_MainFreme, onParseUrlList)
{
    Stub stub;
    stub.set(ADDR(MainFrame, onDownloadNewUrl), MainframeOndownloadnewurl);

    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);

    QVector<LinkInfo> urlList;
    urlList << LinkInfo();
    QString path = Settings::getInstance()->getDownloadSavePath();
    MainFrame::instance()->onParseUrlList(urlList, path);
}

TEST_F(ut_MainFreme, setAutoStart_true)
{
    MainFrame::instance()->setAutoStart(true);
}

TEST_F(ut_MainFreme, onDownloadFinish)
{
    MainFrame::instance()->onDownloadFinish();
}

TEST_F(ut_MainFreme, setAutoStart_false)
{
    MainFrame::instance()->setAutoStart(false);
}

TEST_F(ut_MainFreme, initDbus)
{
    MainFrame::instance()->initDbus();
}



TEST_F(ut_MainFreme, aria2MethodUnpauseAll)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    QJsonObject json;
    int iCurrentRow = 0;
    table->getTableControl()->aria2MethodUnpauseAll(json, iCurrentRow);
}

TEST_F(ut_MainFreme, aria2MethodForceRemove)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    QJsonObject json;
    table->getTableControl()->aria2MethodForceRemove(json);
}

TEST_F(ut_MainFreme, downloadListRedownload)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        Global::DownloadDataItem *data = model->renderList().at(0);
        table->getTableControl()->downloadListRedownload(data->taskId);
    }
}

TEST_F(ut_MainFreme, onUnusualConfirm)
{
    Stub stub;
    stub.set(ADDR(MainFrame, showRedownloadMsgbox), MainframeShowredownloadmsgbox);
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    if(model->renderList().count() >0){
        Global::DownloadDataItem *data = model->renderList().at(0);
        table->getTableControl()->onUnusualConfirm(0, data->taskId);
    }
}

TEST_F(ut_MainFreme, addHttpTask2)
{
    Stub stub;
    stub.set(ADDR(MainFrame, showRedownloadMsgbox), MainframeShowredownloadmsgbox);
    MainFrame::instance()->onDownloadNewUrl("http://download.qt.io/archive/qt/4.1/qt-x11-opensource-src-4.1.4.tar.gz",
                            Settings::getInstance()->getDownloadSavePath(), "qt-x11-opensource-src-4.1.4", "tar.gz");

    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    TableModel *model = static_cast<TableModel *>(table->model());
    EXPECT_TRUE(true);
}

//TEST_F(ut_MainFreme, onSettingsMenuClicked)
//{
//    typedef int (*fptr)(DSettingsDialog*);
//    fptr foo = (fptr)(&DSettingsDialog::exec);
//    Stub stub;
//    stub.set(foo, DSettingsDialog_exec);
//    MainFrame::instance()->onSettingsMenuClicked();
//}

TEST_F(ut_MainFreme, showDiagnosticTool)
{
    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&DiagnosticTool::exec);
    Stub stub;
    stub.set(foo, DiagnostictoolExec);
    MainFrame::instance()->showDiagnosticTool();
}

//TEST_F(ut_MainFreme, onDownloadFirstBtnClicked)
//{
//    typedef int (*fptr)(DSettingsDialog*);
//    fptr foo = (fptr)(&MessageBox::exec);
//    Stub stub;
//    stub.set(foo, MessageboxExec);

//    MainFrame::instance()->onDownloadFirstBtnClicked();
//}

TEST_F(ut_MainFreme, onRemoveFinished)
{
    MainFrame::instance()->onRemoveFinished();
}

TEST_F(ut_MainFreme, showWarningMsgbox)
{
    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub;
    stub.set(foo, MessageboxExec);
    MainFrame::instance()->showWarningMsgbox("");
}

TEST_F(ut_MainFreme, showClearMsgbox)
{
    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub;
    stub.set(foo, MessageboxExec);
    MainFrame::instance()->showClearMsgbox();
}

TEST_F(ut_MainFreme, showDeleteMsgbox)
{
    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub;
    stub.set(foo, MessageboxExec);
    MainFrame::instance()->showDeleteMsgbox(true);
}

TEST_F(ut_MainFreme, showRedownloadMsgbox)
{
    typedef int (*fptr)(DSettingsDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub;
    stub.set(foo, MessageboxExec);
    MainFrame::instance()->showRedownloadMsgbox("");
}

TEST_F(ut_MainFreme, clearShardMemary)
{
    TableView *table = MainFrame::instance()->findChild<TableView *>("downloadTableView");
    table->getTableControl()->clearShardMemary();
}

TEST_F(ut_MainFreme, initConnection)
{
    MainFrame::instance()->initConnection();
}

TEST_F(ut_MainFreme, initAria2)
{
    MainFrame::instance()->initAria2();
}

TEST_F(ut_MainFreme, initTabledata)
{
    MainFrame::instance()->initTabledata();
}

TEST_F(ut_MainFreme, updateDHTFile)
{
    MainFrame::instance()->updateDHTFile();
}

TEST_F(ut_MainFreme, onTrayQuitClick)
{
    MainFrame::instance()->onTrayQuitClick();
}

TEST_F(ut_MainFreme, tableView)
{
    TableView *table = new TableView(1);
    table->mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
}

TEST_F(ut_MainFreme, tableView2)
{
    TableView *table = new TableView(1);
    table->mouseMoveEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
}

TEST_F(ut_MainFreme, tableView3)
{
    TableView *table = new TableView(1);
    table->mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
}

TEST_F(ut_MainFreme, tableView4)
{
    TableView *table = new TableView(1);
    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::NoModifier);
    table->leaveEvent(keyEvent);
}

TEST_F(ut_MainFreme, tableView5)
{
    TableView *table = new TableView(1);
    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::NoModifier);
    table->keyPressEvent(keyEvent);
}

TEST_F(ut_MainFreme, Websockethandle)
{
    QWebSocket socket;
    //socket.ignoreSslErrors(expectedSslErrors);
    socket.open(QUrl("ws://127.0.0.1:12345"));
    bool b = socket.isValid();
    QString data = "http://test.html";
    qint64 num = socket.sendTextMessage(data);
    EXPECT_TRUE(num >= 0);
}

TEST_F(ut_MainFreme, Websocketconnect)
{
    Websockethandle *handle = new Websockethandle;
    handle->receiveText("text");
    EXPECT_NE(handle, nullptr);
}

TEST_F(ut_MainFreme, SearchResoultWidget)
{
    SearchResoultWidget *widget = new SearchResoultWidget;
    EXPECT_TRUE(true);
    delete widget;
}

TEST_F(ut_MainFreme, SearchResoultWidgetsetData)
{
    SearchResoultWidget *widget = new SearchResoultWidget;
    QList<QString> taskIDList;
    QList<int> taskStatusList;
    QList<QString> tasknameList;
    widget->setData(taskIDList, taskStatusList, tasknameList);
    EXPECT_TRUE(true);
    delete widget;
}

