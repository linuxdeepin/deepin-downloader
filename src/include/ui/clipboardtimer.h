/**
* @file clipboardtimer.h
* @brief 剪切板托管类
* @author bulongwei  <bulongwei@uniontech.com>
* @version 1.0.0
* @date 2020-06-01 16:01
* @copyright 2020-2020 Uniontech Technology Co., Ltd.
*/

#ifndef CLIPBOARDTIMER_H
#define CLIPBOARDTIMER_H
#include <DObject>
#include <QClipboard>
#include <QObject>
#include <QApplication>

class ClipboardTimer : public QObject
{
    Q_OBJECT
public:
    explicit ClipboardTimer(QObject *parent = nullptr);
    ~ClipboardTimer();

private:
    bool isMagnet(QString _str);        //是否是磁力链接
    bool isHttp(QString _str);          //是否是http+后缀

private slots:
    void getDataChanged();              //得到剪切板内容

signals:
    emit void sendClipboardText(QString url); //发送url
private:
    QClipboard *m_clipboard;            //剪切板
};

#endif // CLIPBOARDTIMER_H
