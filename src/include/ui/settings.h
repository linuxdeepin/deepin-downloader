/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file settings.h
 *
 * @brief 设置类，主要实现调用json文件，生成配置文件；创建自定义的控件窗口
 *
 *@date 2020-06-09 09:50
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <DDialog>
#include <DSettingsDialog>
#include <DSettingsWidgetFactory>

#include <QObject>
#include <QSettings>
#include <qsettingbackend.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

struct S_DownloadSettings
{
    QString m_strType; // 下载类型，限速下载为1,全速下载为0
    QString m_strMaxDownload; // 最大下载限速
    QString m_strMaxUpload; // 最大上传限速
    QString m_strStartTime; // 限速开始时间
    QString m_strEndTime; // 限速结束时间
};

/**
 * @class Settings
 * @brief 设置类
*/
class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief 获取单例对象
     * @return 返回单例对象
     */
    static Settings *getInstance();

    /**
     * @brief 创建下载目录窗口
     * @param obj option对象
     */
    static QWidget *createFileChooserEditHandle(QObject *obj);

    /**
     * @brief 创建HTTP下载类型窗口
     * @param obj option对象
     */
    static QWidget *createHttpDownloadEditHandle(QObject *obj);

    /**
     * @brief 创建BT下载类型窗口
     * @param obj option对象
     */
    static QWidget *createBTDownloadEditHandle(QObject *obj);

    /**
     * @brief 创建磁力链接下载类型窗口
     * @param obj option对象
     */
    static QWidget *createMagneticDownloadEditHandle(QObject *obj);

    /**
     * @brief 创建下载托盘窗口
     * @param obj option对象
     */
    static QWidget *createDownloadTraySettingHandle(QObject *obj);

    /**
     * @brief 创建下载磁盘缓存窗口
     * @param obj option对象
     */
    static QWidget *createDownloadDiskCacheSettiingHandle(QObject *obj);

    /**
     * @brief 创建下载设置窗口
     * @param obj option对象
     */
    static QWidget *createDownloadSpeedLimitSettiingHandle(QObject *obj);

    /**
     * @brief 获取开机启动状态值
     * @return 开启返回true，否则返回false
     */
    bool getPowerOnState();

    /**
     * @brief 获取启动后自动获取未完成的任务状态值
     * @return 开启返回true，否则返回false
     */
    bool getAutostartUnfinishedTaskState();

    /**
     * @brief 获取下载目录选择选项
     * @return 自动修改为上次使用的目录返回1,默认目录返回0
     */
    int getDownloadDirectorySelected();

    /**
     * @brief 获取下载保存路径
     * @return 返回保存地址
     */
    QString getDownloadSavePath();

    /**
     * @brief 获取一键下载状态值
     * @return 开启返回true，否则返回false
     */
    bool getOneClickDownloadState();

    /**
     * @brief 获取关闭主界面选择选项
     * @return 退出下载器返回1,最小化到托盘返回0
     */
    int getCloseMainWindowSelected();

    /**
     * @brief 获取同时下载最大任务数
     * @return 返回最大任务个数
     */
    int getMaxDownloadTaskNumber();

    /**
     * @brief 获取下载完成后自动打开状态值
     * @return 开启返回true，否则返回false
     */
    bool getDownloadFinishedOpenState();

    /**
     * @brief 获取自动删除“文件不存在”任务状态值
     * @return 开启返回true，否则返回false
     */
    bool getAutoDeleteFileNoExistentTaskState();

    /**
     * @brief 获取下载设置选择选项
     * @return 限速下载返回1,全速下载返回0
     */
    int getDownloadSettingSelected();

    /**
     * @brief 获取最大下载限速值
     * @return 限速值
     */
    QString getMaxDownloadSpeedLimit();

    /**
     * @brief 获取最大上传限速值
     * @return 限速值
     */
    QString getMaxUploadSpeedLimit();

    /**
     * @brief 获取限速时段开始时间
     * @return 开始时间字符串
     */
    QString getSpeedLimitStartTime();

    /**
     * @brief 获取限速时段结束时间
     * @return 结束时间字符串
     */
    QString getSpeedLimitEndTime();

    /**
     * @brief 获取所有限速下载限制参数
     * @return 限制参数
     */
    S_DownloadSettings getAllSpeedLimitInfo();

    /**
     * @brief 获取剪切板状态值
     * @return 开启返回true，否则返回false
     */
    bool getClipBoardState();

    /**
     * @brief 获取Http下载状态值
     * @return 选中返回true，否则返回false
     */
    bool getHttpDownloadState();

    /**
     * @brief 获取Bt下载状态值
     * @return 选中返回true，否则返回false
     */
    bool getBtDownloadState();

    /**
     * @brief 获取磁力链接下载状态值
     * @return 选中返回true，否则返回false
     */
    bool getMagneticDownloadState();

    /**
     * @brief 获取下载种子文件后自动打开新建窗口状态值
     * @return 开启返回true，否则返回false
     */
    bool getAutoOpenNewTaskWidgetState();

    /**
     * @brief 获取启动时关联BT种子文件状态值
     * @return 开启返回true，否则返回false
     */
    bool getStartAssociatedBTFileState();

    /**
     * @brief 获取下载完成/失败时系统通知提醒状态值
     * @return 开启返回true，否则返回false
     */
    bool getDownloadInfoSystemNotifyState();

    /**
     * @brief 获取下载完成后播放提示音状态值
     * @return 开启返回true，否则返回false
     */
    bool getDownloadFinishedPlayToneState();

    /**
     * @brief 获取新建任务时显示主界面状态值
     * @return 开启返回true，否则返回false
     */
    bool getNewTaskShowMainWindowState();

    /**
     * @brief 获取打开主界面快捷键
     * @return 返回快捷键，如：Alt+D
     */
    QString getOpenMainWindowShortcutKey();

    /**
     * @brief 获取磁盘缓存值
     * @return 返回数值
     */
    int getDisckcacheNum();

    /**
     * @brief 设置关闭主界面选择选项
     * @param nSelect 设置选中选项，退出下载器设置1,最小化到托盘设置0
     */
    void setCloseMainWindowSelected(int nSelect);

    /**
     * @brief 获取自定义文件路径
     * @return 返回数值
     */
    QString getCustomFilePath();

    /**
     * @brief 设置自定义文件路径
     * @return 返回数值
     */
    void setCustomFilePath(const QString &path);

    /**
     * @brief 获取关闭时是否显示提示
     * @return 返回数值
     */
    bool getIsShowTip();

    /**
     * @brief 设置关闭时是否显示提示
     * @return 返回数值
     */
    void setIsShowTip(bool b);



    DSettings *m_pSettings;

signals:
    void poweronChanged(bool bState);
    void maxDownloadTaskNumberChanged(int nTaskNumber);
    void downloadSettingsChanged();
    void disckCacheChanged(int nNum);

public slots:

private:
    static Settings *s_pInstance;
    Dtk::Core::QSettingBackend *m_pBackend;
    QString m_configPath; // 配置文件路径
    QSettings *m_pIniFile;

};

#endif // SETTINGS_H
