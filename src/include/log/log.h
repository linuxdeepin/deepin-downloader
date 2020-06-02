#ifndef LOG_H
#define LOG_H

#include <QMutex>
#include <QString>
#include <QStorageInfo>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

#define DEFALT_REMAIN_SIZE 100 * 1024 * 1024
#define MAXLOGSIZE 10 * 1024 * 1024
#define DEFALT_REMAIN_TIME 7

static QMutex s_logMutex;
static QString s_logPath;
static QString _logDir;
static QFile _logFile;

void setLogDir(const QString &dir);
void CheckFreeDisk();
void CheckLogTime();
void CreateNewLog();
bool CheckRotateSize();
void setLogPath(const QString &path);
void setLogLevel(int level);
void customLogMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
#endif // LOG_H
