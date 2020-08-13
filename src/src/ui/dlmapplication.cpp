#include "dlmapplication.h"

DlmApplication::DlmApplication(int &argc, char **argv):
    DApplication (argc, argv)
{

}

void DlmApplication::handleQuitAction()
{
    emit applicatinQuit();
}
