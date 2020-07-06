/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file filesavepathchooser.cpp
 *
 * @brief 下载目录窗口，主要实现切换单选按钮时，文件保存路径控件的使能以及设置数据的传递与显示
 *
 *@date 2020-06-09 11:00
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
#include "filesavepathchooser.h"
#include "messagebox.h"

#include <DSwitchButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDebug>
#include <QStandardPaths>

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

    if(m_nCurrentSelect == 1)
    {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);
    }
    else
    {
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
    connect(m_pAutoLastPathRadioButton,SIGNAL(clicked()),this,SLOT(radioButtonClickSlot()));
    connect(m_pCustomsPathRadioButton,SIGNAL(clicked()),this,SLOT(radioButtonClickSlot()));
    connect(m_pFileChooserEdit,SIGNAL(textChanged(const QString &)),this,SLOT(lineEditTextChanged(const QString &)));
}

void FileSavePathChooser::radioButtonClickSlot()
{
    DRadioButton *pRadioButton = qobject_cast<DRadioButton *>(sender());
    if(m_pAutoLastPathRadioButton == pRadioButton)
    {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);

        QString strText = "auto;" + m_pFileChooserEdit->text();
        emit textChanged(strText);
    }
    else if(m_pCustomsPathRadioButton == pRadioButton)
    {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);

        QString strText = "custom;" + m_pFileChooserEdit->text();
        emit textChanged(strText);
    }
}

void FileSavePathChooser::lineEditTextChanged(const QString &strText)
{
    QFileInfo fileinfo;
    fileinfo.setFile(strText);
    if(!fileinfo.isWritable())
    {
        MessageBox *msg=new MessageBox();
        QString title = tr("Permission denied. Please try other folder.");
        msg->setWarings(title, tr("sure"));
        m_pFileChooserEdit->setText(m_strDownloadPath);
        msg->exec();
    }
    else {
        QString strChangedText = "custom;" + strText;
        emit textChanged(strChangedText);
    }
}

void FileSavePathChooser::setLineEditText(const QString &strText)
{
    m_strDownloadPath = strText;
    m_pFileChooserEdit->setText(strText);
}

void FileSavePathChooser::setCurrentSelectRadioButton(int nCurrentSelect)
{
    if(nCurrentSelect == 1)
    {
        m_pAutoLastPathRadioButton->setChecked(true);
        m_pCustomsPathRadioButton->setChecked(false);
        m_pFileChooserEdit->setDisabled(true);
    }
    else
    {
        m_pAutoLastPathRadioButton->setChecked(false);
        m_pCustomsPathRadioButton->setChecked(true);
        m_pFileChooserEdit->setDisabled(false);
    }
}
