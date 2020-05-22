#ifndef NEWTASKWIDGET_H
#define NEWTASKWIDGET_H

#include <DMainWindow>
#include <QObject>
#include <DPushButton>
#include <DDialog>
#include <DLabel>
#include <DTextEdit>
#include <QHBoxLayout>
#include <DSuggestButton>
#include <DFileDialog>
#include <QSizePolicy>
#include <QDropEvent>
#include <QMimeData>
#include "btinfodialog.h"

DWIDGET_USE_NAMESPACE

/**
 * @class newTaskWidget
 * @brief 新建任务类
*/

class  newTaskWidget :public DDialog
{
    Q_OBJECT
public:
    explicit newTaskWidget(QString path, DDialog *parent=0);
    ~newTaskWidget();
private:
    void initUi();      //初始化ui


private slots:

    void openfileDialog();  //打开选择文件窗口
    void onCancelBtnClicked();  //关闭窗口
    void onSureBtnClicked();    //确定下载


protected:
    void dragEnterEvent(QDragEnterEvent *event);        // 拖拽处理函数
    void dropEvent(QDropEvent *event);                  // 拖拽释放处理函数

private:
    QString m_savePath;                 //当前保存文件路径
};


#endif // NEWTASKWIDGET_H
