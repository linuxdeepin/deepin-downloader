/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file settings.cpp
 *
 * @brief 设置类，主要实现调用json文件，生成配置文件；创建自定义的控件窗口
 *
 *@date 2020-06-09 09:55
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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
#include "settings.h"

#include <DThemeManager>
#include <DSettings>
#include <DSettingsGroup>
#include <DSettingsWidgetFactory>
#include <DSettingsOption>

#include <QStyleFactory>
#include <QFontDatabase>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <DPushButton>

#include "filesavepathchooser.h"
#include "settinginfoinputwidget.h"
#include "itemselectionwidget.h"
#include "downloadsettingwidget.h"
#include "notificationssettiingwidget.h"
#include "settingslabel.h"
#include "settingswidget.h"
/**
 * @brief 判断字符串是否为纯数字
 * @param src 字符串
 * @return 返回0表示是纯数字，否则返回-1
 */
int isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1(); //QString 转换为 char*
    const char *s = ba.data();

    while (*s && *s >= '0' && *s <= '9')
        s++;

    if (*s) { //不是纯数字
        return -1;
    } else { //纯数字
        return 0;
    }
}

Settings *Settings::m_instance = nullptr;
Settings *Settings::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new Settings;
    }

    return m_instance;
}

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    m_configPath = QString("%1/%2/%3/config.conf")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());

    m_backend = new QSettingBackend(m_configPath);
    m_settings = DSettings::fromJsonFile(":/json/settings");

    m_settings->setBackend(m_backend);

    //上次保存文件位置以及右上角关闭时是否显示提示框
    QString iniConfigPath = QString("%1/%2/%3/usrConfig.conf")
                                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                .arg(qApp->organizationName())
                                .arg(qApp->applicationName());

    m_iniFile = new QSettings(iniConfigPath, QSettings::IniFormat);

    if (!m_iniFile->contains("FilePath/Filename")) {
        m_iniFile->setValue("FilePath/Filename", "");
    }

    if (!m_iniFile->contains("Close/showTip")) {
        m_iniFile->setValue("Close/showTip", "true");
    }

    // 初始化同时下载最大任务数
    auto maxDownloadTaskOption = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");

    QStringList values;
    QStringList keys;
    keys << "3"
         << "5"
         << "10"
         << "20";
    values << tr("3") << tr("5") << tr("10") << tr("20");

    QMap<QString, QVariant> mapData;
    mapData.insert("keys", keys);
    mapData.insert("values", values);

    maxDownloadTaskOption->setData("items", mapData);

    if (maxDownloadTaskOption->value().toString().isEmpty()) {
        maxDownloadTaskOption->setValue("5");
    }

    // 最大下载任务数
    auto maxDownloadTaskNumber = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");
    connect(maxDownloadTaskNumber, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit maxDownloadTaskNumberChanged(value.toInt());
        }
    });

    // 下载设置
    auto downloadSettingInfo = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");
    connect(downloadSettingInfo, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit downloadSettingsChanged();
        }
    });

    //速度小于多少自动增加任务窗口
    auto atoDownloadBySpeed = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoDownload");
    connect(atoDownloadBySpeed, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit autoDownloadBySpeedChanged(value.toString().left(1).toInt());
        }
    });

    //按下载速度排序
    auto atoSortBySpeed = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoSortBySpeed");
    connect(atoSortBySpeed, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit autoSortBySpeedChanged(value.toBool());
        }
    });

    // 下载磁盘缓存
    auto diskCacheNum = m_settings->option("AdvancedSetting.DownloadDiskCache.DownloadDiskCacheSettiing");
    connect(diskCacheNum, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit disckCacheChanged(value.toInt());
        }
    });

    // 开机启动
    auto poweronSwitchbutton = m_settings->option("Basic.Start.PowerOn");
    connect(poweronSwitchbutton, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit poweronChanged(value.toBool());
        }
    });

    // 启动时关联BT种子文件
    auto startAssociatedBTFile = m_settings->option("Monitoring.BTRelation.AssociateBTFileAtStartup");
    connect(startAssociatedBTFile, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit startAssociatedBTFileChanged(value.toBool());
        }
    });

    // 是否接管浏览器
    auto isControlBrowser = m_settings->option("Monitoring.MonitoringObject.WebBrowser");
    connect(isControlBrowser, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            emit controlBrowserChanged(value.toBool());
        }
    });

    // 实现剪切板和接管下载类型关联
    auto optionClipBoard = m_settings->option("Monitoring.MonitoringObject.ClipBoard");
    auto optionHttpDownload = m_settings->option("Monitoring.MonitoringDownloadType.HttpDownload");
    auto optionBTDownload = m_settings->option("Monitoring.MonitoringDownloadType.BTDownload");
    auto optionMagneticDownload = m_settings->option("Monitoring.MonitoringDownloadType.MagneticDownload");

    // 剪切板状态改变
    connect(optionClipBoard, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            if (value.toBool() == true) {
                if (!optionHttpDownload->value().toBool() && !optionBTDownload->value().toBool() && !optionMagneticDownload->value().toBool()) {
                    optionHttpDownload->setValue(true);
                    optionBTDownload->setValue(true);
                    optionMagneticDownload->setValue(true);
                }
            } else {
                optionHttpDownload->setValue(false);
                optionBTDownload->setValue(false);
                optionMagneticDownload->setValue(false);
            }
        }
    });

    // Http下载状态改变
    connect(optionHttpDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            if (value.toBool()) {
                if (!optionClipBoard->value().toBool()) {
                    optionClipBoard->setValue(true);
                }
            } else {
                if (!optionBTDownload->value().toBool() && !optionMagneticDownload->value().toBool()) {
                    optionClipBoard->setValue(false);
                }
            }
        }
    });

    // BT下载状态改变
    connect(optionBTDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            if (value.toBool()) {
                if (!optionClipBoard->value().toBool()) {
                    optionClipBoard->setValue(true);
                }
            } else {
                if (!optionHttpDownload->value().toBool() && !optionMagneticDownload->value().toBool()) {
                    optionClipBoard->setValue(false);
                }
            }
        }
    });

    // 磁力链接下载状态改变
    connect(optionMagneticDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        if (!value.isNull()) {
            if (value.toBool()) {
                if (!optionClipBoard->value().toBool()) {
                    optionClipBoard->setValue(true);
                }
            } else {
                if (!optionHttpDownload->value().toBool() && !optionBTDownload->value().toBool()) {
                    optionClipBoard->setValue(false);
                }
            }
        }
    });

    // json文件国际化
    auto basicName = tr("Basic"); // 基本设置
    auto startName = tr("Start up"); // 启动
    auto powerOnName = tr("Auto startup"); // 开机启动
    auto autoStartUnfinishedTaskName = tr("Resume downloading on startup"); // 启动后自动开始未完成的任务
    auto downloadDirectoryName = tr("Download Directory"); // 下载目录
    auto oneClickDownloadName = tr("1-Click Download"); // 一键下载
    auto createTasksDirectly = tr("Create tasks directly"); //一键创建新任务
    auto closeMainWindowName = tr("Close window"); // 关闭主界面
    auto minimizeToSystemTrayName = tr("Minimize to System Tray"); // 最小化到托盘
    auto exitName = tr("Exit"); // 退出下载器
    auto tasksName = tr("Tasks"); // 任务管理
    auto activeDownloadsName = tr("Max. concurrent downloads"); // 同时下载最大任务数
    auto openFileAfterDownloadName = tr("Open files when completed"); // 下载完成后自动打开
    auto deleteTasksWithoutFilesName = tr("Delete tasks without files"); // 自动删除“文件不存在”的任务
    auto downloadsName = tr("Downloads"); // 下载设置
    auto monitoringName = tr("Monitoring"); // 接管设置
    auto monitoringApplicationName = tr("Applications"); // 接管对象
    auto clipBoardName = tr("Clipboard"); // 剪切板
    auto WebBrowser = tr("WebBrowser"); // 浏览器
    auto monitoringDownloadLinkName = tr("Links Monitored"); // 接管下载类型
    auto monitoringBTFilesName = tr("BT Files"); // BT关联
    auto downloadingName = tr("Create new task when a torrent file downloaded"); // 下载种子文件后自动打开下载面板
    auto openingName = tr("Create new task when opening a torrent file"); // 启动时关联BT种子文件
    auto notificationsName = tr("Notifications"); // 通知提醒
    auto succeedOrFailedName = tr("Notify me when downloading finished or failed"); // 下载完成/失败时，系统通知提醒
    auto downloadingSucceedName = tr("Allow sounds when downloading succeed"); // 下载完成后，播放提示音
    auto advancedName = tr("Advanced"); // 高级设置
    auto shortcutsName = tr("Shortcuts"); // 快捷设置
    auto newTaskName = tr("Show main window when creating new task"); // 新建任务时显示主界面
    auto showMainName = tr("Show main window"); // 打开主界面快捷键
    auto diskCacheName = tr("Cache"); // 下载磁盘缓存
    auto AutoSortBySpeedName = tr("Move low speed tasks to the end"); // 低速的移动到末尾
}

