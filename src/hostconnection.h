#ifndef HOSTCONNECTION_H
#define HOSTCONNECTION_H

#include "udptunnelconnection.h"
#include <QTcpSocket>
#include <QEventLoop>
#include <QObject>

class HostConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(HostConnection)
public:
    explicit HostConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    ~HostConnection();
    void connectToHost(const QString& host, const quint16& port);
    void send(const QByteArray& data);
    void disconnect();
    void quit();

signals:
    void clientIsConnected();
    void receivedData(const QByteArray& data);
    void clientWasDisconnected();
    void clientHasQuit();

private:
    void handleUdpTunnelPacket(const QByteArray& data);
    void handleTcpConnect();
    void handleTcpData(const QByteArray& data);
    void handleTcpDisconnect();
    void handleClientQuit();

    QTcpSocket tcpSocket;
    std::unique_ptr<UDPTunnelConnectionSettings> udpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
};

#endif // HOSTCONNECTION_H
