// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef QT_EVENT_HELPER_H
#define QT_EVENT_HELPER_H

#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QDragEnterEvent>
#include <QPoint>

// Qt6.4+ 中 QMouseEvent 旧构造函数已废弃，统一使用新构造函数
inline QMouseEvent *createMouseEvent(QEvent::Type type, const QPoint &pos,
                                     Qt::MouseButton button, Qt::MouseButtons buttons,
                                     Qt::KeyboardModifiers modifiers)
{
#if QT_VERSION_MAJOR > 5
    // Qt6: 使用带 globalPos 参数的非废弃构造函数
    return new QMouseEvent(type, pos, pos, button, buttons, modifiers);
#else
    return new QMouseEvent(type, pos, button, buttons, modifiers);
#endif
}

#endif // QT_EVENT_HELPER_H
