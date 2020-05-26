/**
* @file filesavepathchooser.h
* @brief 下载目录窗口，主要实现切换单选按钮时，文件保存路径控件的使能以及设置数据的传递与显示
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 15:19
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef FILESAVEPATHCHOOSER_H
#define FILESAVEPATHCHOOSER_H

#include <QWidget>
#include <DRadioButton>
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE

/**
 * @class FileSavePathChooser
 * @brief 下载目录窗口类
*/
class FileSavePathChooser : public QWidget
{
    Q_OBJECT
public:
    explicit FileSavePathChooser(int nCurrentSelect, const QString &strDownloadPath);

    /**
     * @brief 设置文件保存路径
     * @param strText 文件路径
     */
    void setLineEditText(const QString &strText);

    /**
     * @brief 设置当前选中按钮，当为1时，选中上次使用目录按钮
     * @param nCurrentSelect 当前选中值
     */
    void setCurrentSelectRadioButton(int nCurrentSelect);

signals:
    void textChanged(QString strText);

public slots:

    /**
     * @brief 单选按钮选择响应的槽函数
     */
    void radioButtonClickSlot();

    /**
     * @brief 文件保存路径改变响应的槽函数
     * @param strText 改变后的路径
     */
    void lineEditTextChanged(const QString &strText);

private:

    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DFileChooserEdit *m_pFileChooserEdit; // 文件保存路径选择
    DRadioButton *m_pAutoLastPathRadioButton; // 自动修改为上次使用的目录按钮
    DRadioButton *m_pCustomsPathRadioButton; // 设置默认目录按钮
    int m_nCurrentSelect; // 当前选中按钮值
    QString m_strDownloadPath; // 当前保存路径
};

#endif // FILESAVEPATHCHOOSER_H
