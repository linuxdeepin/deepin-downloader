#ifndef TABLEVIEW_H
#define TABLEVIEW_H


#include <QTableView>


class TableModel;
/**
 * @class TableView
 * @brief 下载条目列表
*/
class TableView:public QTableView
{
    Q_OBJECT
public:
    TableView(int Flag);
    void reset(bool switched=false);
   // RecycleTableViewModel* get_recycletableViewModel();


private:
    /**
     * @brief 界面初始化
    */
    void initUI();
    /**
     * @brief 信号槽连接初始化
    */
    void initConnections();
    /**
     * @brief 表格初始化
    */
    void initTableView();
    /**
     * @brief 增加测试数据
    */
    void setTestData();
    /**
     * @brief 获取TableModel指针
    */
    TableModel* get_tableViewModel();
signals:
    void get_datachanged();
    void header_stateChanged(bool checked);
    void clear_header_check();
    void get_tableview_allchecked(bool checked);
    void signal_hoverChanged(const QModelIndex &index);
protected:
    /**
     * @brief 鼠标按下事件
    */
    void mousePressEvent(QMouseEvent *event);
    /**
     * @brief 离开事件
    */
    void leaveEvent(QEvent *event);
    /**
     * @brief 鼠标移动事件
    */
    void mouseMoveEvent(QMouseEvent *event);
    /**
     * @brief 键盘按下事件
    */
    void keyPressEvent(QKeyEvent *event);
private:
    int m_iTableFlag;
    TableModel *m_pTableModel;
};

#endif // TABLEVIEW_H
