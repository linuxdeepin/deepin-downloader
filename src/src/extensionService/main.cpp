// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @copyright 2021-2021 Uniontech Technology Co., Ltd.
 *
 * @file main.cpp
 *
 * @brief 下载器与浏览器扩展交互后台进程
 *
 * @date 2021-06-29 16:01
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
**/


#include <QCoreApplication>
#include <QDebug>

#include "extensionservice.h"
#include "ddlog.h"
#include "logger.h"

int main(int argc, char** argv)
{
    // 动态日志规则必须最先初始化：在任何日志打印之前构造 MLogger，
    // 否则 QT_LOGGING_RULES 环境变量取消设置不生效（参考 deepin-reader）
    static MLogger logger;
    Q_UNUSED(logger)

    qCDebug(extensionLog) << "[ExtensionMain] Starting extension service";

    QCoreApplication a(argc, argv);
    extensionService e;
    return a.exec();
}