QWidget *Settings::createFileChooserEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    int currentSelect = 2;
    QString downloadPath;

    if (option->value().toString().isEmpty()) {
        downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    } else {
        QString currentValue = option->value().toString();

        if (currentValue.contains("auto;")) {
            currentSelect = 1;
            QStringList currentValueList = currentValue.split(';');

            if (currentValueList.count() > 1) {
                downloadPath = currentValueList.at(1);

                if (downloadPath.isEmpty()) {
                    downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
            }
        } else {
            QStringList currentValueList = currentValue.split(';');

            if (currentValueList.count() > 1) {
                downloadPath = currentValueList.at(1);

                if (downloadPath.isEmpty()) {
                    downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
            }
        }
    }

    FileSavePathChooser *fileSavePathChooser = new FileSavePathChooser(currentSelect, downloadPath);

    connect(fileSavePathChooser, &FileSavePathChooser::textChanged, fileSavePathChooser, [=](QVariant var) {
        QString currentValue = var.toString();
        QString optionValue = option->value().toString();

        if (currentValue == "custom;" && !optionValue.isEmpty()) {
            QString oldPath = optionValue.section(QString(';'), 1, 1);

            if (!oldPath.isEmpty()) {
                currentValue = "custom;" + oldPath;
                option->setValue(currentValue);
            }
        } else {
            option->setValue(var.toString());
        }
    });

    connect(option, &DSettingsOption::valueChanged, fileSavePathChooser, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            QString currentValue = var.toString();
            QString currentPath = currentValue.section(QString(';'), 1, 1);

            if (currentPath.isEmpty()) {
                currentPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }

            fileSavePathChooser->setLineEditText(currentPath);

            if (currentValue.contains("custom;")) {
                fileSavePathChooser->setCurrentSelectRadioButton(2);
            } else {
                fileSavePathChooser->setCurrentSelectRadioButton(1);
            }
        }
    });

    return fileSavePathChooser;
}

