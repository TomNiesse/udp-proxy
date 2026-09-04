#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include "udptunnelconnection.h"
#include <QTcpSocket>
#include <QObject>
#include <QEventLoop>

// TODO: remove this file(?)

class ClientConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ClientConnection)
public:
    explicit ClientConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings);
    ~ClientConnection();
    QEventLoop& getEventLoop();

protected:
    void handleUdpTunnelPacket(const QByteArray& data);
    void handleTcpConnect();
    void handleTcpData();
    void handleTcpDisconnect();
    void handleClientQuit();

private:
    void connectToHost(const QString& host, const quint16& port);
    void write(const QByteArray& data);
    void disconnect();
    void quit();

    QTcpSocket tcpSocket;
    UDPTunnelConnectionSettings senderUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings receiverUdpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
    QEventLoop eventLoop;
};

#endif // CLIENTCONNECTION_H
