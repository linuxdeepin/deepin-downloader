// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file main.cpp
 *
 * @brief main函数
 *
 * @date 2020-09-17 13:41
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <DApplication>
#include <DGuiApplicationHelper>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DSwitchButton>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationSettings>
#endif
#include <QTranslator>
#include <QClipboard>
#include <QSharedMemory>
#include <QBuffer>
#include <QCommandLineParser>
#include <QDBusMessage>
#include <QStringList>
#include <QDBusConnection>
#include <QPushButton>
#include <QCheckBox>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QAccessible>

#include "mainframe.h"
#include "log.h"
#include "settings.h"
#include "config.h"
#include "dlmapplication.h"
#include "accessiblewidget.h"
#include "ddlog.h"
#include "logger.h"
DWIDGET_USE_NAMESPACE

QString readShardMemary(QSharedMemory &sharedMemory);
void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl);
bool checkProcessExist();
QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object);

int main(int argc, char *argv[])
{
    // 动态日志规则必须最先初始化：在任何日志打印之前构造 MLogger，
    // 否则 QT_LOGGING_RULES 环境变量取消设置不生效（参考 deepin-reader）
    static MLogger logger;
    Q_UNUSED(logger)

    qCDebug(downloaderLog) << "[Main] Application starting";

    if (qEnvironmentVariableIsEmpty("XDG_CURRENT_DESKTOP")){
        qCDebug(downloaderLog) << "[Main] Setting XDG_CURRENT_DESKTOP to Deepin";
        qputenv("XDG_CURRENT_DESKTOP", "Deepin");
    }

    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
#if QT_VERSION_MAJOR <= 5
    if (XDG_SESSION_TYPE == QLatin1String("x11")) {
        qCDebug(downloaderLog) << "[Main] X11 session detected, loading DXcb plugin";
        DlmApplication::loadDXcbPlugin();
    }
#endif
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    DlmApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.loadTranslator(); //加载程序的翻译文件
    a.setOrganizationName("uos"); //设置公司名
    a.setApplicationName(QObject::tr("downloader")); //设置应用程序名
    a.setApplicationDisplayName(QObject::tr("Downloader"));
    a.setApplicationVersion(DLM_VERSION_STRING); //设置应用程序版本
    a.setProductIcon(QIcon::fromTheme("downloader", QIcon(":/icons/icon/downloader.svg")));
    a.setProductName(QObject::tr("Downloader")); //设置产品的名称
    a.setApplicationDescription(QObject::tr("Downloader is a user-friendly download tool, supporting URLs and torrent files")); //设置产品的描述信息
    //a.setApplicationDisplayName(QCoreApplication::translate("Main", "Uos Download Management Application")); //设置应用程序的显示信息

    // 安装工厂
    QAccessible::installFactory(accessibleFactory);

    //处理命令行类
    QCommandLineParser parser;
    parser.process(a);
    QStringList comList = parser.positionalArguments();
    QSharedMemory sharedMemory;
    sharedMemory.setKey("downloader");
    if (sharedMemory.attach()) //设置成单例程序
    {
        qCDebug(downloaderLog) << "[Main] Shared memory attached, checking for existing instance";
        if (!checkProcessExist()) { //下载器任务不存在，清空共享内存并启动
            qCDebug(downloaderLog) << "[Main] No existing process found, detaching shared memory";
            sharedMemory.detach();
        } else { //下载器任务存在
            qCDebug(downloaderLog) << "[Main] Existing instance found";
            if (comList.isEmpty()) {
                qCDebug(downloaderLog) << "[Main] No command line arguments, raising existing window";
                QDBusInterface iface("com.downloader.service",
                                     "/downloader/path",
                                     "local.downloader.MainFrame",
                                     QDBusConnection::sessionBus());
                iface.asyncCall("Raise");
            } else {
                qCDebug(downloaderLog) << "[Main] Writing new URL to shared memory:" << comList[0];
                if (sharedMemory.isAttached()) {
                    qCDebug(downloaderLog) << "[Main] Shared memory is attached, checking for duplicate URL";
                    if (readShardMemary(sharedMemory) == comList[0]) {
                        qCDebug(downloaderLog) << "[Main] main function ended with return code: 0";
                        return 0;
                    } else {
                        qCDebug(downloaderLog) << "[Main] New URL detected, writing to shared memory";
                        writeShardMemary(sharedMemory, comList[0]);
                    }
                }
                if (comList[0].contains(".torrent") || comList[0].contains(".metalink")) {
                    qCDebug(downloaderLog) << "[Main] Torrent or metalink file detected";
                    QDBusInterface iface("com.downloader.service",
                                         "/downloader/path",
                                         "local.downloader.MainFrame",
                                         QDBusConnection::sessionBus());
                    if(comList[0].contains("http://") || comList[0].contains("https://") || comList[0].contains("ftp://")) {
                        qCDebug(downloaderLog) << "[Main] URL detected, creating new task";
                        iface.asyncCall("createNewTask", comList[0]);
                    } else {
                        qCDebug(downloaderLog) << "[Main] Local file detected, opening file";
                        iface.asyncCall("OpenFile", comList[0]);
                        qCDebug(downloaderLog) << "[Main] Opening file:" << comList[0];
                    }
                }
            }
            return 0;
        }
    }
    if (sharedMemory.create(199)) {
        char *to = static_cast<char *>(sharedMemory.data());
        qCDebug(downloaderLog) << "[Main] Initializing shared memory";
        memset(to, 0, 199);
    }
#if QT_VERSION_MAJOR <= 5
    // 保存程序的窗口主题设置
    DApplicationSettings as;
    Q_UNUSED(as)
#endif
    QDir dirCheck;
    QString LogPath = QString("%1/%2/%3/Log/")
                           .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                           .arg(qApp->organizationName())
                           .arg(qApp->applicationName());

    setLogDir(LogPath);
    qCDebug(downloaderLog) << "[Main] Setting up log directory:" << LogPath;
    if (!dirCheck.exists(LogPath)) {
        qCDebug(downloaderLog) << "[Main] Creating log directory";
        dirCheck.mkpath(LogPath);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    // 注意：不再把文件日志级别压到 QtWarningMsg——分级开关由 DConfig 下发的
    // Qt 日志分类规则统一控制（调试模式按 debug/info/warning 三级动态调整），
    // 自定义 handler 内再做级别拦截会导致调试模式开不出 debug 级日志
    if(comList.size() == 1 && comList.first() == "debug") {
        qCDebug(downloaderLog) << "[Main] Debug mode enabled";
        setLogLevel(0);
    }
    CreateNewLog();
    qCDebug(downloaderLog) << "[Main] Log system initialized";
    qInstallMessageHandler(customLogMessageHandler);

    qCDebug(downloaderLog) << LogPath; //QStandardPaths::displayName(QStandardPaths::ConfigLocation);
    MainFrame w;
    Dtk::Widget::moveToCenter(&w);
    w.show();
    qCDebug(downloaderLog) << "[Main] Main window shown";

    for (int i = 0; i < comList.size(); i++) {
        if (comList[i].endsWith(".torrent") || comList[i].endsWith(".metalink")) {
            // qDebug() << "[Main] Processing torrent/metalink file:" << comList[i];
            if (Settings::getInstance()->getOneClickDownloadState()) {
                // qDebug() << "[Main] One-click download enabled, hiding window";
                w.hide();
            }
            w.OpenFile(comList[i]);
        }
    }
    QObject::connect(&a, &DlmApplication::applicatinQuit, &w, &MainFrame::onTrayQuitClick);

    return a.exec();
}

QString readShardMemary(QSharedMemory &sharedMemory)
{
    qCDebug(downloaderLog) << "[Main] Reading shared memory";
    sharedMemory.lock();
    char *to = static_cast<char *>(sharedMemory.data());
    sharedMemory.unlock();
    return QString(to);
}

void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl)
{
    qCDebug(downloaderLog) << "[Main] Writing to shared memory:" << strUrl;
    sharedMemory.lock();
    char *to = static_cast<char *>(sharedMemory.data());
    QByteArray array = strUrl.toLocal8Bit();
    QBuffer buffer;
    buffer.setBuffer(&array);
    const char *from = buffer.data().constData();
    int size = strUrl.size();
    int num = qMin(size, sharedMemory.size());
    memset(to, 0, 199);
    memcpy(to, from, static_cast<size_t>(num));
    sharedMemory.unlock();
}

