#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <DWidgetUtil>
#include <DMainWindow>
#include <DTitlebar>
#include <DIconButton>
#include <DApplication>
#include <QJsonObject>

class MainFrame : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = Q_NULLPTR);
    ~MainFrame();
private:
    void initAria2();//初始化aria2
private slots:
    void slotRPCSuccess(QString method, QJsonObject json);//处理rpc成功返回的信息
    void slotRPCError(QString method, QString id, int);//处理返回的错误信息
};

#endif // MAINFRAME_H
