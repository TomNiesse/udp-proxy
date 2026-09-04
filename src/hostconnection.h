#ifndef PROXYTUNNELHOSTCONNECTION_H
#define PROXYTUNNELHOSTCONNECTION_H

#include "udptunnelconnection.h"
#include <QTcpSocket>
#include <QEventLoop>
#include <QObject>

class ProxyTunnelHostConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProxyTunnelHostConnection)
public:
    explicit ProxyTunnelHostConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings);
    ~ProxyTunnelHostConnection();
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
    UDPTunnelConnectionSettings senderUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings receiverUdpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
};

#endif // PROXYTUNNELHOSTCONNECTION_H
