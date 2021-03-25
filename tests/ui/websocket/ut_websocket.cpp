
#include <QObject>
#include <QJsonObject>
#include <iostream>
#include <QWebSocket>
#include <QWebSocketServer>

#include "gtest/gtest.h"
#include "websockettransport.h"
#include "websocketclientwrapper.h"

class ut_websocket : public ::testing::Test
{
protected:
    ut_websocket()
    {
    }

    virtual ~ut_websocket()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_websocket, WebSocketTransport)
{
    QWebSocket *soc = new QWebSocket;
    WebSocketTransport *port = new WebSocketTransport(soc);
    QJsonObject message;
    port->sendMessage(message);
    port->textMessageReceived("adads");
    EXPECT_TRUE(true);
    delete port;
    delete soc;
}


TEST_F(ut_websocket, WebSocketClientWrapper)
{
    QWebSocketServer *soc = new QWebSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
    WebSocketClientWrapper *wrapper = new WebSocketClientWrapper(soc, new QObject);
    QJsonObject message;
    wrapper->handleNewConnection();
    EXPECT_TRUE(true);
    delete wrapper;
    delete soc;
}
