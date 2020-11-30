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

int DSettingsDialog_exec();

int DiagnosticTool_exec();

int MessageBox_exec();

#endif // STUBALLTHREAD_H