QWidget *Settings::createHttpDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *itemSelectionWidget = new ItemSelectionWidget();
    itemSelectionWidget->setLabelText(tr("HTTP")); // HTTP下载
    itemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(itemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, itemSelectionWidget, [=](QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, itemSelectionWidget, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            itemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return itemSelectionWidget;
}

QWidget *Settings::createBTDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *itemSelectionWidget = new ItemSelectionWidget();
    itemSelectionWidget->setLabelText(tr("BitTorrent")); // BT下载
    itemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(itemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, itemSelectionWidget, [=](QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, itemSelectionWidget, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            itemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return itemSelectionWidget;
}

QWidget *Settings::createMagneticDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *itemSelectionWidget = new ItemSelectionWidget();
    itemSelectionWidget->setLabelText(tr("Magnet URI scheme")); // 磁力链接下载
    itemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(itemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, itemSelectionWidget, [=](QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, itemSelectionWidget, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            itemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return itemSelectionWidget;
}

QWidget *Settings::createDownloadSpeedLimitSettiingHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    int currentSelect = 2;
    QString maxDownloadSpeedLimit;
    QString maxUploadSpeedLimit;
    QString startTime;
    QString endTime;

    if (option->value().toString().isEmpty()) {
        maxDownloadSpeedLimit = "10240";
        maxUploadSpeedLimit = "32";
        startTime = "08:00:00";
        endTime = "17:00:00";
    } else {
        QString currentValue = option->value().toString();

        if (currentValue.contains("fullspeed;")) {
            currentSelect = 1;
        }

        QStringList currentValueList = currentValue.split(';');

        if (currentValueList.count() > 4) {
            maxDownloadSpeedLimit = currentValueList.at(1);
            maxUploadSpeedLimit = currentValueList.at(2);
            startTime = currentValueList.at(3);
            endTime = currentValueList.at(4);
        }
    }

    DownloadSettingWidget *downloadSettingWidget = new DownloadSettingWidget();
    downloadSettingWidget->setCurrentSelectRadioButton(currentSelect);
    downloadSettingWidget->setMaxDownloadSpeedLimit(maxDownloadSpeedLimit);
    downloadSettingWidget->setMaxUploadSpeedLimit(maxUploadSpeedLimit);
    downloadSettingWidget->setStartTime(startTime);
    downloadSettingWidget->setEndTime(endTime);

    connect(downloadSettingWidget, &DownloadSettingWidget::speedLimitInfoChanged, downloadSettingWidget, [=](QVariant var) {
        QString currentValue = var.toString();
        QStringList currentValueList = currentValue.split(';');

        if (currentValueList.count() > 4) {
            QString maxDownloadSpeedLimit = currentValueList.at(1);
            QString maxUploadSpeedLimit = currentValueList.at(2);

            if (maxDownloadSpeedLimit.toInt() >= 100 && maxDownloadSpeedLimit.toInt() <= 102400
                && maxUploadSpeedLimit.toInt() >= 16 && maxUploadSpeedLimit.toInt() <= 5120) {
                option->setValue(currentValue);
            }
        }
    });

    connect(option, &DSettingsOption::valueChanged, downloadSettingWidget, [=](QVariant var) {
        // option->blockSignals(true);
        if (!var.toString().isEmpty()) {
            QString currentValue = option->value().toString();
            int currentSelect = 2;
            QString maxDownloadSpeedLimit;
            QString maxUploadSpeedLimit;
            QString startTime;
            QString endTime;

            if (currentValue.isEmpty()) {
                maxDownloadSpeedLimit = "10240";
                maxUploadSpeedLimit = "32";
                startTime = "08:00:00";
                endTime = "17:00:00";
            } else {
                if (currentValue.contains("fullspeed;")) {
                    currentSelect = 1;
                }

                QStringList currentValueList = currentValue.split(';');

                if (currentValueList.count() > 4) {
                    maxDownloadSpeedLimit = currentValueList.at(1);
                    maxUploadSpeedLimit = currentValueList.at(2);
                    startTime = currentValueList.at(3);
                    endTime = currentValueList.at(4);
                }
            }

            downloadSettingWidget->setCurrentSelectRadioButton(currentSelect);
            downloadSettingWidget->setMaxDownloadSpeedLimit(maxDownloadSpeedLimit);
            downloadSettingWidget->setMaxUploadSpeedLimit(maxUploadSpeedLimit);
            downloadSettingWidget->setStartTime(startTime);
            downloadSettingWidget->setEndTime(endTime);
            //   option->blockSignals(false);
        }
    });

    return downloadSettingWidget;
}

QWidget *Settings::createNotificationsSettiingHandle(QObject *obj)
{
    NotificationsSettiingWidget *pWidget = new NotificationsSettiingWidget;
    return pWidget;
}

QWidget *Settings::createAutoDownloadBySpeedHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QString speed = "";
    bool check = false;

    if (option->value().toString().isEmpty()) {
        speed = "100";
        check = false;
    } else {
        speed = option->value().toString().mid(2);
        check = option->value().toString().left(1).toInt();
    }
    SettingsControlWidget *pWidget = new SettingsControlWidget();
    pWidget->initUI(tr("When total speed is lower than"), tr("KB/S add active downloads"));
    pWidget->setSpeend(speed);
    pWidget->setSwitch(check);

    connect(pWidget, &SettingsControlWidget::TextChanged, pWidget, [=](QString text) {
        option->setValue("1:" + text);
    });
    connect(pWidget, &SettingsControlWidget::checkedChanged, pWidget, [=](bool stat) {
        if (stat) {
            option->setValue("1:" + option->value().toString().mid(2));
        } else {
            option->setValue("0:" + option->value().toString().mid(2));
        }
    });

    connect(option, &DSettingsOption::valueChanged, pWidget, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            QString text = option->value().toString();
            pWidget->setSpeend(text.mid(2));
            pWidget->setSwitch(text.left(1).toInt());
        }
    });

    return pWidget;
}

