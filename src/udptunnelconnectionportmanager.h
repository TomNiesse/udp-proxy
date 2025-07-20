#ifndef UDPTUNNELCONNECTIONPORTMANAGER_H
#define UDPTUNNELCONNECTIONPORTMANAGER_H

#include "udptunnelconnectionsettings.h"
#include <QObject>
#include <QMutex>

class UDPTunnelConnectionPortManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelConnectionPortManager)
public:
    UDPTunnelConnectionPortManager(const std::vector<uint16_t>& availablePorts);
    const std::pair<UDPTunnelConnectionSettings, UDPTunnelConnectionSettings> generateUDPTunnelConnectionSettings(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    void freePorts(const std::pair<UDPTunnelConnectionSettings, UDPTunnelConnectionSettings>& udpTunnelConnectionSettings);

    static const std::vector<uint16_t> generatePortList(const uint16_t& start, const uint16_t& end);

private:
    const std::vector<uint16_t> requestPorts();
    void freePorts(const std::vector<uint16_t>& usedPorts);

    std::map<uint16_t, bool> ports;
    QMutex lock;
};

#endif // UDPTUNNELCONNECTIONPORTMANAGER_H
