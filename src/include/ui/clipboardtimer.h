/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file clipboardtimer.h
 *
 * @brief 剪切板托管类
 *
 * @date 2020-06-09 10:51
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
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

#ifndef CLIPBOARDTIMER_H
#define CLIPBOARDTIMER_H
#include "settings.h"
#include <DObject>
#include <QObject>
#include <QApplication>
#include <QSystemTrayIcon>

class QClipboard;
class QMimeData;


class ClipboardTimer : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardTimer(QObject *parent = nullptr);
    ~ClipboardTimer();

private:
    bool isMagnet(QString str);        //是否是磁力链接
    bool isHttp(QString str);          //是否是http+后缀
    bool isBt(QString str);             //是否是 http + bt后缀


private slots:
    void getDataChanged();              //得到剪切板内容

signals:
    emit void sendClipboardTextChange(QString url); //发送url
private:
    QClipboard *m_clipboard;            //剪切板
    QByteArray m_timeStamp;
    bool m_isHttp;
    bool m_isMagnet;
};

#endif // CLIPBOARDTIMER_H