QWidget *Settings::createPriorityDownloadBySizeHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QString size = "";
    bool check = false;

    if (option->value().toString().isEmpty()) {
        size = "30";
        check = false;
    } else {
        size = option->value().toString().mid(2);
        check = option->value().toString().left(1).toInt();
    }
    SettingsControlWidget *pWidget = new SettingsControlWidget();
    pWidget->initUI(tr("Priority to download less than"), tr("MB Task"), false);
    pWidget->setSize(size);
    pWidget->setSwitch(check);

    connect(pWidget, &SettingsControlWidget::TextChanged, pWidget, [=](QString text) {
        option->setValue("1:" + text);
    });

    connect(pWidget, &SettingsControlWidget::checkedChanged, pWidget, [=](bool stat) {
        if (stat) {
            option->setValue("1:" + option->value().toString().mid(2));
        } else {
            option->setValue("0:" + option->value().toString().mid(2));
        }
    });

    connect(option, &DSettingsOption::valueChanged, pWidget, [=](QVariant var) {
        if (!var.toString().isEmpty()) {
            QString text = option->value().toString();
            pWidget->setSize(text.mid(2));
            pWidget->setSwitch(text.left(1).toInt());
        }
    });

    return pWidget;
}

QWidget *Settings::createDiskCacheSettiingLabelHandle(QObject *obj)
{
    Q_UNUSED(obj);
    //    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QString diskCacheInfo = tr("Larger disk cache will result in faster download speed \nand more resource consumption.");
    SettingsLabel *settingsLabel = new SettingsLabel;
    settingsLabel->setLabelText(diskCacheInfo);

    QFont font;
    font.setPointSize(10); // 设置字号
    settingsLabel->setLabelFont(font);

    DPalette palette;
    QColor fontColor("#6d7c88");
    palette.setColor(DPalette::Text, fontColor);
    settingsLabel->setLabelPalette(palette);

    return settingsLabel;
}

