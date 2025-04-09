#ifndef TCPPROXYCONNECTIONMANAGER_H
#define TCPPROXYCONNECTIONMANAGER_H

#include "udptunnelconnection.h"
#include <vector>
#include <QMutex>

class TCPProxyConnectionManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TCPProxyConnectionManager)
public:
    explicit TCPProxyConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    ~TCPProxyConnectionManager();

public slots:
    void connectToHost(const size_t& connectionId, const QByteArray& host, const quint16& port);
    void write(const size_t& connectionId, const QByteArray& payload);
    void disconnect(const size_t& connectionId);

signals:
    void connected(const size_t& connectionId);
    void disconnected(const size_t& connectionId);
    void bytesWritten(const size_t& connectionId);
    void bytesReceived(const size_t& connectionId, const QByteArray& payload);

private:
    void handleReceivedBytes(const QByteArray& packet);
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
    QMutex lock;
};

#endif // TCPPROXYCONNECTIONMANAGER_H
