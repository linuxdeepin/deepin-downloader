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
#include "filesavepathchooser.h"
#include "settinginfoinputwidget.h"
#include "itemselectionwidget.h"
#include "groupselectionwidget.h"
#include "downloadsettingwidget.h"

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

/**
 * @brief 判断字符串是否为纯数字
 * @param src 字符串
 * @return 返回0表示是纯数字，否则返回-1
 */
int isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return -1;
    }
    else
    { //纯数字
        return 0;
    }
}

Settings *Settings::s_pInstance = nullptr;
Settings *Settings::getInstance()
{
    if(s_pInstance == nullptr)
    {
        s_pInstance = new Settings;
    }

    return s_pInstance;
}

Settings::Settings(QObject *parent) : QObject(parent)
{
    m_configPath = QString("%1/%2/%3/config.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    m_pBackend = new QSettingBackend(m_configPath);

    m_pSettings = DSettings::fromJsonFile(":/json/settings");
    m_pSettings->setBackend( m_pBackend );

    // 初始化同时下载最大任务数
    auto maxDownloadTaskOption = m_pSettings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");
    QStringList values;
    QStringList keys;
    keys << "3" << "5" << "10" << "20";
    values << "3" << "5" << "10" << "20";
    QMap<QString, QVariant> mapData;

    mapData.insert("keys", keys);
    mapData.insert("values", values);
    maxDownloadTaskOption->setData("items", mapData);

    if (maxDownloadTaskOption->value().toString().isEmpty())
    {
        maxDownloadTaskOption->setValue("5");
    }

    // 最大下载任务数
    auto maxDownloadTaskNumber = m_pSettings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");
    connect(maxDownloadTaskNumber, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit maxDownloadTaskNumberChanged(value.toInt());
            }
        });

    // 下载设置
    auto downloadSettingInfo = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");
    connect(downloadSettingInfo, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                QString strCurrentValue = value.toString();
                S_DownloadSettings downloadSettings;
                QStringList lstCurrentValue = strCurrentValue.split(';');
                if(lstCurrentValue.count() > 4)
                {
                    if(strCurrentValue.contains("speedlimit;"))
                    {
                        downloadSettings.m_strType = "1";
                    }
                    else
                    {
                        downloadSettings.m_strType = "0";
                    }
                    downloadSettings.m_strMaxDownload = lstCurrentValue.at(1);
                    downloadSettings.m_strMaxUpload = lstCurrentValue.at(2);
                    downloadSettings.m_strStartTime = lstCurrentValue.at(3);
                    downloadSettings.m_strEndTime = lstCurrentValue.at(4);
                }
                emit downloadSettingsChanged();
            }
    });

    // 下载磁盘缓存
    auto diskCacheNum = m_pSettings->option("AdvancedSetting.DownloadDiskCache.DownloadDiskCacheSettiing");
    connect(diskCacheNum, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit disckCacheChanged(value.toInt());
            }
        });

    // 开机启动
    auto poweronSwitchbutton = m_pSettings->option("Basic.Start.PowerOn");
    connect(poweronSwitchbutton, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit poweronChanged(value.toBool());
            }
        });

    // 启动时关联BT种子文件
    auto startAssociatedBTFile = m_pSettings->option("Monitoring.BTRelation.AssociateBTFileAtStartup");
    connect(startAssociatedBTFile, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if (!value.isNull()) {
                emit startAssociatedBTFileChanged(value.toBool());
            }
        });

    // 实现剪切板和接管下载类型关联
    auto optionClipBoard = m_pSettings->option("Monitoring.MonitoringObject.ClipBoard");
    auto optionHttpDownload = m_pSettings->option("Monitoring.MonitoringDownloadType.HttpDownload");
    auto optionBTDownload = m_pSettings->option("Monitoring.MonitoringDownloadType.BTDownload");
    auto optionMagneticDownload = m_pSettings->option("Monitoring.MonitoringDownloadType.MagneticDownload");

    // 剪切板状态改变
    connect(optionClipBoard, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(value.toBool() == true)
                {
                    if(optionHttpDownload->value().toBool() == false
                            && optionBTDownload->value().toBool() == false
                            && optionMagneticDownload->value().toBool() == false)
                    {
                        optionHttpDownload->setValue(true);
                        optionBTDownload->setValue(true);
                        optionMagneticDownload->setValue(true);
                    }
                }
                else
                {
                    optionHttpDownload->setValue(false);
                    optionBTDownload->setValue(false);
                    optionMagneticDownload->setValue(false);
                }
            }
        });

    // Http下载状态改变
    connect(optionHttpDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(value.toBool() == true)
                {
                    if(optionClipBoard->value().toBool() == false)
                    {
                        optionClipBoard->setValue(true);
                    }
                }
                else
                {
                    if(optionBTDownload->value().toBool() == false && optionMagneticDownload->value().toBool() == false)
                    {
                        optionClipBoard->setValue(false);
                    }
                }
            }
        });

    // BT下载状态改变
    connect(optionBTDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(value.toBool() == true)
                {
                    if(optionClipBoard->value().toBool() == false)
                    {
                        optionClipBoard->setValue(true);
                    }
                }
                else
                {
                    if(optionHttpDownload->value().toBool() == false && optionMagneticDownload->value().toBool() == false)
                    {
                        optionClipBoard->setValue(false);
                    }
                }
            }
        });

    // 磁力链接下载状态改变
    connect(optionMagneticDownload, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(value.toBool() == true)
                {
                    if(optionClipBoard->value().toBool() == false)
                    {
                        optionClipBoard->setValue(true);
                    }
                }
                else
                {
                    if(optionHttpDownload->value().toBool() == false && optionBTDownload->value().toBool() == false)
                    {
                        optionClipBoard->setValue(false);
                    }
                }
            }
        });

    // json文件国际化
    auto group_basicName = tr("Basic"); // 基本设置
    auto group_basic_startName = tr("Start up"); // 启动
    auto basic_start_poweronName = tr("Auto start after boot"); // 开机启动
    auto basic_start_autoStartUnfinishedTaskName = tr("Auto download after start-up"); // 启动后自动开始未完成的任务
    auto group_basic_downloadDirectoryName = tr("File Storage"); // 下载目录
    auto group_basic_oneClickDownloadName = tr("1-click download"); // 一键下载
    auto group_basic_closeMainWindowName = tr("Close window"); // 关闭主界面
    auto basic_closeMainWindow_minimizeToSystemTrayName = tr("Minimize to System Tray"); // 最小化到托盘
    auto basic_closeMainWindow_exitName = tr("Exit"); // 退出下载器
    auto group_tasksName = tr("Tasks"); // 任务管理
    auto tasks_activeDownloadsName = tr("Active downloads"); // 同时下载最大任务数
    auto tasks_openFileAfterDownloadName = tr("Open file after download"); // 下载完成后自动打开
    auto tasks_deleteTasksWithoutFilesName = tr("Delete tasks without files"); // 自动删除“文件不存在”的任务
    auto group_downloadsName = tr("Downloads"); // 下载设置
    auto group_monitoringName = tr("Monitoring"); // 接管设置
    auto group_monitoring_monitoringApplicationName = tr("Application"); // 接管对象
    auto monitoring_monitoringApplication_clipBoardName = tr("Clipboard"); // 剪切板
    auto group_monitoring_monitoringDownloadLinkName = tr("Link type"); // 接管下载类型
    auto group_monitoring_monitoringBTFilesName = tr("BT files"); // BT关联
    auto monitoring_monitoringBTFiles_downloadingName = tr("Create new task after downloading BT files"); // 下载种子文件后自动打开下载面板
    auto monitoring_monitoringBTFiles_openingName = tr("Create new task when BT files opening"); // 启动时关联BT种子文件
    auto group_notificationsName = tr("Notifications"); // 通知提醒
    auto notifications_succeedOrFailedName = tr("Allow notifications when downloading succeed or failed"); // 下载完成/失败时，系统通知提醒
    auto notifications_downloadingSucceedName = tr("Allow sounds when downloading succeed"); // 下载完成后，播放提示音
    auto group_advancedName = tr("Advanced"); // 高级设置
    auto group_advanced_shortcutsName = tr("Shortcuts"); // 快捷设置
    auto advanced_shortcuts_newTaskName = tr("Show main window when creating new task"); // 新建任务时显示主界面
    auto advanced_shortcuts_showMainName = tr("Show main window"); // 打开主界面快捷键
    auto group_advanced_diskCacheName = tr("Cache"); // 下载磁盘缓存

    //上次保存文件位置以及右上角关闭时是否显示提示框

    QString iniConfigPath = QString("%1/%2/%3/usrConfig.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());
    m_pIniFile = new QSettings(iniConfigPath, QSettings::IniFormat);
    if(!m_pIniFile->contains("FilePath/Filename")){
        m_pIniFile->setValue("FilePath/Filename",  "/home/sanhei/Downloads");
    }
    if(!m_pIniFile->contains("Close/showTip")){
        m_pIniFile->setValue("Close/showTip",  "true");
    }
}

