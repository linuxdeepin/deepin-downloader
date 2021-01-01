#include "httpadvancedsettingwidget.h"

#include <DPushButton>
#include <QAction>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "func.h"

HttpAdvancedSettingWidget::HttpAdvancedSettingWidget(DDialog *parent)
    : DDialog(parent)
{
    setFixedSize(475, 345);
    m_configPath = QString("%1/%2/%3/httpAdvanced.json")
                       .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                       .arg(qApp->organizationName())
                       .arg(qApp->applicationName());
    initUI();
}

void HttpAdvancedSettingWidget::initUI()
{
    m_btnBox = new DButtonBox(this);
    QList<DButtonBoxButton*> list;
    DButtonBoxButton *suffixBtn =new DButtonBoxButton(tr("Edit File Extensions"));  //编辑下载文件扩展名
    connect(suffixBtn,&DButtonBoxButton::clicked, this, &HttpAdvancedSettingWidget::onSuffixBtnClicked);
    m_btnBox->setId(suffixBtn, 0);
    DButtonBoxButton *webBtn =new DButtonBoxButton(tr("Edit Unmonitored Sites"));       //辑不接管的网站
    connect(webBtn,&DButtonBoxButton::clicked, this, &HttpAdvancedSettingWidget::onWebBtnClicked);
    m_btnBox->setId(webBtn, 1);
    list<<suffixBtn<< webBtn;
    m_btnBox->setButtonList(list,true);
    m_btnBox->setFixedSize(280,36);
    addContent(m_btnBox, Qt::AlignHCenter);

    m_textEdit = new DTextEdit(this);
    m_textEdit->setFixedSize(454,168);
    addContent(m_textEdit, Qt::AlignHCenter);

    m_defaultBtn = new DCommandLinkButton(tr("恢复默认"), this);
    connect(m_defaultBtn,&DCommandLinkButton::clicked, this, &HttpAdvancedSettingWidget::onRstoreDefaultClicked);
    addContent(m_defaultBtn,  Qt::AlignRight);

    QWidget *w = new QWidget(this);
    DPushButton * cancel = new DPushButton(this);
    connect(cancel,&DPushButton::clicked, this, &HttpAdvancedSettingWidget::onCancelClicked);
    cancel->setText(tr("取消"));
    cancel->setFixedSize(216,36);
    DPushButton * sure = new DPushButton(this);
    connect(sure,&DPushButton::clicked, this, &HttpAdvancedSettingWidget::onSureClicked);
    sure->setFixedSize(216,36);
    sure->setText(tr("确认"));
    QHBoxLayout * hblyt = new QHBoxLayout();
    hblyt->addWidget(cancel);
    hblyt->addWidget(sure);
    w->setLayout(hblyt);
    addContent(w, Qt::AlignHCenter);

    suffixBtn->click();
}

void HttpAdvancedSettingWidget::onSuffixBtnClicked()
{
    QFile file(m_configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString curSuffix = obj.value("CurSuffix").toString();
    m_textEdit->setPlaceholderText(tr("Separate file extensions by semicolons (;)"));   //请输入正确的文件扩展名，以;分隔
    m_textEdit->setText(curSuffix);
    m_defaultBtn->show();
}
void HttpAdvancedSettingWidget::onWebBtnClicked()
{
    QFile file(m_configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    QString curWeb = obj.value("CurWeb").toString();
    m_textEdit->setPlaceholderText(tr("Please enter one URL per line"));        //添加多个网站链接时，请确保每行只有一个链接
    m_textEdit->setText(curWeb);
    m_defaultBtn->hide();
}

void HttpAdvancedSettingWidget::onRstoreDefaultClicked()
{
    QFile file(m_configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    QString defaultSuffix = obj.value("DefaultSuffix").toString();
    m_textEdit->setText(defaultSuffix);
}

void HttpAdvancedSettingWidget::onSureClicked()
{
    QFile file(m_configPath);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    QString value = m_textEdit->toPlainText();
    if(m_btnBox->buttonList()[0]->isChecked()){
        obj["CurSuffix"] = value;
    }else {
        obj["CurWeb"] = value;
    }
    jdc.setObject(obj);
    file.resize(0);
    file.write(jdc.toJson());
    file.close();
    file.flush();
    close();
}

void HttpAdvancedSettingWidget::onCancelClicked()
{
    close();
}

























