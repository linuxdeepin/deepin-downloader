
#ifndef TABLEVIEW_H
#define TABLEVIEW_H


#include <QTableView>

class TableView:public QTableView
{
    Q_OBJECT
public:
    TableView(int Flag);
    void reset(bool switched=false);
   // RecycleTableViewModel* get_recycletableViewModel();


private:
    void initUI();
    void initConnections();
    void initTableView();
    void setTestData();
signals:
    void get_datachanged();
    void header_stateChanged(bool checked);
    void clear_header_check();
    void get_tableview_allchecked(bool checked);
    void signal_hoverChanged(const QModelIndex &index);
protected:
    void mousePressEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    int m_iTableFlag;

};

#endif // TABLEVIEW_H
