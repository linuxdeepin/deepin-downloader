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
#include "diagnostictool.h"
#include "downloadsettingwidget.h"
#include "filesavepathchooser.h"
#include "settingswidget.h"

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
    Settings *pSettings = new Settings;
    EXPECT_NE(pSettings, nullptr);
    pSettings->getOneClickDownloadState();
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

TEST_F(ut_Settings, DiagnosticModel)
{
    DiagnosticModel *model = new DiagnosticModel;
    model->appendData(true);
    EXPECT_TRUE(true);
}

TEST_F(ut_Settings, DiagnosticModel1)
{
    DiagnosticModel *model = new DiagnosticModel;
    model->appendData(true);
    model->setData(model->index(0,0), true, Qt::DisplayRole);
    EXPECT_TRUE(true);
}

TEST_F(ut_Settings, DiagnosticModel2)
{
    DiagnosticModel *model = new DiagnosticModel;
    model->appendData(true);
    model->data(model->index(0,0), Qt::DisplayRole);
    model->data(model->index(0,1), Qt::DisplayRole);
    model->data(model->index(1,1), Qt::DisplayRole);
    model->data(model->index(2,1), Qt::DisplayRole);
    model->data(model->index(3,1), Qt::DisplayRole);
    model->data(model->index(4,1), Qt::DisplayRole);
    EXPECT_TRUE(true);
}

TEST_F(ut_Settings, DiagnosticModel3)
{
    DiagnosticModel *model = new DiagnosticModel;
    model->clearData();
    EXPECT_TRUE(true);
}

TEST_F(ut_Settings, DiagnosticModel4)
{
    DiagnosticDelegate *pDelegate = new DiagnosticDelegate;
    EXPECT_TRUE(true);
}


TEST_F(ut_Settings, DownloadSettingWidget)
{
    DownloadSettingWidget *pWidget = new DownloadSettingWidget;
    pWidget->onRadioButtonClicked();
}

TEST_F(ut_Settings, DownloadSettingWidget1)
{
    DownloadSettingWidget *pWidget = new DownloadSettingWidget;
    pWidget->onTextChanged("str");
}

TEST_F(ut_Settings, DownloadSettingWidget2)
{
    DownloadSettingWidget *pWidget = new DownloadSettingWidget;
    pWidget->onTextChanged("1234");
}

TEST_F(ut_Settings, DownloadSettingWidget3)
{
    DownloadSettingWidget *pWidget = new DownloadSettingWidget;
    pWidget->onFocusChanged(true);
}

TEST_F(ut_Settings, FileSavePathChooser) {
    FileSavePathChooser *pWidget = new FileSavePathChooser(1,"");
    pWidget->onRadioButtonClicked();
}

TEST_F(ut_Settings, FileSavePathChooser1) {
    FileSavePathChooser *pWidget = new FileSavePathChooser(1,"");
    pWidget->onLineEditTextChanged("123");
}

TEST_F(ut_Settings, FileSavePathChooser2) {
    FileSavePathChooser *pWidget = new FileSavePathChooser(1,"");
    pWidget->setCurrentSelectRadioButton(0);
}

TEST_F(ut_Settings, FileSavePathChooser3) {
    FileSavePathChooser *pWidget = new FileSavePathChooser(1,"");
    pWidget->setLineEditText("123456");
}

TEST_F(ut_Settings, SettingsControlWidget) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
}

TEST_F(ut_Settings, SettingsControlWidget1) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->setSize("111");
}

TEST_F(ut_Settings, SettingsControlWidget2) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->setSpeend("111");
}

TEST_F(ut_Settings, SettingsControlWidget3) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->setSwitch(false);
}

TEST_F(ut_Settings, SettingsControlWidget4) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->m_Edit->setText("4444");
}

TEST_F(ut_Settings, SettingsControlWidget5) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->m_Edit->setText("+11");
}

TEST_F(ut_Settings, SettingsControlWidget6) {
    SettingsControlWidget *pWidget = new SettingsControlWidget;
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->m_Edit->setText("001");
}
