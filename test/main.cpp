
#include "gtest/gtest.h"
#include <QApplication>
#include <QtTest>

class QTestMain;

class QTestMain : public QObject
{
    Q_OBJECT

public:
    QTestMain();
    ~QTestMain();

    void testGTest(int argc, char *argv[]);
private slots:
    void initTestCase();
    void cleanupTestCase();
};

QTestMain::QTestMain()
{
}

QTestMain::~QTestMain()
{
}

void QTestMain::initTestCase()
{
}

void QTestMain::cleanupTestCase()
{
}

void QTestMain::testGTest(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    Q_UNUSED(ret)
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setOrganizationName("uos"); //设置公司名
    a.setApplicationName("downloader"); //设置应用程序名
    QTestMain tc;
    tc.testGTest(argc, argv);
    return QTest::qExec(&tc, argc, argv);
}

//#include <QtTest>
//#include <QCoreApplication>
//#include <QApplication>
//#include <QObject>
//#include "mainframe.h"
//#include "gtest/gtest.h"
//#include <gmock/gmock-matchers.h>

//DWIDGET_USE_NAMESPACE

//#define QMYTEST_MAIN(TestObject) \
//    QT_BEGIN_NAMESPACE \
//    QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS \
//    QT_END_NAMESPACE \
//    int main(int argc, char *argv[]) \
//    { \
//        QCoreApplication app(argc, argv); \
//        app.setAttribute(Qt::AA_Use96Dpi, true); \
//        QTEST_DISABLE_KEYPAD_NAVIGATION; \
//        QTEST_ADD_GPU_BLACKLIST_SUPPORT; \
//        TestObject tc; \
//        tc.testGTest(argc, argv); \
//        QTEST_SET_MAIN_SOURCE_PATH; \
//        return QTest::qExec(&tc, argc, argv); \
//    }

//QMYTEST_MAIN(QTestMain)

#include "main.moc"
