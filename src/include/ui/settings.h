/**
* @file settings.h
* @brief 设置类，主要实现调用json文件，生成配置文件；创建自定义的控件窗口
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 12:25
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <DDialog>
#include <DSettingsDialog>
#include <DSettingsWidgetFactory>
#include <qsettingbackend.h>
#include <QSettings>
#include <QPointer>
#include <QTime>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

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

    DSettings *m_pSettings;

signals:

public slots:

private:
    Dtk::Core::QSettingBackend *m_pBackend;
    QString m_configPath; // 配置文件路径

};

#endif // SETTINGS_H
