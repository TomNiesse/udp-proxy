#ifndef UDPTUNNELPACKET_H
#define UDPTUNNELPACKET_H

#include <QObject>
#include <QByteArray>
#include "udptunnelpacketheader.h"

#define MAX_UDP_PACKET_SIZE 508
#define UDPTUNNEL_PAYLOAD_SIZE (MAX_UDP_PACKET_SIZE - UDPTunnelPacketHeader::getHeaderSize())

class UDPTunnelPacket
{
public:
    UDPTunnelPacket();
    UDPTunnelPacket(std::pair<UDPTunnelPacketHeader, QByteArray>& decodedPacketData);
    UDPTunnelPacket(const UDPTunnelPacketHeader& header, const QByteArray& payload);
    UDPTunnelPacket(const QByteArray& encoded);
    const UDPTunnelPacketHeader& getHeader() const;
    void setHeader(const UDPTunnelPacketHeader& header);
    const QByteArray& getPayload() const;
    void setPayload(const QByteArray& payload);
    const QByteArray encode() const;

    static const QByteArray encode(const UDPTunnelPacketHeader& header, const QByteArray& payload);
    static const std::pair<UDPTunnelPacketHeader, QByteArray> decode(const QByteArray& data);
    static const std::vector<QByteArray> split(const QByteArray& payload, const size_t& chunkSize);
    static const std::vector<QByteArray> addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& payloads);
    static const QByteArray payloadFromSegmentedEncodedPackets(const std::vector<QByteArray>& segmentedEncodedPackets);

private:
    UDPTunnelPacketHeader header;
    QByteArray payload;
};

#endif // UDPTUNNELPACKET_H
