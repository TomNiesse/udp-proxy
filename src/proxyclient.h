#ifndef PROXYCLIENT_H
#define PROXYCLIENT_H

#include "tcpclient.h"
#include "udptunnelconnection.h"

class ProxyClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProxyClient)
public:
    explicit ProxyClient(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    ~ProxyClient();

private:
    TCPClient tcpClient;
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
};

#endif // PROXYCLIENT_H
