#include "udptunnelconnection.h"
#include "udptunnelpacket.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QElapsedTimer>

UDPTunnelConnection::UDPTunnelConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, const QString& name)
{
    this->udpTunnelConnectionSettings = std::make_unique<UDPTunnelConnectionSettings>(udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.getEgressPort());
    this->name = name;

    const auto& ingressAddress = this->udpTunnelConnectionSettings->getIngressAddress();
    const auto& ingressPort = this->udpTunnelConnectionSettings->getIngressPort();
    const auto& egressAddress = this->udpTunnelConnectionSettings->getEgressAddress();
    const auto& egressPort = this->udpTunnelConnectionSettings->getEgressPort();

    this->udpTunnelPacketTransceiver = std::make_unique<UDPTunnelPacketTransceiver>(ingressAddress, ingressPort, egressAddress, egressPort);

    QObject::connect(this->udpTunnelPacketTransceiver.get(), &UDPTunnelPacketTransceiver::receivedData, this, &UDPTunnelConnection::receivedData);
}

UDPTunnelConnection::~UDPTunnelConnection()
{
    QObject::disconnect(this->udpTunnelPacketTransceiver.get(), &UDPTunnelPacketTransceiver::receivedData, this, &UDPTunnelConnection::receivedData);
}

bool UDPTunnelConnection::send(const QByteArray& payload)
{
    const auto& payloadChunks = UDPTunnelPacket::split(payload, UDPTUNNEL_PAYLOAD_SIZE);
    UDPTunnelPacketHeader header;
    header.setPacketType(UDPTunnelPacketType::UDP_DATA);
    header.setPacketId(this->packetId);
    const auto& encodedChunks = UDPTunnelPacket::addHeaders(header, payloadChunks);
    this->packetId += encodedChunks.size();

    for(const auto& chunk : encodedChunks)
    {
        this->udpTunnelPacketTransceiver->send(chunk);
    }

    return true;
}

const UDPTunnelConnectionSettings UDPTunnelConnection::getUdpTunnelConnectionSettings() const
{
    return *this->udpTunnelConnectionSettings;
}