QWidget *Settings::createFileChooserEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    int nCurrentSelect = 2;
    QString strDownloadPath;

    if(option->value().toString().isEmpty())
    {
        strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    }
    else
    {
        QString strCurrentValue = option->value().toString();
        if(strCurrentValue.contains("auto;"))
        {
            nCurrentSelect = 1;
            QStringList lstCurrentValue = strCurrentValue.split(';');
            if(lstCurrentValue.count() > 1)
            {
                strDownloadPath = lstCurrentValue.at(1);
                if(strDownloadPath.isEmpty())
                {
                    strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
            }
        }
        else
        {
            QStringList lstCurrentValue = strCurrentValue.split(';');
            if(lstCurrentValue.count() > 1)
            {
                strDownloadPath = lstCurrentValue.at(1);
                if(strDownloadPath.isEmpty())
                {
                    strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
            }
        }
    }

    FileSavePathChooser *pFileSavePathChooser = new FileSavePathChooser(nCurrentSelect, strDownloadPath);

    connect(pFileSavePathChooser, &FileSavePathChooser::textChanged, pFileSavePathChooser, [=] (QVariant var) {

        QString strCurrentValue = var.toString();
        QString strOptionValue = option->value().toString();

        if(strCurrentValue == "custom;" && !strOptionValue.isEmpty())
        {
            QString strOldPath = strOptionValue.section(QString(';'),1,1);
            if(!strOldPath.isEmpty())
            {
                strCurrentValue = "custom;" + strOldPath;
                option->setValue(strCurrentValue);
            }
        }
        else
        {
            option->setValue(var.toString());
        }
    });

    connect(option, &DSettingsOption::valueChanged, pFileSavePathChooser, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            QString strCurrentValue = var.toString();
            QString strCurrentPath = strCurrentValue.section(QString(';'),1,1);

            if(strCurrentPath.isEmpty())
            {
                strCurrentPath =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }

            pFileSavePathChooser->setLineEditText(strCurrentPath);

            if(strCurrentValue.contains("custom;"))
            {
                pFileSavePathChooser->setCurrentSelectRadioButton(2);
            }
            else {
                pFileSavePathChooser->setCurrentSelectRadioButton(1);
            }
        }
    });

    return pFileSavePathChooser;
}

