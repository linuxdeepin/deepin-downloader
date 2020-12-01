#include "stubAll.h"

QAction *QMenu_exec(const QPoint &pos)
{
    return nullptr;
}

int DSettingsDialog_exec(void* obj)
{
    return 0;
}

int DiagnosticTool_exec(void* obj)
{
    return 0;
}

int MessageBox_exec(void* obj)
{
    return 1;
}

void QSystemTrayIcon_show()
{
    return;
}

void MainFrame_onDownloadNewUrl(QString url, QString savePath, QString fileName, QString type)
{
    return;
}

bool MainFrame_showRedownloadMsgbox(const QString sameUrl)
{
    return true;
}

bool QIODevice_open(QIODevice::OpenMode flags)
{
    return true;
}

void ClipboardTimer_checkClipboardHasUrl()
{
    return;
}
