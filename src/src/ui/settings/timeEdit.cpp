// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timeEdit.h"

#include <QRegExpValidator>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>

CTimeEdit::CTimeEdit(QWidget *parent)
    : DComboBox(parent)
{
    initUI();
    initConnection();
    setFocus(Qt::MouseFocusReason);
}

CTimeEdit::~CTimeEdit() {}

void CTimeEdit::setTime(QTime time)
{
    m_time = time;
    setCurrentText(m_time.toString("hh:mm"));
}

QTime CTimeEdit::getTime()
{
    //获取显示的text
    QString timetext = lineEdit()->displayText();
    //将text转换为时间
    m_time = QTime::fromString(timetext, "hh:mm");
    return m_time;
}

bool CTimeEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit()) {
        if (event->type() == QEvent::FocusIn) {
            emit focusChanged(true);
        } else if (event->type() == QEvent::FocusOut) {
            emit focusChanged(false);
        }
    }
    return DComboBox::eventFilter(watched, event);
}

void CTimeEdit::initUI()
{
    setEditable(true);
    lineEdit()->setClearButtonEnabled(false);
    setMinimumHeight(22);
    QRegExpValidator *validator = nullptr;
    QRegExp rx("0[0-9]:[0-5][0-9]|1[0-9]:[0-5][0-9]|2[0-3]:[0-5][0-9]");
    validator = new QRegExpValidator(rx, this);
    lineEdit()->setValidator(validator);

    QStringList list;

    for (int i = 0; i < 24; i++) {
        list << QString("%1:%2").arg(i, 2, 10, QLatin1Char('0')).arg(0, 2, 10, QLatin1Char('0'));
        list << QString("%1:%2").arg(i, 2, 10, QLatin1Char('0')).arg(30);
    }
    this->addItems(list);
}

void CTimeEdit::initConnection()
{
    connect(this, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onIndexChanged(const QString &)));
    lineEdit()->installEventFilter(this);
}

void CTimeEdit::onIndexChanged(const QString &text)
{
    emit inputFinished(this->currentText());
}
