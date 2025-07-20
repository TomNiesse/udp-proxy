#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include "udptunnelconnection.h"
#include <QTcpSocket>
#include <QObject>
#include <QEventLoop>

class ClientConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ClientConnection)
public:
    explicit ClientConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
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
    std::unique_ptr<UDPTunnelConnectionSettings> udpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelConnection> udpTunnel;
    QEventLoop eventLoop;
};

#endif // CLIENTCONNECTION_H
