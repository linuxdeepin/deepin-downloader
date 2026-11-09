// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
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
#include <DMainWindow>
#include <DWidgetUtil>
#include <DSwitchButton>
#include <DApplicationSettings>
#include <QFont>
#include <QToolTip>
#include <QTimer>
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
DWIDGET_USE_NAMESPACE

QString readShardMemary(QSharedMemory &sharedMemory);
void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl);
bool checkProcessExist();
QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object);

int main(int argc, char *argv[])
{

    if (qEnvironmentVariableIsEmpty("XDG_CURRENT_DESKTOP")){
            qputenv("XDG_CURRENT_DESKTOP", "Deepin");
    }

    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    if (XDG_SESSION_TYPE == QLatin1String("x11")) {
        DlmApplication::loadDXcbPlugin();
    }
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    DlmApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.loadTranslator(); //加载程序的翻译文件
    a.setOrganizationName("uos"); //设置公司名
    a.setApplicationName(QObject::tr("downloader")); //设置应用程序名
    a.setApplicationDisplayName(QObject::tr("Downloader"));
    a.setApplicationVersion(DLM_VERSION_STRING); //设置应用程序版本
    a.setProductIcon(QIcon(":/icons/icon/downloader.svg")); //从系统主题中获取图标并设置成产品图标
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
        if (!checkProcessExist()) { //下载器任务不存在，清空共享内存并启动
            sharedMemory.detach();
        } else { //下载器任务存在
            if (comList.isEmpty()) {
                QDBusInterface iface("com.downloader.service",
                                     "/downloader/path",
                                     "local.downloader.MainFrame",
                                     QDBusConnection::sessionBus());
                iface.asyncCall("Raise");
            } else {
                if (sharedMemory.isAttached()) {
                    if (readShardMemary(sharedMemory) == comList[0]) {
                        return 0;
                    } else {
                        writeShardMemary(sharedMemory, comList[0]);
                    }
                }
                if (comList[0].contains(".torrent") || comList[0].contains(".metalink")) {
                    QDBusInterface iface("com.downloader.service",
                                         "/downloader/path",
                                         "local.downloader.MainFrame",
                                         QDBusConnection::sessionBus());
                    if(comList[0].contains("http://") || comList[0].contains("https://") || comList[0].contains("ftp://")) {
                        iface.asyncCall("createNewTask", comList[0]);
                    } else {
                        iface.asyncCall("OpenFile", comList[0]);
                        qDebug() << "OpenFile :  " << comList[0];
                    }
                }
            }
            return 0;
        }
    }
    if (sharedMemory.create(199)) {
        char *to = static_cast<char *>(sharedMemory.data());
        memset(to, 0, 199);
    }
    // 保存程序的窗口主题设置
    DApplicationSettings as;
    Q_UNUSED(as)

    // 放大 QToolTip 字体，解决任务列表名称悬浮提示太小的问题
    // 使用 1.5 倍（50% 放大），通过 QToolTip::setFont() 设置 QTipLabel 字体
    // 同时处理 pointSize 和 pixelSize 两种情况（DDE 主题可能使用 pixelSize）
    // 注意：Qt 5.11 的 tooltip 窗口类名是 QTipLabel，字体 role 是 "QTipLabel"；
    //       必须用 QToolTip::setFont()（内部即 setFont(..., "QTipLabel")），
    //       不能用 setFont(font, "QToolTip")——该 class 名不存在，放大不生效。
    // 注意：不能使用 qApp->setStyleSheet 设置 tooltip 字体，实测会导致 setFont 被重置
    {
        // 存储初始字体大小，用于后续重新应用时避免叠加放大
        static int s_basePointSize = QToolTip::font().pointSize();
        static int s_basePixelSize = QToolTip::font().pixelSize();

        auto enlargeTooltipFont = []() {
            QFont tooltipFont = QToolTip::font();
            int basePointSize = s_basePointSize;
            int basePixelSize = s_basePixelSize;

            if (basePointSize > 0) {
                // 字体使用 pointSize，放大 1.5 倍
                tooltipFont.setPointSize(qRound(basePointSize * 1.5));
            } else if (basePixelSize > 0) {
                // DDE 主题使用 pixelSize，放大 1.5 倍
                tooltipFont.setPixelSize(qRound(basePixelSize * 1.5));
            } else {
                // 回退到应用默认字体
                int appPointSize = qApp->font().pointSize();
                int appPixelSize = qApp->font().pixelSize();
                if (appPointSize > 0) {
                    tooltipFont.setPointSize(qRound(appPointSize * 1.5));
                } else if (appPixelSize > 0) {
                    tooltipFont.setPixelSize(qRound(appPixelSize * 1.5));
                } else {
                    // 最终回退：设置固定像素大小
                    tooltipFont.setPixelSize(18);
                }
            }
            QToolTip::setFont(tooltipFont);

            qDebug() << "[ToolTipFont] QToolTip basePointSize=" << basePointSize
                     << "basePixelSize=" << basePixelSize
                     << "newPointSize=" << tooltipFont.pointSize()
                     << "newPixelSize=" << tooltipFont.pixelSize();
        };

        // 立即应用
        enlargeTooltipFont();

        // 在事件循环启动后重新应用（防止 DDE 主题/样式在运行时覆盖 setFont）
        // 使用存储的初始大小计算，避免叠加放大
        QTimer::singleShot(0, qApp, enlargeTooltipFont);
    }
    QDir dirCheck;
    QString LogPath = QString("%1/%2/%3/Log/")
                           .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                           .arg(qApp->organizationName())
                           .arg(qApp->applicationName());

    setLogDir(LogPath);
    if (!dirCheck.exists(LogPath)) {
        dirCheck.mkpath(LogPath);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    setLogLevel(1);
    if(comList.size() == 1 && comList.first() == "debug") {
        setLogLevel(0);
    }
    CreateNewLog();
    qInstallMessageHandler(customLogMessageHandler);

    qDebug() << LogPath; //QStandardPaths::displayName(QStandardPaths::ConfigLocation);
    MainFrame w;
    Dtk::Widget::moveToCenter(&w);
    w.show();

    for (int i = 0; i < comList.size(); i++) {
        if (comList[i].endsWith(".torrent") || comList[i].endsWith(".metalink")) {
            if (Settings::getInstance()->getOneClickDownloadState()) {
                w.hide();
            }
            w.OpenFile(comList[i]);
        }
    }
    w.setWindowIcon(QIcon(":/icons/icon/downloader.svg"));
    QObject::connect(&a, &DlmApplication::applicatinQuit, &w, &MainFrame::onTrayQuitClick);

    return a.exec();
}

QString readShardMemary(QSharedMemory &sharedMemory)
{
    sharedMemory.lock();
    char *to = static_cast<char *>(sharedMemory.data());
    sharedMemory.unlock();
    return QString(to);
}

void writeShardMemary(QSharedMemory &sharedMemory, QString strUrl)
{
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
    QProcess process;
    QStringList list;
    process.start("pgrep downloader");
    process.start();
    process.waitForStarted(1000);
    process.waitForFinished(1000);
    QString str = process.readAll();
    QStringList strList = str.split('\n');
    if (strList.size() > 1 && strList.at(1).isEmpty()) {
        return false;
    }
    return true;
}

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;

    if (object && object->isWidgetType()) {
        if (classname == "QLabel")
            interface = new AccessibleLabel(qobject_cast<QLabel *>(object));

        if (classname == "QPushButton")
            interface = new AccessibleButton(qobject_cast<QPushButton *>(object));

//        if (classname == "QCheckBox")
//            interface = new AccessibleCheckBox(qobject_cast<QCheckBox *>(object));
    }
    return interface;
}
