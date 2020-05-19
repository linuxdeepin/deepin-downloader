#include "mainframe.h"
#include "aria2rpcinterface.h"
#include <QAction>
#include <DMenu>


MainFrame::MainFrame(QWidget *parent) :
    DMainWindow(parent)
{

}

MainFrame::~MainFrame()
{

}

//初始化aria2
void MainFrame::initAria2()
{
    //aria2c = new Aria2cInterface(this);
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_success(QString, QJsonObject)), this, SLOT(slotRpcSuccess(QString, QJsonObject)));
    connect(Aria2RPCInterface::Instance(), SIGNAL(signal_error(QString, QString, int)), this, SLOT(slotRpcError(QString, QString, int)));
}

void MainFrame::slotRPCSuccess(QString method, QJsonObject json)
{

}

void MainFrame::slotRPCError(QString method, QString id, int error)
{

}


