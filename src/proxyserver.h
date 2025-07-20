#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include "udptunnelconnection.h"
#include "udptunnelconnectionportmanager.h"
#include "hostconnectionmanager.h"
#include <QObject>
#include <QTcpServer>

typedef enum ProxyConnectionState
{
    WAITING,
    COMMUNICATION,
    STOP_CLIENT
} ProxyConnectionState;

class ProxyServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProxyServer)
public:
    ProxyServer(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);

private slots:
    void incomingConnection(const qintptr socketDescriptor) override;

private:
    void communicationThread(const qintptr socketDescriptor, const UDPTunnelConnectionSettings udpTunnelConnectionSettings);

    std::unique_ptr<UDPTunnelConnectionPortManager> portManager;
    std::unique_ptr<HostConnectionManager> hostConnectionManager;
};

#endif // PROXYSERVER_H
