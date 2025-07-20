#include "hostconnectionmanager.h"

HostConnectionManager::HostConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(udpTunnelConnectionSettings);
}

void HostConnectionManager::openConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    const auto& udpTunnelConnectionSettingsBytes = udpTunnelConnectionSettings.encode();
    this->udpTunnelConnection->send(udpTunnelConnectionSettingsBytes);
}

const UDPTunnelConnectionSettings HostConnectionManager::getUdpTunnelConnectionSettings() const
{
    return this->udpTunnelConnection->getUdpTunnelConnectionSettings();
}
