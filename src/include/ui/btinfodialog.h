#ifndef BTINFODIALOG_H
#define BTINFODIALOG_H

//#include "aria2cinterface.h"
#include "btinfodelegate.h"

#include <DDialog>
#include <QDebug>
#include <DTableView>
#include <DPushButton>
#include <DLineEdit>
#include <DLabel>
#include <DTitlebar>
#include <QStandardItemModel>
#include <DCheckBox>
#include <QStandardPaths>
#include "dfilechooseredit.h"
#include "aria2cbtinfo.h"

#include "btinfotableview.h"


DWIDGET_USE_NAMESPACE


/**
 * @class BtInfoDialog
 * @brief BT窗口类
*/

/* 使用示例：
    QString file = DFileDialog::getOpenFileName(this, tr("Choose Torrent File"), QDir::homePath(), "File(*.torrent)");
    if(file != "") {
        BtInfoDialog *dialog = new BtInfoDialog(file);//torrent文件路径
        int ret = dialog->exec();
        if(ret == QDialog::Accepted) {
            //tood aria2c.addTorrent(file, otp);
        }
        delete dialog;
    }
 */

class BtInfoDialog : public DDialog
{
    Q_OBJECT

public:
    explicit BtInfoDialog(QString torrentFile, QString bt_last_save_path);
    ~BtInfoDialog() override;

    /**
     * @brief getSelected 获取选中下载的文件索引集合(用于aria2的select-file的参数)
     * @return 以逗号分割的索引字符串，如"1,2,3,6"
     */
    QString getSelected();

    /**
     * @brief getSaveto 获取选择的下载到路径
     * @return
     */
    QString getSaveto();

    /**
     * @brief getName 获取bt信息下载名称
     * @return
     */
    QString getName();

    /**
     * @brief updateSelectedInfo 更新文件列表选中后的信息（Delegate内部调用）
     */
    void updateSelectedInfo();

    /**
     * @brief exec
     * @return
     */
    int exec() override;

private:
    void initUI();//初始化UI
    bool isVideo(QString ext);//判断扩展名是否是常见视频格式
    bool isAudio(QString ext);//判断扩展名是否是常见音频格式
    bool isPicture(QString ext);//判断扩展名是否是常见图片格式

private:
    QString m_torrentFile;
    QString m_defaultDownloadDir;   //默热下载文件路径
    QStandardItemModel *m_model;    //tableview中的模型，数据交流
    BtInfoDelegate *m_delegate;     //tableview中选中表格item

private:
    DTitlebar *m_titleBar;//标题栏
    BtInfoTableView *m_tableView;//列表
    DWidget *m_widget;      //包裹view

    DPushButton *m_btnOK;//Download按钮

    DLabel *m_labelTitle;       //标题
    DLabel *m_labelSelectedFileNum; //选中文件数
    DLabel *m_labelFileSize;         //总大小标签
    DLabel *m_labelCapacityFree;    //下载路径所在分区剩余磁盘容量
    DLabel *m_folderIcon;           //文件icon

    DLabel *m_labelInfoName;        //下载信息名称
    DFileChooserEdit *m_editDir;    //选择下载路径窗口

    DCheckBox *m_checkAll;          //文件类型全选
    DCheckBox *m_checkVideo;        //视频文件类型
    DCheckBox *m_checkAudio;        //音频文件类型
    DCheckBox *m_checkPicture;      //图片文件类型
    DCheckBox *m_checkOther;        //其他文件类型
    Aria2cBtInfo m_ariaInfo;        //当前bt文件信息


public slots:
    void slot_btnOK();          //确定按钮
    void slot_checkAll();       //全选按钮
    void slot_checkVideo();     //视频按钮
    void slot_checkAudio();     //音频按钮
    void slot_checkPicture();   //图片按钮
    void slot_checkOther();     //其他按钮
    void slot_filechoosed(const QString &); //选择下载路径

    void slot_paletteTypeChanged(DGuiApplicationHelper::ColorType type);    //跟随dtk主题变化

};

#endif // BTINFODIALOG_H
