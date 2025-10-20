// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timeEdit.h"

#if QT_VERSION_MAJOR <= 5
#include <QRegExpValidator>
#else
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#endif
#include <QDebug>

CTimeEdit::CTimeEdit(QWidget *parent)
    : DComboBox(parent)
{
    qDebug() << "TimeEdit widget created";
    initUI();
    initConnection();
    setFocus(Qt::MouseFocusReason);
    qDebug() << "[TimeEdit] Constructor ended";
}

CTimeEdit::~CTimeEdit() 
{
    // qDebug() << "TimeEdit widget destroyed";
}

void CTimeEdit::setTime(QTime time)
{
    qDebug() << "Setting time to:" << time.toString("hh:mm");
    m_time = time;
    QString sss = m_time.toString("hh:mm");
    m_timeEdit->setText(m_time.toString("hh:mm"));
}

QTime CTimeEdit::getTime()
{
    qDebug() << "[TimeEdit] getTime function started";
    //获取显示的text
    QString timetext = m_timeEdit->lineEdit()->displayText();
    //将text转换为时间
    m_time = QTime::fromString(timetext, "hh:mm");
    qDebug() << "Getting current time:" << m_time.toString("hh:mm");
    return m_time;
}

DLineEdit *CTimeEdit::dLineEdit()
{
    // qDebug() << "[TimeEdit] dLineEdit function started";
    return m_timeEdit;
}

void CTimeEdit::initUI()
{
    // qDebug() << "[TimeEdit] initUI function started";
    m_timeEdit = new DLineEdit(this);
    //m_timeEdit->lineEdit()->setInputMask("00:00;0");
    m_timeEdit->setClearButtonEnabled(false);
    m_timeEdit->setMinimumHeight(22);
#if QT_VERSION_MAJOR > 5
    QRegularExpressionValidator *validator = nullptr;
    QRegularExpression rx("0[0-9]:[0-5][0-9]|1[0-9]:[0-5][0-9]|2[0-3]:[0-5][0-9]");
    validator = new QRegularExpressionValidator(rx, this);
#else
    QRegExpValidator *validator = nullptr;
    QRegExp rx("0[0-9]:[0-5][0-9]|1[0-9]:[0-5][0-9]|2[0-3]:[0-5][0-9]");
    validator = new QRegExpValidator(rx, this);
#endif
    m_timeEdit->lineEdit()->setValidator(validator);
    setLineEdit(m_timeEdit->lineEdit());

    QStringList list;

    for (int i = 0; i < 24; i++) {
        list << QString("%1:%2").arg(i, 2, 10, QLatin1Char('0')).arg(0, 2, 10, QLatin1Char('0'));
        list << QString("%1:%2").arg(i, 2, 10, QLatin1Char('0')).arg(30);
    }
    this->addItems(list);
}

void CTimeEdit::initConnection()
{
    // qDebug() << "[TimeEdit] initConnection function started";
//    m_timeEdit->disconnect(SIGNAL(returnPressed()));
//    m_timeEdit->disconnect(SIGNAL(editingFinished()));
//    m_timeEdit->disconnect(SIGNAL(selectionChanged()));
//    m_timeEdit->disconnect(SIGNAL(textChanged(const QString &)));
//    m_timeEdit->disconnect(SIGNAL(textEdited(const QString &)));
//    m_timeEdit->disconnect(SIGNAL(cursorPositionChanged(int, int)));

    //disconnect(SIGNAL(activated(int)));
    //disconnect(SIGNAL(activated(const QString &)));
    //disconnect(SIGNAL(currentIndexChanged(int)));
    //disconnect(SIGNAL(currentIndexChanged(const QString &)));
    //disconnect(SIGNAL(currentTextChanged(const QString &)));
    //disconnect(SIGNAL(editTextChanged(const QString &)));
    //disconnect(SIGNAL(highlighted(int)));
    //disconnect(SIGNAL(highlighted(const QString &)));
    connect(this, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onIndexChanged(const QString &)));
    connect(m_timeEdit, &DLineEdit::editingFinished, this, [=](){
        QString a = this->currentText();
        QString b = m_timeEdit->text();
        QString c = m_timeEdit->lineEdit()->text();
       // emit inputFinished(this->currentText());
    });
    // qDebug() << "[TimeEdit] initConnection function ended";
}

void CTimeEdit::onIndexChanged(const QString &text)
{
    // qDebug() << "Time selection changed to:" << text;
    m_timeEdit->setText(text);

    QString a = this->currentText();
    QString b = m_timeEdit->text();
    QString c = m_timeEdit->lineEdit()->text();
    emit inputFinished(this->currentText());
}
