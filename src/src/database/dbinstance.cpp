#include "dbinstance.h"

bool DBInstance::getData()
{
    QSqlDatabase _q = DataBase::Instance().getDB();
    if(!_q.open())
    {
        qDebug()<<_q.lastError();
        return  false;
    }
    QString _str = "select * from url_info";
    QSqlQuery _sql;
    _sql.prepare(_str);
    if(!_sql.exec())
    {
         qDebug()<< _sql.lastError();
        _q.close();
        return false;
    }
    while(_sql.next())
    {
        QString task_id = _sql.value(0).toString();
        int download_status = _sql.value(1).toInt(); //下载状态
        QString compeletedLength = _sql.value(3).toString();
        QString download_speed = _sql.value(4).toString();
        QString totalLength = _sql.value(5).toString();
        int percent = _sql.value(6).toInt();
        int totalFromSource = _sql.value(7).toInt();

    }
    _q.close();
    return true;
}