bool Settings::getPowerOnState()
{
    auto option = m_settings->option("Basic.Start.PowerOn");

    return option->value().toBool();
}

void Settings::setAutoStart(bool ret)
{
    auto option = m_settings->option("Basic.Start.PowerOn");

    option->setValue(ret);
}

bool Settings::getAutostartUnfinishedTaskState()
{
    auto option = m_settings->option("Basic.Start.AutoStartUnfinishedTask");

    return option->value().toBool();
}

int Settings::getDownloadDirectorySelected()
{
    auto option = m_settings->option("Basic.DownloadDirectory.downloadDirectoryFileChooser");

    QString currentValue = option->value().toString();

    if (currentValue.contains("auto;")) {
        return 1;
    } else {
        return 0;
    }
}

QString Settings::getDownloadSavePath()
{
    auto option = m_settings->option("Basic.DownloadDirectory.downloadDirectoryFileChooser");

    QString currentValue = option->value().toString();
    QString downloadPath;

    if (currentValue.contains("custom;")) {
        QStringList currentValueList = currentValue.split(';');

        if (currentValueList.count() > 1) {
            downloadPath = currentValueList.at(1);

            if (downloadPath.isEmpty()) {
                downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        } else {
            downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
        }
    } else if (currentValue.contains("auto;")) {
        downloadPath = getCustomFilePath();

        if (downloadPath.isEmpty()) {
            downloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
        }
    }

    return downloadPath;
}

bool Settings::getOneClickDownloadState()
{
    auto option = m_settings->option("Basic.OnekeyDownload.onekeydownload");

    return option->value().toBool();
}

int Settings::getCloseMainWindowSelected()
{
    auto option = m_settings->option("Basic.CloseMainWindow.closemainwindow");

    return option->value().toInt();
}

int Settings::getMaxDownloadTaskNumber()
{
    auto option = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");

    return option->value().toInt();
}

bool Settings::getDownloadFinishedOpenState()
{
    auto option = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoOpen");

    return option->value().toBool();
}

bool Settings::getAutoDeleteFileNoExistentTaskState()
{
    auto option = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoDelete");

    return option->value().toBool();
}

int Settings::getDownloadSettingSelected()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();

    if (currentValue.contains("speedlimit;")) {
        return 1;
    } else {
        return 0;
    }
}