QWidget *Settings::createHttpDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText(tr("HTTP")); // HTTP下载
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return pItemSelectionWidget;
}

QWidget *Settings::createBTDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText(tr("BitTorrent")); // BT下载
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return pItemSelectionWidget;
}

QWidget *Settings::createMagneticDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText(tr("Magnet URI scheme")); // 磁力链接下载
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return pItemSelectionWidget;
}

QWidget *Settings::createDownloadTraySettingHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QStringList lstItemName;
    lstItemName << "显示"  << "下载时显示"  << "隐藏";

    QString strCurrentSelected = option->value().toString();
    if(strCurrentSelected.isEmpty())
    {
        if(lstItemName.count() > 0)
        {
            strCurrentSelected = lstItemName.at(0);
        }
    }

    GroupSelectionWidget *pGroupSelectionWidget = new GroupSelectionWidget(lstItemName);
    pGroupSelectionWidget->setLabelIsHide(true);
    pGroupSelectionWidget->setCurrentSelected(strCurrentSelected);

    connect(pGroupSelectionWidget, &GroupSelectionWidget::selectedChanged, pGroupSelectionWidget, [=] (QVariant var) {
        QString strCurrentValue = var.toString();
        if(strCurrentValue.isEmpty())
        {
            if(lstItemName.count() > 0)
            {
                strCurrentValue = lstItemName.at(0);
            }
        }
        option->setValue(strCurrentValue);
    });

    connect(option, &DSettingsOption::valueChanged, pGroupSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            QString strCurrentSelected = option->value().toString();
            if(strCurrentSelected.isEmpty())
            {
                if(lstItemName.count() > 0)
                {
                    strCurrentSelected = lstItemName.at(0);
                }
            }
            pGroupSelectionWidget->setCurrentSelected(strCurrentSelected);
        }
    });

    return pGroupSelectionWidget;
}

