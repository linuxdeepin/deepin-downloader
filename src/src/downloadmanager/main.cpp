#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <QTranslator>
#include <QClipboard>
#include <QCommandLineParser>
#include "mainframe.h"
#include "log.h"
#include "settings.h"
DWIDGET_USE_NAMESPACE



int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    DApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.loadTranslator();//加载程序的翻译文件
    a.setOrganizationName("uos");//设置公司名
    a.setApplicationName("downloadmanager");//设置应用程序名
    a.setApplicationVersion("5.1.0.2");//设置应用程序版本
    a.setProductIcon(QIcon(":/icons/icon/downloader4.svg"));//从系统主题中获取图标并设置成产品图标
    auto download_manager_name = QObject::tr("Download Manager");
    a.setProductName(download_manager_name);//设置产品的名称

    auto download_manager_info = QObject::tr("This is a download manage application.");
    a.setApplicationDescription(download_manager_info);//设置产品的描述信息

    a.setApplicationDisplayName(QCoreApplication::translate("Main", "Uos Download Management Application"));//设置应用程序的显示信息

    //处理命令行类
    QCommandLineParser parser;
    parser.process(a);
    QStringList _comList = parser.positionalArguments();
    if (!a.setSingleInstance("downloadmanager"))//设置成单例程序
    {
        QClipboard *_c = QApplication::clipboard();
        if(_comList.isEmpty())
        {
            _c->setText("start_manager_for_clipboard");
        }
        else
        {
            //发送以.torrent结尾文件
            _c->setText(_comList[0]);
        }

        return 0;
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
    qInstallMessageHandler(customLogMessageHandler);

    qDebug()<<Log_path;//QStandardPaths::displayName(QStandardPaths::ConfigLocation);
    MainFrame w;
    w.show();
    for (int i = 0; i < _comList.size(); i++)
    {
        if(_comList[i].endsWith(".torrent"))
        {
            if(Settings::getInstance()->getOneClickDownloadState())
            {
                w.hide();
            }
             w.onClipboardDataForBt(_comList[i]);     
        }
    }
    w.setWindowIcon(QIcon(":/icons/icon/downloader4.svg"));
    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
