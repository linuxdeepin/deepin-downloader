/**
* @file settings.cpp
* @brief 设置类，主要实现调用json文件，生成配置文件；创建自定义的控件窗口
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 13:39
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#include "settings.h"

#include "dthememanager.h"
#include <DSettings>
#include <DSettingsGroup>
#include <DSettingsWidgetFactory>
#include <DSettingsOption>
#include <dfilechooseredit.h>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "filesavepathchooser.h"
#include "settinginfoinputwidget.h"
#include "itemselectionwidget.h"
#include "groupselectionwidget.h"
#include "downloadsettingwidget.h"

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
    keys << "1" << "2" << "3" << "4" << "5";
    values << "1个" << "2个" << "3个" << "4个" << "5个";
    QMap<QString, QVariant> mapData;

    mapData.insert("keys", keys);
    mapData.insert("values", values);
    maxDownloadTaskOption->setData("items", mapData);

    if (maxDownloadTaskOption->value().toString().isEmpty())
    {
        maxDownloadTaskOption->setValue("5");
    }

    // 开机启动
    auto poweron_switchbutton = m_pSettings->option("Basic.Start.PowerOn");
    connect(poweron_switchbutton, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {


            }

        });
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

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pFileSavePathChooser);

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
            if(strCurrentValue.contains("custom;"))
            {
                pFileSavePathChooser->setCurrentSelectRadioButton(2);
            }
            QString strCurrentPath = strCurrentValue.section(QString(';'),1,1);
            if(strCurrentPath.isEmpty())
            {
                strCurrentPath =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
            pFileSavePathChooser->setLineEditText(strCurrentPath);
        }
    });

    return optionWidget;
}

QWidget *Settings::createHttpDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText("HTTP下载");
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pItemSelectionWidget);

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return optionWidget;
}

QWidget *Settings::createBTDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText("BT下载");
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pItemSelectionWidget);

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return optionWidget;
}

QWidget *Settings::createMagneticDownloadEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    ItemSelectionWidget *pItemSelectionWidget = new ItemSelectionWidget();
    pItemSelectionWidget->setLabelText("磁力链接下载");
    pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pItemSelectionWidget);

    connect(pItemSelectionWidget, &ItemSelectionWidget::checkBoxIsChecked, pItemSelectionWidget, [=] (QVariant var) {
        option->setValue(var.toString());
    });

    connect(option, &DSettingsOption::valueChanged, pItemSelectionWidget, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            pItemSelectionWidget->setCheckBoxChecked(option->value().toBool());
        }
    });

    return optionWidget;
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

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pGroupSelectionWidget);

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

    return optionWidget;
}

QWidget *Settings::createDownloadDiskCacheSettiingHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QStringList lstItemName;
    lstItemName << "128"  << "256"  << "512";

    QString strCurrentSelected = option->value().toString();
    if(strCurrentSelected.isEmpty())
    {
        if(lstItemName.count() > 0)
        {
            strCurrentSelected = lstItemName.at(0);
        }
    }

    GroupSelectionWidget *pGroupSelectionWidget = new GroupSelectionWidget(lstItemName);
    pGroupSelectionWidget->setLabelText("磁盘缓存越大，下载速度越快，占用电脑资源越多");
    pGroupSelectionWidget->setCurrentSelected(strCurrentSelected);

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pGroupSelectionWidget);

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

    return optionWidget;
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

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, pDownloadSettingWidget);

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

    return optionWidget;
}








