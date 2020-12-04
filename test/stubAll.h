#ifndef STUBALL_H
#define STUBALL_H

#include <QThread>
#include <QThreadPool>
#include <QDebug>
#include <DDialog>
#include <QMenu>
#include <QCursor>
#include <QAction>

void QThreadPool_start(QRunnable *runnable, int priority = 0);

QAction* QMenu_exec(const QPoint &pos);

int DSettingsDialog_exec(void* obj);

int DiagnosticTool_exec(void* obj);

int MessageBox_exec(void* obj);

void QSystemTrayIcon_show();

void MainFrame_onDownloadNewUrl(QString url, QString savePath, QString fileName, QString type);

bool MainFrame_showRedownloadMsgbox(const QString sameUrl);

bool QIODevice_open(QIODevice::OpenMode flags);

double CreateTaskWidget_formatSpeed(QString str);

void ClipboardTimer_checkClipboardHasUrl();

void ClipboardTimer_checkClipboardHasUrl();

#endif // STUBALLTHREAD_H