QWidget *Settings::createDownloadDiskCacheSettiingHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QStringList lstItemName;
    lstItemName << tr("128")  << tr("256")  << tr("512");

    QString strCurrentSelected = option->value().toString();
    if(strCurrentSelected.isEmpty())
    {
        if(lstItemName.count() > 0)
        {
            strCurrentSelected = lstItemName.at(0);
        }
    }

    GroupSelectionWidget *pGroupSelectionWidget = new GroupSelectionWidget(lstItemName);
    pGroupSelectionWidget->setLabelText(tr("More disk cache, faster download speed and more computer consume")); // 磁盘缓存越大，下载速度越快，占用电脑资源越多
    pGroupSelectionWidget->setCurrentSelected(strCurrentSelected);

    connect(pGroupSelectionWidget, &GroupSelectionWidget::selectedChanged, pGroupSelectionWidget, [=] (QVariant var) {
        QString strCurrentValue = var.toString();
        if(strCurrentValue.isEmpty())
        {
            if(lstItemName.count() > 0)
            {
                strCurrentValue = lstItemName.at(0);
            }
        }
        option->setValue(strCurrentValue);
    });

    connect(option, &DSettingsOption::valueChanged, pGroupSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            QString strCurrentSelected = option->value().toString();
            if(strCurrentSelected.isEmpty())
            {
                if(lstItemName.count() > 0)
                {
                    strCurrentSelected = lstItemName.at(0);
                }
            }
            pGroupSelectionWidget->setCurrentSelected(strCurrentSelected);
        }
    });

    return pGroupSelectionWidget;
}

QWidget *Settings::createDownloadSpeedLimitSettiingHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    int nCurrentSelect = 2;
    QString strMaxDownloadSpeedLimit;
    QString strMaxUploadSpeedLimit;
    QString strStartTime;
    QString strEndTime;

    if(option->value().toString().isEmpty())
    {
        strMaxDownloadSpeedLimit = "10240";
        strMaxUploadSpeedLimit = "32";
        strStartTime = "08:00:00";
        strEndTime = "17:00:00";
    }
    else
    {
        QString strCurrentValue = option->value().toString();
        if(strCurrentValue.contains("fullspeed;"))
        {
            nCurrentSelect = 1;
        }
        QStringList lstCurrentValue = strCurrentValue.split(';');
        if(lstCurrentValue.count() > 4)
        {
            strMaxDownloadSpeedLimit = lstCurrentValue.at(1);
            strMaxUploadSpeedLimit = lstCurrentValue.at(2);
            strStartTime = lstCurrentValue.at(3);
            strEndTime = lstCurrentValue.at(4);
        }
    }

    DownloadSettingWidget *pDownloadSettingWidget = new DownloadSettingWidget();
    pDownloadSettingWidget->setCurrentSelectRadioButton(nCurrentSelect);
    pDownloadSettingWidget->setMaxDownloadSpeedLimit(strMaxDownloadSpeedLimit);
    pDownloadSettingWidget->setMaxUploadSpeedLimit(strMaxUploadSpeedLimit);
    pDownloadSettingWidget->setStartTime(strStartTime);
    pDownloadSettingWidget->setEndTime(strEndTime);

    connect(pDownloadSettingWidget, &DownloadSettingWidget::speedLimitInfoChanged, pDownloadSettingWidget, [=] (QVariant var) {
        QString strCurrentValue = var.toString();
        QStringList lstCurrentValue = strCurrentValue.split(';');
        if(lstCurrentValue.count() > 4)
        {
            QString strMaxDownloadSpeedLimit = lstCurrentValue.at(1);
            QString strMaxUploadSpeedLimit = lstCurrentValue.at(2);
            if(strMaxDownloadSpeedLimit.toInt() >= 100 && strMaxDownloadSpeedLimit.toInt() <= 102400
                    && strMaxUploadSpeedLimit.toInt() >= 16 && strMaxUploadSpeedLimit.toInt() <= 5120)
            {
                option->setValue(strCurrentValue);
            }
        }
    });

    connect(option, &DSettingsOption::valueChanged, pDownloadSettingWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            QString strCurrentValue = option->value().toString();
            int nCurrentSelect = 2;
            QString strMaxDownloadSpeedLimit;
            QString strMaxUploadSpeedLimit;
            QString strStartTime;
            QString strEndTime;

            if(strCurrentValue.isEmpty())
            {
                strMaxDownloadSpeedLimit = "10240";
                strMaxUploadSpeedLimit = "32";
                strStartTime = "08:00:00";
                strEndTime = "17:00:00";
            }
            else
            {
                if(strCurrentValue.contains("fullspeed;"))
                {
                    nCurrentSelect = 1;
                }

                QStringList lstCurrentValue = strCurrentValue.split(';');
                if(lstCurrentValue.count() > 4)
                {
                    strMaxDownloadSpeedLimit = lstCurrentValue.at(1);
                    strMaxUploadSpeedLimit = lstCurrentValue.at(2);
                    strStartTime = lstCurrentValue.at(3);
                    strEndTime = lstCurrentValue.at(4);
                }
            }

            pDownloadSettingWidget->setCurrentSelectRadioButton(nCurrentSelect);
            pDownloadSettingWidget->setMaxDownloadSpeedLimit(strMaxDownloadSpeedLimit);
            pDownloadSettingWidget->setMaxUploadSpeedLimit(strMaxUploadSpeedLimit);
            pDownloadSettingWidget->setStartTime(strStartTime);
            pDownloadSettingWidget->setEndTime(strEndTime);
        }
    });

    return pDownloadSettingWidget;
}

