/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file messagebox.h
 *
 * @brief 提示窗口类
 *
 * @date 2020-06-09 10:52
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
#include "global.h"
#include <DDialog>
#include <DLabel>
#include <DTextEdit>
#include <DCheckBox>
#include <DLineEdit>
#include <DRadioButton>

DWIDGET_USE_NAMESPACE

class QPushButton;

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
     * @param sameUrlCount 相同url数量
     * @param sameUrlList 相同url列表
    */
    void setWarings(QString warningMsg,QString surebtntext,QString cancalbtntext="",int sameUrlCount=0,QList<QString> sameUrlList={});
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
    void RenameSig(QString newname);
    /**
     * @brief 清空信号
     * @param ret 是否清空全部
    */
    void ClearrecycleSig(bool ret);
    /**
     * @brief 删除信号
     * @param ret
     * @param ret1
    */
    void DeletedownloadSig(bool ret,bool ret1);

    /**
     * @brief 关闭确认信号
    */
    void signalCloseConfirm();

public slots:
    /**
     * @brief 重命名编辑框更新槽函数
     * @param text 更改内容
    */
    void getRenamelineeditChanged(const QString &text);
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
    bool m_bDeleteFlag;
    DCheckBox *m_pCheckBox;
    DLineEdit *m_pNewnameLineedit;
    QPushButton *m_pRenameSureButton;

    DRadioButton *m_pButtonQuit;
    DRadioButton *m_pButtonMin;
};

#endif // MESSAGEBOX_H
