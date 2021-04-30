#ifndef EXTENSIONSERVICE_H
#define EXTENSIONSERVICE_H

#include <QObject>
class QWebSocketServer;
class QTimer;

class extensionService : public QObject
{
    Q_OBJECT
public:
    extensionService();
    ~extensionService();

private:
    void initWebsokcet();
    void sendUrlToDownloader(const QString& url);

private slots:
    void checkConnection();
    void freeSharedMem();

private:
    QWebSocketServer* m_server;
    QTimer *m_timer;
};

#endif // EXTENSIONSERVICE_H
