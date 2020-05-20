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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dsettingsdialog.h"
#include "describeinputbox.h"
#include "timesectioncontrol.h"
#include "dfilechooseredit.h"
#include "messagebox.h"
#include <qsettingbackend.h>
#include <QSettings>
#include <QPointer>
#include <QTime>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class Settings : public QObject
{
    Q_OBJECT

public:
     Settings(QWidget *parent = 0);
     ~Settings();

    static QWidget *createFileChooserEditHandle(QObject *obj);
    static QWidget * createDescribeMaxDownloadNumInputBoxHandle(QObject *obj);
    static QWidget *createDescribeInputBoxHandle(QObject *obj);
    static QWidget *createdescribeUploadInputBoxHandle(QObject *obj);
    static QWidget *createTimeSectionControlBoxHandle(QObject *obj);
    static QWidget * createSettingsLabelHandle(QObject *obj);



    DSettings *settings;

signals:
    void get_download_limitspeed_changed(QString download_limitspeed);
    void get_upload_limitspeed_changed(QString upload_limitspeed);
    void get_disckCacheNum_changed(QString upload_limitspeed);
    void clear_lineEdit_signal();
    void get_speedLimit_period_changed(QString speed_limitperiod);
    void radioButton_changed(QVariant value);
    void get_max_downloadnum_changed(QString maxNum);
    void get_poweron_changed(int poweronStatus);


private:

    Dtk::Core::QSettingBackend *m_backend;

    QString m_configPath;
    bool m_userChangeKey = false;




};

#endif // SETTINGS_H
