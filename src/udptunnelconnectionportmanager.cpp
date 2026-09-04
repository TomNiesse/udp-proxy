#include "udptunnelconnectionportmanager.h"
#include <QDebug>

UDPTunnelConnectionPortManager::UDPTunnelConnectionPortManager(const std::vector<uint16_t>& availablePorts)
{
    const QMutexLocker lock(&this->lock);

    for(const auto& availablePort : availablePorts)
    {
        const bool& portAvailable = true;
        this->ports.emplace(availablePort, portAvailable);
    }
}

const std::pair<UDPTunnelConnectionSettings, UDPTunnelConnectionSettings> UDPTunnelConnectionPortManager::generateUDPTunnelConnectionSettings(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    const QMutexLocker lock(&this->lock);

    const auto& usedPorts = this->requestPorts();

    UDPTunnelConnectionSettings hostUdpTunnelConnectionSettings(udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.getEgressPort());
    UDPTunnelConnectionSettings clientUdpTunnelConnectionSettings(udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.getEgressPort());

    hostUdpTunnelConnectionSettings.setIngressPort(usedPorts.at(0));
    hostUdpTunnelConnectionSettings.setEgressPort(usedPorts.at(1));

    clientUdpTunnelConnectionSettings.setIngressPort(usedPorts.at(1));
    clientUdpTunnelConnectionSettings.setEgressPort(usedPorts.at(0));

    return std::make_pair(hostUdpTunnelConnectionSettings, clientUdpTunnelConnectionSettings);
}

void UDPTunnelConnectionPortManager::freePorts(const std::pair<UDPTunnelConnectionSettings, UDPTunnelConnectionSettings>& udpTunnelConnectionSettings)
{
    const QMutexLocker lock(&this->lock);

    std::vector<uint16_t> usedPorts;

    usedPorts.push_back(udpTunnelConnectionSettings.first.getIngressPort());
    usedPorts.push_back(udpTunnelConnectionSettings.first.getEgressPort());

    this->freePorts(usedPorts);
}

const std::vector<uint16_t> UDPTunnelConnectionPortManager::generatePortList(const uint16_t& start, const uint16_t& end)
{
    std::vector<uint16_t> ports;

    for(uint16_t port = start; port <= end; port++)
    {
        ports.push_back(port);
    }

    return ports;
}

// Private

// TODO: check if a port is not used by other processes before allocating it
const std::vector<uint16_t> UDPTunnelConnectionPortManager::requestPorts()
{
    std::vector<uint16_t> usedPorts;

    for(auto& port : this->ports)
    {
        const auto& portNumber = port.first;
        auto& portAvailable = port.second;

        if(portAvailable)
        {
            usedPorts.push_back(portNumber);
            portAvailable = false;
        }

        if(usedPorts.size() == 2)
        {
            break;
        }
    }

    if(usedPorts.size() < 2)
    {
        qDebug().noquote() << "Cannot reserve enough ports. Exiting.";
        exit(EXIT_FAILURE);
    }

    return usedPorts;
}

void UDPTunnelConnectionPortManager::freePorts(const std::vector<uint16_t>& usedPorts)
{
    for(const auto& usedPort : usedPorts)
    {
        auto itr = std::find_if(this->ports.begin(), this->ports.end(), [this, usedPort](std::pair<uint16_t, bool> port){
            return usedPort == port.first;
        });
        itr->second = true;
    }
}