bool checkProcessExist()
{
    qCDebug(downloaderLog) << "[Main] Checking for existing process";
    QProcess process;
    QStringList list;
#if QT_VERSION_MAJOR > 5
    process.start("pgrep", {"downloader"});
#else
    process.start("pgrep downloader");
    process.start();
#endif
    process.waitForStarted(1000);
    process.waitForFinished(1000);
    QString str = process.readAll();
    QStringList strList = str.split('\n');
    if (strList.size() > 1 && strList.at(1).isEmpty()) {
        qCDebug(downloaderLog) << "[Main] No existing process found";
        return false;
    }
    qCDebug(downloaderLog) << "[Main] Existing process found";
    return true;
}

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    // qDebug() << "[Main] accessibleFactory function started for class:" << classname;
    QAccessibleInterface *interface = nullptr;

    if (object && object->isWidgetType()) {
        // qDebug() << "[Main] Object is a widget type";
        if (classname == "QLabel") {
            // qDebug() << "[Main] Creating AccessibleLabel interface";
            interface = new AccessibleLabel(qobject_cast<QLabel *>(object));
        }

        if (classname == "QPushButton") {
            // qDebug() << "[Main] Creating AccessibleButton interface";
            interface = new AccessibleButton(qobject_cast<QPushButton *>(object));
        }

//        if (classname == "QCheckBox")
//            interface = new AccessibleCheckBox(qobject_cast<QCheckBox *>(object));
    }
    // qDebug() << "[Main] accessibleFactory function ended";
    return interface;
}
