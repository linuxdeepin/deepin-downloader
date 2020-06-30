#include "settingslabel.h"

#include <QHBoxLayout>

SettingsLabel::SettingsLabel(QWidget *parent) : QWidget(parent)
{
    m_pLabel = new DLabel;

    QHBoxLayout *pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(m_pLabel);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pMainLayout);
}

void SettingsLabel::setLabelText(const QString &strText)
{
    m_pLabel->setText(strText);
}

void SettingsLabel::setLabelFont(QFont font)
{
    m_pLabel->setFont(font);
}

void SettingsLabel::setLabelPalette(DPalette palette)
{
    m_pLabel->setPalette(palette);
}
