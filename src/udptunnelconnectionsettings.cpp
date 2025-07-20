#include "udptunnelconnectionsettings.h"

UDPTunnelConnectionSettings::UDPTunnelConnectionSettings()
{

}

UDPTunnelConnectionSettings::UDPTunnelConnectionSettings(const QByteArray& ingressAddress, const uint16_t& ingressPort, const QByteArray& egressAddress, const uint16_t& egressPort)
{
    this->ingressAddress = ingressAddress;
    this->ingressPort = ingressPort;
    this->egressAddress = egressAddress;
    this->egressPort = egressPort;
}

UDPTunnelConnectionSettings::UDPTunnelConnectionSettings(const QByteArray& data)
{
    const auto& decodedData = UDPTunnelConnectionSettings::decode(data);
    this->ingressAddress = std::get<0>(decodedData);
    this->ingressPort = std::get<1>(decodedData);
    this->egressAddress = std::get<2>(decodedData);
    this->egressPort = std::get<3>(decodedData);
}

const QByteArray UDPTunnelConnectionSettings::encode() const
{
    return UDPTunnelConnectionSettings::encode(this->ingressAddress, this->ingressPort, this->egressAddress, this->egressPort);
}

const QByteArray UDPTunnelConnectionSettings::getIngressAddress() const
{
    return this->ingressAddress;
}

void UDPTunnelConnectionSettings::setIngressAddress(const QByteArray& ingressAddress)
{
    this->ingressAddress = ingressAddress;
}

uint16_t UDPTunnelConnectionSettings::getIngressPort() const
{
    return this->ingressPort;
}

void UDPTunnelConnectionSettings::setIngressPort(const uint16_t& ingressPort)
{
    this->ingressPort = ingressPort;
}

const QByteArray UDPTunnelConnectionSettings::getEgressAddress() const
{
    return this->egressAddress;
}

void UDPTunnelConnectionSettings::setEgressAddress(const QByteArray& egressAddress)
{
    this->egressAddress = egressAddress;
}

uint16_t UDPTunnelConnectionSettings::getEgressPort() const
{
    return this->egressPort;
}

void UDPTunnelConnectionSettings::setEgressPort(const uint16_t& egressPort)
{
    this->egressPort = egressPort;
}

// Private

const QByteArray UDPTunnelConnectionSettings::encode(const QByteArray& ingressAddress, const uint16_t& ingressPort, const QByteArray& egressAddress, const uint16_t& egressPort)
{
    QByteArray bytes;

    size_t ingressAddressSize = ingressAddress.size();
    char ingressAddressSizeBytes[sizeof(ingressAddressSize)] = {0};
    memcpy(ingressAddressSizeBytes, &ingressAddressSize, sizeof(ingressAddressSize));
    bytes.append(ingressAddressSizeBytes, sizeof(ingressAddressSize));

    bytes.append(ingressAddress);

    char ingressPortBytes[sizeof(ingressPort)] = {0};
    memcpy(ingressPortBytes, &ingressPort, sizeof(ingressPort));
    bytes.append(ingressPortBytes, sizeof(ingressPort));

    size_t egressAddressSize = egressAddress.size();
    char egressAddressSizeBytes[sizeof(egressAddressSize)] = {0};
    memcpy(egressAddressSizeBytes, &egressAddressSize, sizeof(egressAddressSize));
    bytes.append(egressAddressSizeBytes, sizeof(egressAddressSize));

    bytes.append(egressAddress);

    char egressPortBytes[sizeof(egressPort)] = {0};
    memcpy(egressPortBytes, &egressPort, sizeof(egressPort));
    bytes.append(egressPortBytes, sizeof(egressPort));

    return bytes;
}

const std::tuple<QByteArray, uint16_t, QByteArray, uint16_t> UDPTunnelConnectionSettings::decode(const QByteArray data)
{
    size_t ingressAddressSize = 0;
    const auto& ingressAddressSizeBytes = data.mid(0, sizeof(ingressAddressSize));
    memcpy(&ingressAddressSize, ingressAddressSizeBytes.constData(), sizeof(ingressAddressSize));

    const QByteArray& ingressAddress = data.mid(sizeof(ingressAddressSize), ingressAddressSize);

    uint16_t ingressPort = 0;
    const auto& ingressPortBytes = data.mid(sizeof(ingressAddressSize) + ingressAddressSize, sizeof(ingressPort));
    memcpy(&ingressPort, ingressPortBytes, sizeof(uint16_t));

    size_t egressAddressSize = 0;
    const auto& egressAddressSizeBytes = data.mid(sizeof(ingressAddressSize) + ingressAddressSize + sizeof(ingressPort), sizeof(egressAddressSize));
    memcpy(&egressAddressSize, egressAddressSizeBytes.constData(), sizeof(size_t));

    const QByteArray& egressAddress = data.mid(sizeof(ingressAddressSize) + ingressAddressSize + sizeof(ingressPort) + sizeof(egressAddressSize), egressAddressSize);

    uint16_t egressPort = 0;
    const auto& egressPortBytes = data.mid(sizeof(ingressAddressSize) + ingressAddressSize + sizeof(ingressPort) + sizeof(egressAddressSize) + egressAddressSize, sizeof(egressPort));
    memcpy(&egressPort, egressPortBytes, sizeof(uint16_t));

    return std::make_tuple(ingressAddress, ingressPort, egressAddress, egressPort);
}
