/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settings.h"
#include "dthememanager.h"
#include "savepathfilechooser.h"
#include "settingslabel.h"
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


/***
  *判断一个字符串是否为纯数字
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
Settings::Settings(QWidget *parent)
    : QObject(parent)
{
    m_configPath = QString("%1/%2/%3/config.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    m_backend = new QSettingBackend(m_configPath);

    settings = DSettings::fromJsonFile(":/resources/json/settings.json");
    settings->setBackend(m_backend);
    auto downloadlimitSpeed = settings->option("downloadsettings.downloadMode.maximumDownloadSpeedLimit");

    connect(downloadlimitSpeed, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(isDigitStr(value.toString())==0)
                {
                    if(value.toInt() >=100 && value.toInt() <= 102400)
                    {
                         emit get_download_limitspeed_changed(value.toString());
                    }
                }

            }

            downloadlimitSpeed->setValue(value);

        });

    auto maximumUploadSpeedLimit = settings->option("downloadsettings.downloadMode.maximumUploadSpeedLimit");

    connect(maximumUploadSpeedLimit, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                if(isDigitStr(value.toString())==0)
                {

                   emit get_upload_limitspeed_changed(value.toString());

                }
                else {
                    emit get_upload_limitspeed_changed("32");
                }
            }

        });

    //下载磁盘缓存值更变
    auto disckcacheNum = settings->option("AdvancedSetting.downloadDiskCache.disckcacheNum");
    connect(disckcacheNum, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                QString cacheNum ;
                switch (value.toInt()) {
                    case 0:
                        cacheNum = "128";
                        break;
                    case 1:
                        cacheNum = "256";
                        break;
                    case 2:
                        cacheNum = "512";
                        break;
                    default:
                        cacheNum = "256";
                        break;
                }

                emit get_disckCacheNum_changed(cacheNum);

            }

        });


    //限制时间变化
    auto speedLimitPeriod = settings->option("downloadsettings.downloadMode.speedLimitPeriod");
    connect(speedLimitPeriod, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit get_speedLimit_period_changed(value.toString());

            }

        });

    //最大下载数变化
    auto maxDownloadNum = settings->option("downloadsettings.downloadMode.maxDownloadNum");
    connect(maxDownloadNum, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit get_max_downloadnum_changed(value.toString());

            }

        });

    // 开机启动
    auto poweron_switchbutton = settings->option("basic.select_multiple.poweron_switchbutton");
    connect(poweron_switchbutton, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
            if(!value.isNull())
            {
                emit get_poweron_changed(value.toInt());

            }

        });







}


Settings::~Settings()
{
}

QWidget * Settings::createSettingsLabelHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    QString diskCacheInfo = tr("The larger the disk cache, the faster the download speed \n and the more computer resources are consumed.");
    SettingsLabel * diskCacheLabel = new SettingsLabel(diskCacheInfo);

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, diskCacheLabel);

     return optionWidget;
}
//设置默认下载路径
QWidget *Settings::createFileChooserEditHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    int iCurrentSelect = 2;
    QString file_path ;
    if (!option->value().toString().isEmpty())
    {
       QString current_value = option->value().toString();
       if(current_value.contains("auto;"))
       {
           iCurrentSelect = 1 ;
           file_path = current_value.section(QString(';'),1,1);
           if(file_path.isEmpty())
           {
               file_path =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
           }
       }
       else {
           file_path = current_value.section(QString(';'),1,1);
           if(file_path.isEmpty())
           {
               file_path =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
           }
       }
    }
    else
    {
        file_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    }

    SavePathFileChooser * savePathFileChooser = new SavePathFileChooser(iCurrentSelect,file_path);

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, savePathFileChooser);

    connect(savePathFileChooser, &SavePathFileChooser::textChanged, savePathFileChooser, [=] (QVariant var) {

        QString current_value = var.toString();
        QString option_value = option->value().toString();
        if(current_value=="custom;" &&!option_value.isEmpty())
        {
            QString old_file_path = option_value.section(QString(';'),1,1);
            if(!old_file_path.isEmpty())
            {
                current_value = "custom;" + old_file_path;
                option->setValue(current_value);
            }
        }
        else {
             option->setValue(var.toString());
        }


    });

    connect(option, &DSettingsOption::valueChanged, savePathFileChooser, [=] (QVariant var) {
        if(!var.toString().isEmpty())
        {
            QString current_value = var.toString();
            if(current_value.contains("custom;"))
            {
                savePathFileChooser->setSelectRadio("custom");
            }
            QString current_file_path = current_value.section(QString(';'),1,1);
            if(current_file_path.isEmpty())
            {
                current_file_path =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
            savePathFileChooser->setSelectPath(current_file_path);



        }



    });


    return optionWidget;
}

//设置最大下载文件数
QWidget *Settings::createDescribeMaxDownloadNumInputBoxHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    DescribeInputBox *describeInputBox;
    if (option->value().toString().isEmpty())
    {
        describeInputBox = new DescribeInputBox(tr("Max download num"),tr("individual"),tr("(1-20)"));
    }
    else
    {
       describeInputBox = new DescribeInputBox(tr("Max download num"),tr("individual"),tr("(1-20)"),option->value().toString());
    }
    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, describeInputBox);

    connect(describeInputBox,&DescribeInputBox::get_textChanged,option,[=] (const QString &text) {

        if(isDigitStr(text)==0)
        {
            if(text.toInt() >=1 && text.toInt() <= 20)
            {
                option->setValue(text);
            }

        }


    });
    connect(option, &DSettingsOption::valueChanged, describeInputBox, [=] (QVariant text) {

        if(isDigitStr(text.toString())==0)
        {
            if(text.toInt() >=1 && text.toInt() <= 20)
            {
                option->setValue(text);
                describeInputBox->setLineEditValue(text.toString());
            }
        }


        });
    int minValid = 1;
    int maxValid = 20;
    describeInputBox->setValid(minValid,maxValid);

    return optionWidget;
}
QWidget *Settings::createDescribeInputBoxHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    DescribeInputBox *describeInputBox;

    if (option->value().toString().isEmpty())
    {
        describeInputBox = new DescribeInputBox(tr("Max download speed limit"),tr("KB/s"),tr("(100-102400)"));
       // describeInputBox = new DescribeInputBox(tr(" "),tr("KB/s"),tr("(100-102400)"));
    }
    else
    {
        describeInputBox = new DescribeInputBox(tr("Max download speed limit"),tr("KB/s"),tr("(100-102400)"),option->value().toString());
        //describeInputBox = new DescribeInputBox(tr(""),tr("KB/s"),tr("(100-102400)"),option->value().toString());
    }

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, describeInputBox);

    connect(describeInputBox,&DescribeInputBox::get_textChanged,option,[=] (const QString &text) {


        if(isDigitStr(text)==0)
        {
            if(text.toInt() >=100 && text.toInt() <= 102400)
            {
                option->setValue(text);
            }

        }

        });

    connect(option, &DSettingsOption::valueChanged, describeInputBox, [=] (QVariant var) {
            describeInputBox->setLineEditValue(var.toString());
        });
    int minValid = 100;
    int maxValid = 102400;
    describeInputBox->setValid(minValid,maxValid);

    return optionWidget;

}

//创建描述加输入框加描述控件
QWidget *Settings::createdescribeUploadInputBoxHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    DescribeInputBox *describeInputBox;
    if (option->value().toString().isEmpty())
    {
        describeInputBox = new DescribeInputBox(tr("Max upload speed limit"),tr("KB/s"),tr("(16-5120)"));
    }
    else
    {
       describeInputBox = new DescribeInputBox(tr("Max upload speed limit"),tr("KB/s"),tr("(16-5120)"),option->value().toString());
    }
    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, describeInputBox);

    connect(describeInputBox,&DescribeInputBox::get_textChanged,option,[=] (const QString &text) {

        if(isDigitStr(text)==0)
        {
            if(text.toInt() >=16 && text.toInt() <= 5120)
            {
                option->setValue(text);
            }

        }


    });
    connect(option, &DSettingsOption::valueChanged, describeInputBox, [=] (QVariant text) {

        if(isDigitStr(text.toString())==0)
        {
            if(text.toInt() >=16 && text.toInt() <= 5120)
            {
                option->setValue(text);
                describeInputBox->setLineEditValue(text.toString());
            }
        }


        });
    int minValid = 16;
    int maxValid = 5120;
    describeInputBox->setValid(minValid,maxValid);

    return optionWidget;

}
//创建时间段控件
QWidget *Settings::createTimeSectionControlBoxHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);
    QTime *startTime , *endTime;
    QString  userInfo ;
    QString allTimeInfo ;
    startTime = new QTime;
    endTime = new QTime;
    if (option->value().toString().isEmpty())
    {
        startTime->setHMS(8,0,0);
        endTime->setHMS(17,30,0);
        userInfo = "8,0,0;17:30:0";
    }
    else {
        userInfo = option->value().toString();
        QString userInfoStartTime = userInfo.section(QString(';'),0,0);
        QString userInfoEndTime = userInfo.section(QString(';'),1,1);
        startTime->setHMS(userInfoStartTime.section(QString(','),0,0).toInt(),
                          userInfoStartTime.section(QString(','),1,1).toInt(),
                          userInfoStartTime.section(QString(','),2,2).toInt());

        endTime->setHMS(userInfoEndTime.section(QString(','),0,0).toInt(),
                          userInfoEndTime.section(QString(','),1,1).toInt(),
                          userInfoEndTime.section(QString(','),2,2).toInt());

    }
    TimeSectionControl *timeSectionControl = new TimeSectionControl(tr("Speed limit period"),tr("To"),startTime,endTime);

    QWidget *optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, timeSectionControl);

    connect(timeSectionControl,&TimeSectionControl::get_startTimeChanged,option,[=] (const QTime &setTime) {


    startTime->setHMS(setTime.hour(),setTime.minute(),setTime.second()) ;

    option->setValue(QString::number(startTime->hour()) +"," +
                QString::number(startTime->minute())+ "," +
                QString::number(startTime->second()) +";" +
                QString::number(endTime->hour()) + "," +
                QString::number(endTime->minute())+ ","+
                QString::number(endTime->second()));


    });

    connect(timeSectionControl,&TimeSectionControl::get_endTimeChanged,option,[=] (const QTime &setTime) {
       endTime->setHMS(setTime.hour(),setTime.minute(),setTime.second());
       option->setValue(QString::number(startTime->hour()) +"," +
                   QString::number(startTime->minute())+ "," +
                   QString::number(startTime->second()) +";" +
                   QString::number(endTime->hour()) + "," +
                   QString::number(endTime->minute())+ ","+
                   QString::number(endTime->second()));

    });

    connect(option, &DSettingsOption::valueChanged, timeSectionControl, [=] (QVariant var) {
            if(var.toString()!="")
            {
                QString timeInfo = var.toString();
                QString userInfoStartTime = timeInfo.section(QString(';'),0,0);
                QString userInfoEndTime = timeInfo.section(QString(';'),1,1);
                startTime->setHMS(userInfoStartTime.section(QString(','),0,0).toInt(),
                                  userInfoStartTime.section(QString(','),1,1).toInt(),
                                  userInfoStartTime.section(QString(','),2,2).toInt());

                endTime->setHMS(userInfoEndTime.section(QString(','),0,0).toInt(),
                                  userInfoEndTime.section(QString(','),1,1).toInt(),
                                  userInfoEndTime.section(QString(','),2,2).toInt());

                timeSectionControl->setTimeZone(startTime,endTime);
            }

        });

    return optionWidget;
}

