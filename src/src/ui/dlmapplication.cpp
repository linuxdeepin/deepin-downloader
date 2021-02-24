#include "dlmapplication.h"
#include <QDebug>
DlmApplication::DlmApplication(int &argc, char **argv)
    : DApplication(argc, argv)
{
}

void DlmApplication::handleQuitAction()
{
    emit applicatinQuit(false);
}