QString Settings::getMaxDownloadSpeedLimit()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();
    QString maxDownloadSpeedLimit;
    QStringList currentValueList = currentValue.split(';');

    if (currentValueList.count() > 4) {
        maxDownloadSpeedLimit = currentValueList.at(1);

        if (maxDownloadSpeedLimit.isEmpty()) {
            maxDownloadSpeedLimit = "10240";
        }
    }

    return maxDownloadSpeedLimit;
}

QString Settings::getMaxUploadSpeedLimit()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();
    QString maxUploadSpeedLimit;
    QStringList currentValueList = currentValue.split(';');

    if (currentValueList.count() > 4) {
        maxUploadSpeedLimit = currentValueList.at(2);

        if (maxUploadSpeedLimit.isEmpty()) {
            maxUploadSpeedLimit = "32";
        }
    }

    return maxUploadSpeedLimit;
}

QString Settings::getSpeedLimitStartTime()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();
    QString startTime;
    QStringList currentValueList = currentValue.split(';');

    if (currentValueList.count() > 4) {
        startTime = currentValueList.at(3);

        if (startTime.isEmpty()) {
            startTime = "08:00:00";
        }
    }

    return startTime;
}

QString Settings::getSpeedLimitEndTime()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();
    QString endTime;
    QStringList currentValueList = currentValue.split(';');

    if (currentValueList.count() > 4) {
        endTime = currentValueList.at(4);

        if (endTime.isEmpty()) {
            endTime = "17:00:00";
        }
    }

    return endTime;
}

DownloadSettings Settings::getAllSpeedLimitInfo()
{
    auto option = m_settings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString currentValue = option->value().toString();
    DownloadSettings downloadSettings;
    QStringList currentValueList = currentValue.split(';');

    if (currentValueList.count() > 4) {
        if (currentValue.contains("speedlimit;")) {
            downloadSettings.m_type = "1";
        } else {
            downloadSettings.m_type = "0";
        }

        downloadSettings.m_maxDownload = currentValueList.at(1);
        downloadSettings.m_maxUpload = currentValueList.at(2);
        downloadSettings.m_startTime = currentValueList.at(3);
        downloadSettings.m_endTime = currentValueList.at(4);
    }

    return downloadSettings;
}

