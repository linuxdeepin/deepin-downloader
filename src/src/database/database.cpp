// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "database.h"

DataBase::DataBase()
{
    qDebug() << "Initializing database connection";
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    // QString _dataBasePath = qApp->applicationDirPath();
    QString dbPath = QString("%1/%2/%3/downloader.db")
                         .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                         .arg(qApp->organizationName())
                         .arg(qApp->applicationName());
    QFileInfo databaseFile(dbPath);

    //如果数据库文件不存在
    if (!databaseFile.isFile()) {
        //拷贝数据库文件
        qDebug() << "error: no db file  "; //<< QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_FILENAME << QString(_dataBasePath) + UOS_DOWNLOAD_DATABASE_FILENAME << Qt::endl;
        QFile::copy(QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_OLD_FILENAME,
                    dbPath);
        QString strold = UOS_DONWLOAD_DATABASE_PATH + "downloader.db";
        QByteArray ba = strold.toLatin1();
        char *oldDatabaseFile = ba.data();
        remove(oldDatabaseFile);
        QFile::copy(strold, dbPath);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    m_db.open();
    qDebug() << "[Database] DataBase constructor ended";
}

DataBase &DataBase::Instance()
{
    // qDebug() << "[Database] Instance function started";
    static DataBase *d = nullptr;
    if (d == nullptr) {
        // qDebug() << "[Database] Creating new DataBase instance";
        d = new DataBase();
    }
    // qDebug() << "[Database] Instance function ended";
    return *d;
}

QSqlDatabase &DataBase::getDB()
{
    // qDebug() << "[Database] getDB function started";
    return m_db;
}

void DataBase::destory()
{
    // qDebug() << "[Database] destory function started";
    m_db.close();
}
