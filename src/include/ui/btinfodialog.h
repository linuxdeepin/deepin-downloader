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

#include "btinfotableview.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The BtInfoDialog class
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
    QString m_defaultDownloadDir;
    QStandardItemModel *m_model;
    BtInfoDelegate *m_delegate;
    QString m_bt_last_save_path;

private:
    DTitlebar *m_titleBar;//标题栏
    BtInfoTableView *m_tableView;//列表
    DWidget *m_widget;

    DPushButton *m_btnOK;//Download按钮
    //DPushButton *btnSelectDir;

    DLabel *m_labelTitle;
    DLabel *m_labelSelectedFileNum;
    DLabel *m_labelFileSize;
    DLabel *m_labelCapacityFree;
    DLabel *m_folderIcon;

    DLabel *m_labelInfoName;
    DLineEdit *m_editInfoName;
    DFileChooserEdit *m_editDir;

    DCheckBox *m_checkAll;
    DCheckBox *m_checkVideo;
    DCheckBox *m_checkAudio;
    DCheckBox *m_checkPicture;
    DCheckBox *m_checkOther;

public slots:
    void slot_btnSelectDir();
    void slot_btnOK();
    void slot_checkAll();
    void slot_checkVideo();
    void slot_checkAudio();
    void slot_checkPicture();
    void slot_checkOther();
    void slot_filechoosed(const QString &);

    void slot_paletteTypeChanged(DGuiApplicationHelper::ColorType type);

};

#endif // BTINFODIALOG_H