bool Settings::getClipBoardState()
{
    auto option = m_settings->option("Monitoring.MonitoringObject.ClipBoard");

    return option->value().toBool();
}

bool Settings::getWebBrowserState()
{
    auto option = m_settings->option("Monitoring.MonitoringObject.WebBrowser");

    return option->value().toBool();
}

bool Settings::getHttpDownloadState()
{
    auto option = m_settings->option("Monitoring.MonitoringDownloadType.HttpDownload");

    return option->value().toBool();
}

bool Settings::getBtDownloadState()
{
    auto option = m_settings->option("Monitoring.MonitoringDownloadType.BTDownload");

    return option->value().toBool();
}

bool Settings::getMagneticDownloadState()
{
    auto option = m_settings->option("Monitoring.MonitoringDownloadType.MagneticDownload");

    return option->value().toBool();
}

bool Settings::getAutoOpennewTaskWidgetState()
{
    auto option = m_settings->option("Monitoring.BTRelation.OpenDownloadPanel");

    return option->value().toBool();
}

bool Settings::getStartAssociatedBTFileState()
{
    auto option = m_settings->option("Monitoring.BTRelation.AssociateBTFileAtStartup");

    return option->value().toBool();
}

bool Settings::getDownloadInfoSystemNotifyState()
{
    auto option = m_settings->option("Notifications.remind.downloadInfoNotify");

    return option->value().toBool();
}

bool Settings::getNewTaskShowMainWindowState()
{
    auto option = m_settings->option("AdvancedSetting.ShortcutKeySetting.NewTaskShowMainwindow");

    return option->value().toBool();
}

int Settings::getDisckcacheNum()
{
    auto option = m_settings->option("AdvancedSetting.DownloadDiskCache.DownloadDiskCacheSettiing");
    int number = 128;

    switch (option->value().toInt()) {
    case 0:
        number = 128;
        break;
    case 1:
        number = 256;
        break;
    case 2:
        number = 512;
        break;
    default:
        break;
    }

    return number;
}

bool Settings::getAutoDownloadBySpeed(QString &speed)
{
    QString text = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoDownload")->value().toString();
    speed = text.mid(2);
    bool isCheck = text.left(1).toInt();
    return isCheck;
}

bool Settings::getPriorityDownloadBySize(QString &size)
{
    QString text = m_settings->option("DownloadTaskManagement.downloadtaskmanagement.PriorityDownload")->value().toString();
    size = text.mid(2);
    bool isCheck = text.left(1).toInt();
    return isCheck;
}

bool Settings::getAutoSortBySpeed()
{
    return m_settings->option("DownloadTaskManagement.downloadtaskmanagement.AutoSortBySpeed")->value().toBool();
}

void Settings::setCloseMainWindowSelected(int select)
{
    auto option = m_settings->option("Basic.CloseMainWindow.closemainwindow");
    option->setValue(select);
}

QString Settings::getCustomFilePath()
{
    QString path = m_iniFile->value("FilePath/Filename").toString();
    if (path.remove(' ').isEmpty()) {
        return "";
    }
    return path;
}

void Settings::setCustomFilePath(const QString &path)
{
    m_iniFile->setValue("FilePath/Filename", path);
}

bool Settings::getIsShowTip()
{
    return m_iniFile->value("Close/showTip").toBool();
}

void Settings::setIsShowTip(bool b)
{
    if (b) {
        m_iniFile->setValue("Close/showTip", "true");
    } else {
        m_iniFile->setValue("Close/showTip", "false");
    }
    m_iniFile->sync();
}

bool Settings::getIsClipboradStart(QString str)
{
    QString clipboradValue = m_iniFile->value("Clipborad/data").toString();
    if (clipboradValue != str) {
        setIsClipboradStart(str);
    }
    return (str == clipboradValue) ? false : true;
}

void Settings::setIsClipboradStart(QString str)
{
    m_iniFile->setValue("Clipborad/data", str);
    m_iniFile->sync();
}
