/**
* @file messagebox.h
* @brief 提示窗口类
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date 2020-06-01 15:58
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
#include <DDialog>
#include <DLabel>
#include <DTextEdit>
#include <DCheckBox>
#include <DLineEdit>
#include <QPushButton>
#include <DRadioButton>
#include "global.h"

DWIDGET_USE_NAMESPACE

class MessageBox : public DDialog
{
    Q_OBJECT
public:
    explicit MessageBox(DDialog *parent = nullptr);

public:
    /**
     * @brief 设置警告窗口
     * @param warningMsg 文本内容
     * @param surebtntext 确定按钮
     * @param cancalbtntext="" 取消按钮，可不填
     * @param sameurl_count 相同url数量
     * @param same_url_list 相同url列表
    */
    void setWarings(QString warningMsg,QString surebtntext,QString cancalbtntext="",int sameurl_count=0,QList<QString> same_url_list={});
    /**
     * @brief 设置删除窗口
     * @param permanentl bool类型，真为选择删除文件同时，删除本地文件。 假为可选择是否删除本地文件。
    */
    void setDelete(bool permanentl);
    /**
     * @brief 设置清空窗口。可选择是否清空回收站及删除本地文件
    */
    void setClear();
    /**
     * @brief 设置重命名窗口
     * @param title 设置标题
     * @param surebtntext 设置确定按钮内容
     * @param cancelbtn 设置取消按钮内容
     * @param oldname 曾用名
    */
    void setReName(QString title,QString surebtntext,QString cancelbtn,QString oldname);
    /**
     * @brief 设置退出窗口
    */
    void setExit();
private:
    /**
     * @brief 设置label
     * @param text label内容
    */
    void addLabel(QString text);
    /**
     * @brief 设置radioGroup
     * @param text label内容
    */
    void addRadioGroup(QString quitText, QString minText);
    /**
     * @brief Checkbox
     * @param text Checkbox内容
    */
    void addCheckbox(QString checkboxText);
signals:
    /**
     * @brief 重命名信号
     * @param newname 新名字
    */
    void ReName_sig(QString newname);
    /**
     * @brief 清空信号
     * @param ret 是否清空全部
    */
    void ClearRecycle_sig(bool ret);
    /**
     * @brief 删除信号
     * @param ret
     * @param ret1
    */
    void DeleteDownload_sig(bool ret,bool ret1);

    /**
     * @brief 关闭确认信号
    */
    void signalCloseConfirm();

public slots:
    /**
     * @brief 重命名编辑框更新槽函数
     * @param text 更改内容
    */
    void get_renameLineEdit_changed(const QString &text);
    /**
     * @brief 确定重命名
    */
    void renameSureBtn();
    /**
     * @brief 确定清空
    */
    void clearBtn(int index);
    /**
     * @brief 确定删除
    */
    void deleteBtn(int index);

    /** 右上角退出按钮按下弹出框的操作
     * @brief
    */
    void ExitBtn(int index);

private:
    bool m_deleteFlag;
    DCheckBox *m_checkbox;
    DLineEdit *newName_lineedit;
    QPushButton *rename_sure_button;

    DRadioButton *m_pButtonQuit;
    DRadioButton *m_pButtonMin;
};

#endif // MESSAGEBOX_H
