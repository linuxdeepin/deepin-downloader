#ifndef HTTPADVANCEDSETTINGWIDGET_H
#define HTTPADVANCEDSETTINGWIDGET_H
#include <ddialog.h>
#include <QDebug>
#include <DButtonBox>
#include <DTextEdit>
#include <QSettings>
#include <DCommandLinkButton>

DWIDGET_USE_NAMESPACE

const QString UOS_DONWLOAD_HTTPADVANCED_PATH = "/usr/share/downloader/database/"; //备份数据库路径

class HttpAdvancedSettingWidget : public DDialog
{
    Q_OBJECT

public:
    explicit HttpAdvancedSettingWidget(DDialog *parent = nullptr);

private:
    /**
     * @brief 初始化ui
     */
    void initUI();

private slots:
    /**
     * @brief 选择后缀名管理按钮
     */
    void onSuffixBtnClicked();
    /**
     * @brief 选择网站管理按钮
     */
    void onWebBtnClicked();
    /**
     * @brief 恢复默认后缀名管理按钮
     */
    void onRstoreDefaultClicked();
    /**
     * @brief 确定按钮
     */
    void onSureClicked();
    /**
     * @brief 取消按钮
     */
    void onCancelClicked();


private:
    DTextEdit *m_textEdit;
    DButtonBox *m_btnBox;
    DCommandLinkButton * m_defaultBtn;
    QString m_configPath;
};



#endif // HTTPADVANCEDSETTINGWIDGET_H
