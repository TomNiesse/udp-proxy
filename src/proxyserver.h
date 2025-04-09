#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <QObject>
#include <QTcpServer>
#include "tcpproxyconnectionmanager.h"

class ProxyServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProxyServer)
public:
    explicit ProxyServer(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
public slots:
    void incomingConnection(const qintptr socketDescriptor) override;
private:
    size_t connectionId = 0;
    QMutex lock;
    std::unique_ptr<TCPProxyConnectionManager> tcpConnectionManager;
};

#endif // PROXYSERVER_H
