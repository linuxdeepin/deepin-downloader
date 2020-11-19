
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
#include "main.moc"
