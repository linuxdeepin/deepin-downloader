#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <DWidgetUtil>
#include <DMainWindow>
#include <DTitlebar>
#include <DIconButton>
#include <DApplication>

class MainFrame : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainFrame(QWidget *parent = Q_NULLPTR);
    ~MainFrame();

};

#endif // MAINFRAME_H
