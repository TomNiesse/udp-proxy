#ifndef PROXYTUNNELCLIENT_H
#define PROXYTUNNELCLIENT_H

#include <QObject>
#include <QEventLoop>
#include "udptunnelconnection.h"
#include "tcptunnelclientconnection.h"

class ProxyTunnelClient : public QObject
{
    Q_OBJECT
public:
    explicit ProxyTunnelClient(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, QObject* parent = nullptr);
    ~ProxyTunnelClient();

    const UDPTunnelConnectionSettings getReceiverUdpTunnelConnectionSettings() const;
    QEventLoop& getEventLoop();

private:
    UDPTunnelConnection udpTunnelConnection;
    // TCPTunnelClientConnection tcpTunnelConnection;
    QEventLoop eventLoop;
};

#endif // PROXYTUNNELCLIENT_H
