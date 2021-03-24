#ifndef STUBALL_H
#define STUBALL_H

#include <QThread>
#include <QThreadPool>
#include <QDebug>
//#include <DDialog>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QFileDialog>

void QthreadpoolStart(QRunnable *runnable, int priority = 0);

QAction *QmenuExec(const QPoint &pos);

int DsettingsdialogExec(void *obj);

int DiagnostictoolExec(void *obj);

int MessageboxExec();

int createtaskwidgetExec();

void QsystemtrayiconShow();

void MainframeOndownloadnewurl(QString url, QString savePath, QString fileName, QString type);

bool MainframeShowredownloadmsgbox(const QString sameUrl);

bool QiodeviceOpen(QIODevice::OpenMode flags);

double CreatetaskwidgetFormatspeed(QString str);

void ClipboardtimerCheckclipboardhasurl();

void MainFrameInitWebsocket();

void BtinfodialogSetwindowtitle(QString &str);

QString BtGetSelectedNull();

bool BtGetSelectedIsNull();

bool returnFalse();

bool returnTrue();

int UrlThreadGet405();

int ApplicationHelperThemeType2();

static QString BtOnFileDialogOpen(QWidget *parent = nullptr,
                           const QString &caption = QString(),
                           const QString &dir = QString(),
                           const QString &filter = QString(),
                           QString *selectedFilter = nullptr,
                                  QFileDialog::Options options = QFileDialog::Options()){return "1.torrent";}


void TableDataControlAutoDownloadBt(QString btFilePath);
#endif // STUBALLTHREAD_H