bool Settings::getPowerOnState()
{
    auto option = m_pSettings->option("Basic.Start.PowerOn");
    return option->value().toBool();
}

bool Settings::getAutostartUnfinishedTaskState()
{
    auto option = m_pSettings->option("Basic.Start.AutoStartUnfinishedTask");
    return option->value().toBool();
}

int Settings::getDownloadDirectorySelected()
{
    auto option = m_pSettings->option("Basic.DownloadDirectory.downloadDirectoryFileChooser");

    QString strCurrentValue = option->value().toString();
    if(strCurrentValue.contains("auto;"))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

QString Settings::getDownloadSavePath()
{
    auto option = m_pSettings->option("Basic.DownloadDirectory.downloadDirectoryFileChooser");

    QString strCurrentValue = option->value().toString();
    QString strDownloadPath;
    if(strCurrentValue.contains("custom;"))
    {
        QStringList lstCurrentValue = strCurrentValue.split(';');
        if(lstCurrentValue.count() > 1)
        {
            strDownloadPath = lstCurrentValue.at(1);
            if(strDownloadPath.isEmpty())
            {
                strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        }
        else
        {
            strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
        }
    }
    else if(strCurrentValue.contains("auto;"))
    {
        strDownloadPath = getCustomFilePath();
        if(strDownloadPath.isEmpty())
        {
            strDownloadPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
        }
    }

    return strDownloadPath;
}

bool Settings::getOneClickDownloadState()
{
    auto option = m_pSettings->option("Basic.OnekeyDownload.onekeydownload");
    return option->value().toBool();
}

int Settings::getCloseMainWindowSelected()
{
    auto option = m_pSettings->option("Basic.CloseMainWindow.closemainwindow");
    return option->value().toInt();
}

int Settings::getMaxDownloadTaskNumber()
{
    auto option = m_pSettings->option("DownloadTaskManagement.downloadtaskmanagement.MaxDownloadTask");
    return option->value().toInt();
}

bool Settings::getDownloadFinishedOpenState()
{
    auto option = m_pSettings->option("DownloadTaskManagement.downloadtaskmanagement.AutoOpen");
    return option->value().toBool();
}

bool Settings::getAutoDeleteFileNoExistentTaskState()
{
    auto option = m_pSettings->option("DownloadTaskManagement.downloadtaskmanagement.AutoDelete");
    return option->value().toBool();
}

int Settings::getDownloadSettingSelected()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    if(strCurrentValue.contains("speedlimit;"))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

QString Settings::getMaxDownloadSpeedLimit()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    QString strMaxDownloadSpeedLimit;
    QStringList lstCurrentValue = strCurrentValue.split(';');
    if(lstCurrentValue.count() > 4)
    {
        strMaxDownloadSpeedLimit = lstCurrentValue.at(1);
        if(strMaxDownloadSpeedLimit.isEmpty())
        {
            strMaxDownloadSpeedLimit = "10240";
        }
    }

    return  strMaxDownloadSpeedLimit;
}

QString Settings::getMaxUploadSpeedLimit()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    QString strMaxUploadSpeedLimit;
    QStringList lstCurrentValue = strCurrentValue.split(';');
    if(lstCurrentValue.count() > 4)
    {
        strMaxUploadSpeedLimit = lstCurrentValue.at(2);
        if(strMaxUploadSpeedLimit.isEmpty())
        {
            strMaxUploadSpeedLimit = "32";
        }
    }

    return  strMaxUploadSpeedLimit;
}

