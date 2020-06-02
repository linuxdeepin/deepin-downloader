/**
* @file filesavepathchooser.cpp
* @brief 下载目录窗口，主要实现切换单选按钮时，文件保存路径控件的使能以及设置数据的传递与显示
* @author yuandandan  <yuandandan@uniontech.com>
* @version 1.0.0
* @date 2020-05-26 15:34
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/
#include "filesavepathchooser.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <DSwitchButton>
#include "dswitchbutton.h"

FileSavePathChooser::FileSavePathChooser(int nCurrentSelect, const QString &strDownloadPath)
{
    m_nCurrentSelect = nCurrentSelect;
    m_strDownloadPath = strDownloadPath;

    initUI();
    initConnections();
}

void FileSavePathChooser::initUI()
{
    m_pFileChooserEdit = new DFileChooserEdit;
    m_pAutoLastPathRadioButton = new DRadioButton(tr("Automatically changes to the last used directory")); // 自动修改为上次使用的目录
    m_pCustomsPathRadioButton = new DRadioButton(tr("Default directory")); // 设置默认目录
    m_pFileChooserEdit->lineEdit()->setReadOnly(true);
    m_pFileChooserEdit->lineEdit()->setClearButtonEnabled(false);
    m_pFileChooserEdit->setFileMode(QFileDialog::FileMode::DirectoryOnly);

    if (m_nCurrentSelect == 1) {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);
    } else {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);
    }

    m_pFileChooserEdit->setText(m_strDownloadPath);

    QHBoxLayout *pFileChooserLayout = new QHBoxLayout;
    pFileChooserLayout->addWidget(m_pFileChooserEdit);
    pFileChooserLayout->setContentsMargins(28, 0, 0, 0);

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    pMainLayout->addWidget(m_pCustomsPathRadioButton);
    pMainLayout->addLayout(pFileChooserLayout);
    pMainLayout->addWidget(m_pAutoLastPathRadioButton);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    //    setStyleSheet("background:rgba(249, 249, 249, 1)");

    setLayout(pMainLayout);
}

void FileSavePathChooser::initConnections()
{
    connect(m_pAutoLastPathRadioButton, SIGNAL(clicked()), this, SLOT(radioButtonClickSlot()));
    connect(m_pCustomsPathRadioButton, SIGNAL(clicked()), this, SLOT(radioButtonClickSlot()));
    connect(m_pFileChooserEdit, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditTextChanged(const QString &)));
}

void FileSavePathChooser::radioButtonClickSlot()
{
    DRadioButton *pRadioButton = qobject_cast<DRadioButton *>(sender());
    if (m_pAutoLastPathRadioButton == pRadioButton) {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);

        QString strText = "auto;" + m_pFileChooserEdit->text();
        emit textChanged(strText);
    } else if (m_pCustomsPathRadioButton == pRadioButton) {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);

        QString strText = "custom;" + m_pFileChooserEdit->text();
        emit textChanged(strText);
    }
}

void FileSavePathChooser::lineEditTextChanged(const QString &strText)
{
    QString strChangedText = "custom;" + strText;
    emit textChanged(strChangedText);
}

void FileSavePathChooser::setLineEditText(const QString &strText)
{
    m_pFileChooserEdit->setText(strText);
}

void FileSavePathChooser::setCurrentSelectRadioButton(int nCurrentSelect)
{
    if (nCurrentSelect == 1) {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);
    } else {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);
    }
}
