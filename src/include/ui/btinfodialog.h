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
#include "aria2rpcinterface.h"
#include "btinfotableview.h"


DWIDGET_USE_NAMESPACE

/**
* @file %{CurrentDocument:btinfodialog.h}
* @brief BT窗口类
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date %{CurrentDate:2020-05-26} %{CurrentTime:17:59}
* @copyright 2020-%{CurrentDate:2020} Uniontech Technology Co., Ltd.
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
     * @return 选择的下载到路径
     */
    QString getSaveto();

    /**
     * @brief getName 获取bt信息下载名称
     * @return bt信息下载名称
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

    /**
     * @brief getUrlInfo 获取bt信息
     * @param opt bt基本信息
     * @param infoName 文件名字
     * @param infoHash 文件hash值
     */
    void getBtInfo(QMap<QString,QVariant> &opt, QString &infoName, QString &infoHash);

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
    /**
     * @brief 确定按钮
     */
    void slot_btnOK();          //确定按钮
    /**
     * @brief 全选按钮
     */
    void slot_checkAll();       //全选按钮
    /**
     * @brief 视频按钮
     */
    void slot_checkVideo();     //视频按钮
    /**
     * @brief 视频按钮
     */
    void slot_checkAudio();     //音频按钮
    /**
     * @brief 视频按钮
     */
    void slot_checkPicture();   //图片按钮
    /**
     * @brief 视频按钮
     */
    void slot_checkOther();     //其他按钮
    /**
     * @brief 选择下载路径
     */
    void slot_filechoosed(const QString &);
    /**
     * @brief 随dtk主题变化
     */
    void slot_paletteTypeChanged(DGuiApplicationHelper::ColorType type);

};

#endif // BTINFODIALOG_H
