/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file newtaskwidget.h
 *
 * @brief 新建任务类
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

#ifndef NEWTASKWIDGET_H
#define NEWTASKWIDGET_H

#include "settings.h"
#include <dfilechooseredit.h>
#include <DMainWindow>
#include <DPushButton>
#include <DDialog>
#include <DLabel>
#include <DTextEdit>
#include <DSuggestButton>
#include <DFileDialog>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCloseEvent>

class BtInfoDialog;
class MessageBox;

DWIDGET_USE_NAMESPACE

class  CreateTaskWidget :public DDialog
{
    Q_OBJECT
public:
    explicit CreateTaskWidget(DDialog *parent=0);
    ~CreateTaskWidget();

    void setUrl(QString url);
private:
    /**
     * @brief 初始化ui
     */
    void initUi();

    /**
     * @brief 是否为磁力链接
     * @param url 路径
     * @return  true 为磁力链接  false 为不是磁力链接
    */
    bool isMagnet(QString url);

    /**
     * @brief 是否为http链接
     * @param url 路径
     * @return  true 为http链接  false 为不是http链接
    */
    bool isHttp(QString url);

    /**
     * @brief 根据重定向url 获取真是url地址,并发送到主界面
     * @param 重定向地址
     * @return  返回真是url地址
    */
    void getTruetUrl(QString redirecUrl);


private slots:
    /**
     * @brief 打开选择文件窗口按钮
     */
    void onFileDialogOpen();
    /**
     * @brief 关闭窗口按钮
     */
    void onCancelBtnClicked();
    /**
     * @brief 确定下载按钮
     */
    void onSureBtnClicked();
    /**
     * @brief url文本框变化
     */
    void onTextChanged();

    /**
     * @brief 选择下载路径
     */
    void onFilechoosed(const QString &);

protected:
    /**
     * @brief 拖拽处理函数
     * @param event 拖拽事件
    */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * @brief 拖拽释放处理函数
     * @param event 拖拽事件
    */
    void dropEvent(QDropEvent *event);

    void closeEvent(QCloseEvent *event);

signals:
    /**
     * @brief 新建http下载任务
     * @param url http下载地址
     * @param save_path 保存路径
     * @param path_name 文件名字
    */
    void downloadWidgetCreate(QStringList urlList,QString savePath,QString pathName);
    /**
     * @brief 新建bt下载任务
     * @param btPath bt下载地址
     * @param opt 选项
     * @param infoName bt文件名字
     * @param infoHash bt文件hash值
    */
    void downLoadTorrentCreate(QString btPath,QMap<QString,QVariant> &opt,QString infoName, QString infoHash);

private:
    DTextEdit *m_texturl;               //url文本框
    DSuggestButton *m_sureButton;      //确认按钮
    DFileChooserEdit *m_editDir;    //选择下载路径窗口
    QString m_defaultDownloadDir;   //默认文件路径
};


#endif // NEWTASKWIDGET_H
