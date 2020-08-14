#ifndef DLMAPPLICATION_H
#define DLMAPPLICATION_H

#include <DApplication>
DWIDGET_USE_NAMESPACE

class DlmApplication: public DApplication
{
    Q_OBJECT
public:
    DlmApplication(int &argc, char **argv);
protected:
    virtual void handleQuitAction();
signals:
    void applicatinQuit();
};

#endif // DLMAPPLICATION_H