QString Settings::getSpeedLimitStartTime()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    QString strStartTime;
    QStringList lstCurrentValue = strCurrentValue.split(';');
    if(lstCurrentValue.count() > 4)
    {
        strStartTime = lstCurrentValue.at(3);
        if(strStartTime.isEmpty())
        {
            strStartTime = "08:00:00";
        }
    }

    return  strStartTime;
}

QString Settings::getSpeedLimitEndTime()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    QString strEndTime;
    QStringList lstCurrentValue = strCurrentValue.split(';');
    if(lstCurrentValue.count() > 4)
    {
        strEndTime = lstCurrentValue.at(4);
        if(strEndTime.isEmpty())
        {
            strEndTime = "17:00:00";
        }
    }

    return  strEndTime;
}

S_DownloadSettings Settings::getAllSpeedLimitInfo()
{
    auto option = m_pSettings->option("DownloadSettings.downloadsettings.downloadspeedlimit");

    QString strCurrentValue = option->value().toString();
    S_DownloadSettings downloadSettings;
    QStringList lstCurrentValue = strCurrentValue.split(';');
    if(lstCurrentValue.count() > 4)
    {
        if(strCurrentValue.contains("speedlimit;"))
        {
            downloadSettings.m_strType = "1";
        }
        else
        {
            downloadSettings.m_strType = "0";
        }
        downloadSettings.m_strMaxDownload = lstCurrentValue.at(1);
        downloadSettings.m_strMaxUpload = lstCurrentValue.at(2);
        downloadSettings.m_strStartTime = lstCurrentValue.at(3);
        downloadSettings.m_strEndTime = lstCurrentValue.at(4);
    }

    return downloadSettings;
}

bool Settings::getClipBoardState()
{
    auto option = m_pSettings->option("Monitoring.MonitoringObject.ClipBoard");
    return option->value().toBool();
}

bool Settings::getHttpDownloadState()
{
    auto option = m_pSettings->option("Monitoring.MonitoringDownloadType.HttpDownload");
    return option->value().toBool();
}

bool Settings::getBtDownloadState()
{
    auto option = m_pSettings->option("Monitoring.MonitoringDownloadType.BTDownload");
    return option->value().toBool();
}

bool Settings::getMagneticDownloadState()
{
    auto option = m_pSettings->option("Monitoring.MonitoringDownloadType.MagneticDownload");
    return option->value().toBool();
}

bool Settings::getAutoOpenNewTaskWidgetState()
{
    auto option = m_pSettings->option("Monitoring.BTRelation.OpenDownloadPanel");
    return option->value().toBool();
}

bool Settings::getStartAssociatedBTFileState()
{
    auto option = m_pSettings->option("Monitoring.BTRelation.AssociateBTFileAtStartup");
    return option->value().toBool();
}

bool Settings::getDownloadInfoSystemNotifyState()
{
    auto option = m_pSettings->option("Notifications.remind.downloadInfoNotify");
    return option->value().toBool();
}

bool Settings::getDownloadFinishedPlayToneState()
{
    auto option = m_pSettings->option("Notifications.remind.afterDownloadPlayTone");
    return option->value().toBool();
}

bool Settings::getNewTaskShowMainWindowState()
{
    auto option = m_pSettings->option("AdvancedSetting.ShortcutKeySetting.NewTaskShowMainwindow");
    return option->value().toBool();
}

QString Settings::getOpenMainWindowShortcutKey()
{
    auto option = m_pSettings->option("AdvancedSetting.ShortcutKeySetting.OpenMainWindowShortcutKey");
    return option->value().toString();
}

int Settings::getDisckcacheNum()
{
    auto option = m_pSettings->option("AdvancedSetting.DownloadDiskCache.DownloadDiskCacheSettiing");
    return option->value().toInt();
}

void Settings::setCloseMainWindowSelected(int nSelect)
{
    auto option = m_pSettings->option("Basic.CloseMainWindow.closemainwindow");
    option->setValue(nSelect);
}


QString Settings::getCustomFilePath()
{
    return m_pIniFile->value("FilePath/Filename").toString();
}

void Settings::setCustomFilePath(const QString &path)
{
    m_pIniFile->setValue( "FilePath/Filename",  path);

}

bool Settings::getIsShowTip()
{
    return m_pIniFile->value("Close/showTip").toBool();
}

void Settings::setIsShowTip(bool b)
{
    if(b){
        m_pIniFile->setValue( "Close/showTip",  "true");
    } else {
        m_pIniFile->setValue( "Close/showTip",  "false");
    }
}







