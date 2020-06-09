#include "database.h"

DataBase::DataBase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");

   // QString _dataBasePath = qApp->applicationDirPath();
    QString _dbPath = QString("%1/%2/%3/uos-downloadmanager-task.db")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName());
    QFileInfo *databaseFile = new QFileInfo(_dbPath);

    //如果数据库文件不存在
    if (!databaseFile->isFile()) {
        //拷贝数据库文件
        qDebug() << "error: no db file  "; //<< QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_FILENAME << QString(_dataBasePath) + UOS_DOWNLOAD_DATABASE_FILENAME << endl;

        QFile::copy(QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_OLD_FILENAME,
                    _dbPath);

        QString strold = UOS_DONWLOAD_DATABASE_PATH + "uos-downloadmanager-task.db";
        QByteArray ba = strold.toLatin1();
        char *old_database_file = ba.data();
        remove(old_database_file);
        QFile::copy(strold,_dbPath);

    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(_dbPath);
}

DataBase &DataBase::Instance()
{
    static DataBase *_d = nullptr;
    if (_d == nullptr) {
        _d = new DataBase();
    }
    return *_d;
}

QSqlDatabase &DataBase::getDB()
{
    return m_db;
}
