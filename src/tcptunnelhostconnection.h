#ifndef TCPTUNNELHOSTCONNECTION_H
#define TCPTUNNELHOSTCONNECTION_H

#include <QObject>
#include <QMutex>
#include "tcptunnelpacket.h"
#include "tcptunnelconnectionsettings.h"
#include "udptunnelconnection.h"

class TCPTunnelHostConnection : public QObject
{
    Q_OBJECT
public:
    explicit TCPTunnelHostConnection(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject *parent = nullptr);
    ~TCPTunnelHostConnection();

    void connect(const QByteArray& host, const int& port);
    void disconnect();
    void write(const QByteArray& data);

    const TCPTunnelConnectionSettings getTcpTunnelConnectionSettings() const;
    void setTcpTunnelConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings);

private slots:
    void handleTcpTunnelPacket(const QByteArray data);

signals:
    void writeTcpTunnelPacket(const QByteArray tcpTunnelPacket);
    void connected();
    void disconnected();
    void bytesWritten();
    void receivedData(const QByteArray receivedData);
    void connectionTimeout();

private slots:
    void handleConnected();
    void handleDisconnected();
    void handleBytesWritten();
    void handleReceivedData(const QByteArray& receivedData);
    void handleConnectionTimeout();

private:
    mutable QMutex lock;

    TCPTunnelConnectionSettings tcpTunnelConnectionSettings;
    UDPTunnelConnection udpTunnelConnection;
};

#endif // TCPTUNNELHOSTCONNECTION_H
