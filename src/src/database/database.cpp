#include "database.h"

DataBase::DataBase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString _dataBasePath = qApp->applicationDirPath();
    QString newDataBase = QString("%1/%2/%3/%4")
                                    .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                    .arg(qApp->organizationName())
                                    .arg(qApp->applicationName())
                                    .arg(UOS_DOWNLOAD_DATABASE_FILENAME);

    QFileInfo *databaseFile = new QFileInfo(newDataBase);

    //如果数据库文件不存在
    if (!databaseFile->isFile()) {
        //拷贝数据库文件
        qDebug() << "copy file " << QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_FILENAME << QString(_dataBasePath) + UOS_DOWNLOAD_DATABASE_FILENAME << endl;

        QString strold = _dataBasePath + UOS_DOWNLOAD_DATABASE_OLD_FILENAME;
        QByteArray ba = strold.toLatin1();
        char *old_database_file = ba.data();
        remove(old_database_file);



        QFile::copy(QString(UOS_DONWLOAD_DATABASE_PATH) + UOS_DOWNLOAD_DATABASE_OLD_FILENAME,
                    newDataBase);
                    //QString(_dataBasePath + "/" + UOS_DOWNLOAD_DATABASE_FILENAME));
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(newDataBase);
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
