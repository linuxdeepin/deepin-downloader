#include "dbdatacontrol.h"
#include "dbinstance.h"


dbDataControl::dbDataControl(QObject *parent)
    :QObject (parent)
{
     DBInstance::getAllUrl(m_UrlInfoList);
     DBInstance::getAllTask(m_TaskList);
     DBInstance::getAllTaskStatus(m_TashkStatusList);
}
