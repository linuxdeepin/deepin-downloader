#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <QTranslator>
#include <QClipboard>
#include <QSharedMemory>
#include <QBuffer>
#include <QCommandLineParser>
#include <QDBusMessage>
#include <QStringList>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include "mainframe.h"
#include "log.h"
#include "settings.h"
#include "config.h"
#include "dlmapplication.h"
DWIDGET_USE_NAMESPACE

QString readShardMemary(QSharedMemory &sharedMemory);
void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl);
bool checkProcessExist();

int main(int argc, char *argv[])
{
    DlmApplication::loadDXcbPlugin();
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    DlmApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.loadTranslator();//加载程序的翻译文件
    a.setOrganizationName("uos");//设置公司名
    a.setApplicationName("downloadmanager");//设置应用程序名
    a.setApplicationVersion(DLM_VERSION_STRING);//设置应用程序版本
    a.setProductIcon(QIcon(":/icons/icon/downloadmanager.svg"));//从系统主题中获取图标并设置成产品图标
    auto download_manager_name = QObject::tr("Download Manager");
    a.setProductName(download_manager_name);//设置产品的名称
    auto download_manager_info = QObject::tr("This is a download manage application.");
    a.setApplicationDescription(download_manager_info);//设置产品的描述信息
    a.setApplicationDisplayName(QCoreApplication::translate("Main", "Uos Download Management Application"));//设置应用程序的显示信息

    //处理命令行类
    QCommandLineParser parser;
    parser.process(a);
    QStringList comList = parser.positionalArguments();
    QSharedMemory sharedMemory;
    sharedMemory.setKey("downloadmanager");
    if (sharedMemory.attach())//设置成单例程序
    {

        if(!checkProcessExist()){ //下载器任务不存在，清空共享内存并启动
            sharedMemory.detach();
        } else { //下载器任务存在
            if(comList.isEmpty()){
                QDBusInterface iface("com.downloadmanager.service",
                                     "/downloadmanager/path",
                                     "local.downloadmanager.MainFrame",
                                     QDBusConnection::sessionBus());
                iface.asyncCall("Raise");
            } else {
                if(sharedMemory.isAttached()){
                    if(readShardMemary(sharedMemory) == comList[0]){
                        return 0;
                    } else {
                        writeShardMemary(sharedMemory, comList[0]);
                    }
                }
                QDBusInterface iface("com.downloadmanager.service",
                                     "/downloadmanager/path",
                                     "local.downloadmanager.MainFrame",
                                     QDBusConnection::sessionBus());
                iface.asyncCall("OpenBt", comList[0]);
            }
            return 0;
        }
    }
    if(sharedMemory.create(199))
    {
        char *to = static_cast<char*>(sharedMemory.data());
        memset(to,0, 199);
    }


    // 保存程序的窗口主题设置
    DApplicationSettings as;
    Q_UNUSED(as)
    QDir dirCheck;
    QString Log_path = QString("%1/%2/%3/Log/")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    setLogDir(Log_path);
    if (!dirCheck.exists(Log_path))
    {
        dirCheck.mkpath(Log_path);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    CreateNewLog();
    //qInstallMessageHandler(customLogMessageHandler);

    qDebug()<<Log_path;//QStandardPaths::displayName(QStandardPaths::ConfigLocation);
    MainFrame w;
    w.show();
    for (int i = 0; i < comList.size(); i++)
    {
        if(comList[i].endsWith(".torrent"))
        {
            if(Settings::getInstance()->getOneClickDownloadState())
            {
                w.hide();
            }
             w.OpenBt(comList[i]);
        }
    }
    w.setWindowIcon(QIcon(":/icons/icon/downloadmanager.svg"));
    QObject::connect(&a, &DlmApplication::applicatinQuit, &w, &MainFrame::onTrayQuitClick);
    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}

QString readShardMemary(QSharedMemory &sharedMemory)
{
    sharedMemory.lock();
    char *to = static_cast<char*>(sharedMemory.data());
    sharedMemory.unlock();
    return QString(to);
}

void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl)
{
    sharedMemory.lock();
    char *to = static_cast<char*>(sharedMemory.data());
    QByteArray array = strUrl.toLocal8Bit();
    QBuffer buffer;
    buffer.setBuffer(&array);
    const char *from = buffer.data().constData();
    int size = strUrl.size();
    int num = qMin(size,sharedMemory.size());
    memcpy(to,from, static_cast<size_t>(num));
    sharedMemory.unlock();
}

bool checkProcessExist()
{
    QProcess process;
    QStringList list;
    process.start("pgrep downloadmanager");
    process.start();
    process.waitForStarted(1000);
    process.waitForFinished(1000);
    QString str = process.readAll();
    QStringList strList = str.split('\n');
    if(strList.at(1).isEmpty()){
        return false;
    }
    return true;
}
