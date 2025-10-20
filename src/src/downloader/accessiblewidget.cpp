// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accessiblewidget.h"
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

AccessibleWidget::AccessibleWidget(QWidget *widget)
    : QAccessibleWidget(widget)
    , m_widget(widget)
{
    // qDebug() << "[Accessible] Widget initialized for:" << widget->objectName();
}
AccessibleWidget::~AccessibleWidget()
{
    // qDebug() << "[Accessible] Widget destroyed for:" << m_widget->objectName();
}

void *AccessibleWidget::interface_cast(QAccessible::InterfaceType t)
{
    // qDebug() << "[Accessible] Interface cast requested for type:" << t;

    switch (t) {
    case QAccessible::ActionInterface:
        return static_cast<QAccessibleActionInterface *>(this);
    case QAccessible::TextInterface:
        return static_cast<QAccessibleTextInterface *>(this);
    default:
        return nullptr;
    }
}

QString AccessibleWidget::text(QAccessible::Text t) const
{
    // qDebug() << "[Accessible] Text requested for type:" << t;

    switch (t) {
    case QAccessible::Name:
        return m_widget->accessibleName();
    case QAccessible::Description:
        return m_widget->accessibleDescription();
    default:
        return QString();
    }
}

QString AccessibleWidget::text(int startOffset, int endOffset) const
{
    // qDebug() << "[Accessible] text(offset) function started";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

void AccessibleWidget::selection(int selectionIndex, int *startOffset, int *endOffset) const
{
    // qDebug() << "[Accessible] selection function started";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::selectionCount() const
{
    // qDebug() << "[Accessible] selectionCount function started";
    return 0;
}

void AccessibleWidget::addSelection(int startOffset, int endOffset)
{
    // qDebug() << "[Accessible] addSelection function started";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

void AccessibleWidget::removeSelection(int selectionIndex)
{
    // qDebug() << "[Accessible] removeSelection function started";
    Q_UNUSED(selectionIndex)
}

void AccessibleWidget::setSelection(int selectionIndex, int startOffset, int endOffset)
{
    // qDebug() << "[Accessible] setSelection function started";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::cursorPosition() const
{
    // qDebug() << "[Accessible] cursorPosition function started";
    return 0;
}

void AccessibleWidget::setCursorPosition(int position)
{
    // qDebug() << "[Accessible] setCursorPosition function started";
    Q_UNUSED(position)
}

int AccessibleWidget::characterCount() const
{
    // qDebug() << "[Accessible] characterCount function started";
    return 0;
}

QRect AccessibleWidget::characterRect(int offset) const
{
    // qDebug() << "[Accessible] characterRect function started";
    Q_UNUSED(offset)

    return QRect();
}

int AccessibleWidget::offsetAtPoint(const QPoint &point) const
{
    // qDebug() << "[Accessible] offsetAtPoint function started";
    Q_UNUSED(point)

    return 0;
}

void AccessibleWidget::scrollToSubstring(int startIndex, int endIndex)
{
    // qDebug() << "[Accessible] scrollToSubstring function started";
    Q_UNUSED(startIndex)
    Q_UNUSED(endIndex)
}

QString AccessibleWidget::attributes(int offset, int *startOffset, int *endOffset) const
{
    // qDebug() << "[Accessible] attributes function started";
    Q_UNUSED(offset)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

AccessibleLabel::AccessibleLabel(QLabel *label)
    : AccessibleWidget(label)
    , m_label(label)
{
    // qDebug() << "[Accessible] Label initialized for:" << label->text();
}

AccessibleLabel::~AccessibleLabel()
{
    // qDebug() << "[Accessible] Label destroyed for:" << m_label->text();
}

QString AccessibleLabel::text(int startOffset, int endOffset) const
{
    // qDebug() << "[Accessible] Label text requested";

    if (Q_NULLPTR != m_label) {
        // qDebug() << "[Accessible] Label is not null, returning text";
        return m_label->text();
    }

    // qDebug() << "[Accessible] Label is null, calling parent function";
    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleButton::AccessibleButton(QPushButton *button)
    : AccessibleWidget(button)
    , m_button(button)
{
    // qDebug() << "[Accessible] Button initialized for:" << button->text();
}
AccessibleButton::~AccessibleButton()
{
    // qDebug() << "[Accessible] Button destroyed for:" << m_button->text();
}

QString AccessibleButton::text(int startOffset, int endOffset) const
{
    // qDebug() << "[Accessible] Button text function started";

    if (Q_NULLPTR != m_button) {
        // qDebug() << "[Accessible] Button is not null, returning text";
        return m_button->text();
    }

    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleCheckBox::AccessibleCheckBox(QCheckBox *checkbox)
    : AccessibleWidget(checkbox)
    , m_checkbox(checkbox)
{
    // qDebug() << "[Accessible] CheckBox initialized for:" << checkbox->text();
}

AccessibleCheckBox::~AccessibleCheckBox()
{
    // qDebug() << "[Accessible] CheckBox destroyed for:" << m_checkbox->text();
}

QString AccessibleCheckBox::text(int startOffset, int endOffset) const
{
    // qDebug() << "[Accessible] CheckBox text function started";

    if (Q_NULLPTR != m_checkbox) {
        // qDebug() << "[Accessible] CheckBox is not null, returning text";
        return m_checkbox->text();
    }

    // qDebug() << "[Accessible] CheckBox is null, calling parent function";
    return AccessibleWidget::text(startOffset, endOffset);
}

Qt::CheckState AccessibleCheckBox::checkState(int startOffset, int endOffset) const
{
    // qDebug() << "[Accessible] CheckBox checkState function started";

    if (Q_NULLPTR != m_checkbox) {
        // qDebug() << "[Accessible] CheckBox is not null, returning check state";
        return m_checkbox->checkState();
    }

    // qDebug() << "[Accessible] CheckBox is null, returning Unchecked";
    return Qt::Unchecked;
}
