#ifndef STUBALL_H
#define STUBALL_H

#include <QThread>
#include <QThreadPool>
#include <QDebug>
//#include <DDialog>
#include <QMenu>
#include <QCursor>
#include <QAction>

void QthreadpoolStart(QRunnable *runnable, int priority = 0);

QAction* QmenuExec(const QPoint &pos);

int DsettingsdialogExec(void* obj);

int DiagnostictoolExec(void* obj);

int MessageboxExec();

void QsystemtrayiconShow();

void MainframeOndownloadnewurl(QString url, QString savePath, QString fileName, QString type);

bool MainframeShowredownloadmsgbox(const QString sameUrl);

bool QiodeviceOpen(QIODevice::OpenMode flags);

double CreatetaskwidgetFormatspeed(QString str);

void ClipboardtimerCheckclipboardhasurl();

void BtinfodialogSetwindowtitle(QString & str);

#endif // STUBALLTHREAD_H
