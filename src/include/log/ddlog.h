// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DDLOG_H
#define DDLOG_H

#include <QDebug>
#include <QLoggingCategory>

// 全项目统一的日志分类，供调试模式（deepin-debug-config）按分类动态过滤：
// 规则通过 DConfig log_rules 下发，如 *.downloader*.debug=true，
// 分类名必须 org.deepin. 前缀且包含 downloader 关键字，便于通配规则匹配。
// 主程序与各动态库（dlmlog/dlmdatabase/dlmaria2/dlmcontrolui）共用主分类。
inline Q_LOGGING_CATEGORY(downloaderLog, "org.deepin.downloader")

// 独立运行单元：浏览器扩展交互后台进程 dlmextensionservice
inline Q_LOGGING_CATEGORY(extensionLog, "org.deepin.downloader.extensionservice")

#endif // DDLOG_H
