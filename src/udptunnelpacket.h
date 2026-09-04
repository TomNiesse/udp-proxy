#ifndef UDPTUNNELPACKET_H
#define UDPTUNNELPACKET_H

#include <QObject>
#include <QByteArray>
#include "udptunnelpacketheader.h"

class UDPTunnelPacket
{
public:
    explicit UDPTunnelPacket();
    explicit UDPTunnelPacket(std::pair<UDPTunnelPacketHeader, QByteArray>& decodedPacketData);
    explicit UDPTunnelPacket(const UDPTunnelPacketHeader& header, const QByteArray& payload);
    explicit UDPTunnelPacket(const QByteArray& encodedData);

    const UDPTunnelPacketHeader& getHeader() const;
    void setHeader(const UDPTunnelPacketHeader& header);
    const QByteArray& getPayload() const;
    void setPayload(const QByteArray& payload);
    const QByteArray encode() const;

    const QString toString() const;

    static const UDPTunnelPacket createAnnouncementPacket(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd);
    static const UDPTunnelPacket createAnnouncementAcknowledgementPacket(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd);
    static const UDPTunnelPacket createUdpDataPacket(const size_t& packetId, const size_t& chunkId, const QByteArray& payload);
    static const UDPTunnelPacket createUdpDataAcknowledgementPacket(const size_t& packetId, const size_t& chunkId);
    static const UDPTunnelPacket createUdpDataReceivedRequestPacket(const size_t& packetId);
    static const std::vector<UDPTunnelPacket> createUdpDataReceivedResponsePackets(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd, const std::vector<size_t>& missingChunkIds);
    static const UDPTunnelPacket createUdpDataFlushPacket(const size_t& packetId);
    static const UDPTunnelPacket createUdpDataFlushAcknowledgementPacket(const size_t& packetId);

    static const std::vector<QByteArray> split(const QByteArray& payload, const size_t& chunkSize);
    static const std::vector<UDPTunnelPacket> addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& splitPayload);
    static const QByteArray payloadFromChunks(const std::vector<UDPTunnelPacket>& chunks);

private:
    static const QByteArray encode(const UDPTunnelPacketHeader& header, const QByteArray& payload);

    UDPTunnelPacketHeader header;
    QByteArray payload;
};

#endif // UDPTUNNELPACKET_H
