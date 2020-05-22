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
    bool isMagnet(QString _str);
    bool isHttp(QString _str);

private slots:
    void getDataChanged();

signals:
    emit void sendClipboardText(QString url);
private:
    QClipboard *m_clipboard;
};

#endif // CLIPBOARDTIMER_H
