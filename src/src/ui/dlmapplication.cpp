// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dlmapplication.h"
#include <QDebug>
#include "ddlog.h"
DlmApplication::DlmApplication(int &argc, char **argv)
    : DApplication(argc, argv)
{
    qCDebug(downloaderLog) << "Download Manager application initialized";
}

void DlmApplication::handleQuitAction()
{
    qCDebug(downloaderLog) << "Handling application quit action";
    emit applicatinQuit(false);
}
