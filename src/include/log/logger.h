// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QLoggingCategory>

#include <DConfig>

DCORE_USE_NAMESPACE

/**
 * @brief 动态日志规则管理器（参考 deepin-reader 的 MLogger 实现）
 *
 * 从 DConfig 读取 log_rules 并应用为 Qt 日志过滤规则，同时监听
 * DConfig 变更实现运行中动态调整（调试模式 deepin-debug-config 依赖此能力）。
 *
 * 注意：必须在任何日志打印之前构造（main 函数第一条语句），
 * 否则 QT_LOGGING_RULES 环境变量取消设置不生效。
 */
class MLogger : public QObject
{
public:
    explicit MLogger(QObject *parent = nullptr)
        : QObject(parent)
    {
        QByteArray logRules = qgetenv("QT_LOGGING_RULES");
        // qunsetenv 之前一定不要有任何日志打印，否则取消环境变量设置不会生效
        qunsetenv("QT_LOGGING_RULES");

        // 环境变量规则兜底（DConfig 不可用时仍能通过环境变量控制）
        m_rules = logRules;

        // dconfig 规则：appId / name 均为 org.deepin.downloader
        m_config = DConfig::create("org.deepin.downloader", "org.deepin.downloader");
        logRules = m_config->value("log_rules").toByteArray();
        if (logRules.isEmpty()) {
            // DConfig 不可用或 meta 未安装时，兜底使用与 meta 默认值完全一致的规则，
            // 避免退回 Qt 默认（debug 全开）把日志刷进文件
            logRules = "*.downloader*.debug=false;*.downloader*.info=false;*.downloader*.warning=true";
        }
        appendRules(logRules);
        setRules(m_rules);

        // 监听 dconfig 变更，实现运行中动态调整
        connect(m_config, &DConfig::valueChanged, this, [this](const QString &key) {
            if (key == "log_rules")
                setRules(m_config->value(key).toByteArray());
        });
    }

    ~MLogger() override
    {
        if (m_config) {
            m_config->deleteLater();
            m_config = nullptr;
        }
    }

    inline QString rules() const { return m_rules; }

    void setRules(const QString &rules)
    {
        auto tmpRules = rules;
        // Qt 限制：setFilterRules 分隔符必须 '\n'，不能是 ';'
        m_rules = tmpRules.replace(";", "\n");
        QLoggingCategory::setFilterRules(m_rules);
    }

private:
    void appendRules(const QString &rules)
    {
        QString tmpRules = rules;
        tmpRules = tmpRules.replace(";", "\n");
        auto tmplist = tmpRules.split('\n');
        for (int i = 0; i < tmplist.count(); i++)
            if (m_rules.contains(tmplist.at(i))) {
                tmplist.removeAt(i);
                i--;
            }
        if (tmplist.isEmpty())
            return;
        m_rules.isEmpty() ? m_rules = tmplist.join("\n")
                          : m_rules += "\n" + tmplist.join("\n");
    }

    QString m_rules;
    DConfig *m_config = nullptr;
};

#endif // LOGGER_H
