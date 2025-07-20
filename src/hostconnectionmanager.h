#ifndef HOSTCONNECTIONMANAGER_H
#define HOSTCONNECTIONMANAGER_H

#include "udptunnelconnection.h"
#include <QMutex>

class HostConnectionManager
{
public:
    HostConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    void openConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    const UDPTunnelConnectionSettings getUdpTunnelConnectionSettings() const;

private:
    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
};

#endif // HOSTCONNECTIONMANAGER_H
