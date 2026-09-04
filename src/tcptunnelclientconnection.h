#ifndef TCPTUNNELCLIENTCONNECTION_H
#define TCPTUNNELCLIENTCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMutex>
#include "tcptunnelpacket.h"
#include "tcptunnelconnectionsettings.h"
#include "udptunnelconnection.h"

class TCPTunnelClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit TCPTunnelClientConnection(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject *parent = nullptr);
    ~TCPTunnelClientConnection();

    const TCPTunnelConnectionSettings getTcpTunnelConnectionSettings() const;
    void setTcpConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings);

    QEventLoop& getEventLoop();

public slots:
    void handleTcpTunnelPacket(const QByteArray data);

signals:
    void writeTcpTunnelPacket(const QByteArray tcpTunnelPacket);

private slots:
    void handleConnected();
    void handleDisconnected();
    void handleBytesWritten();
    void handleBytesAvailable();
    void handleConnectionTimeout();

private:
    void connect(const QByteArray& host, const int& port, const int& connectionTimeout = (10 * 1000));
    void disconnect();
    void write(const QByteArray& data);

    static const bool socketConnected(const QTcpSocket& socket);
    void startConnectionTimeoutTimer(const int& interval);
    void stopConnectionTimeoutTimer();

    QMutex lock;
    QTcpSocket socket;
    QTimer connectionTimeoutTimer;

    TCPTunnelConnectionSettings tcpTunnelConnectionSettings;
    UDPTunnelConnection udpTunnelConnection;

    QEventLoop eventLoop;
};

#endif // TCPTUNNELCLIENTCONNECTION_H
