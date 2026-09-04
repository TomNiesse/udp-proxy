#ifndef UDPTUNNELPACKETHEADER_H
#define UDPTUNNELPACKETHEADER_H

#include "udptunnelpacketheader.h"
#include <QString>
#include <QByteArray>

#define MAX_UDP_PACKET_SIZE 512
#define MAX_UDP_PACKET_PAYLOAD_SIZE (MAX_UDP_PACKET_SIZE - 100/*UDPTunnelPacketHeader::getUdpDataPacketHeaderSize()*/) // TODO: FIX THIS!
#define MAX_MISSING_CHUNK_LIST_UDP_PACKET_SIZE (MAX_UDP_PACKET_PAYLOAD_SIZE / sizeof(size_t) - sizeof(size_t))

typedef enum
{
    INVALID,
    UDP_ANNOUNCEMENT,
    UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT,
    UDP_DATA,
    UDP_DATA_ACKNOWLEDGEMENT,
    UDP_DATA_RECEIVED_REQUEST,
    UDP_DATA_RECEIVED_RESPONSE,
    UDP_DATA_FLUSH,
    UDP_DATA_FLUSH_ACKNOWLEDGEMENT
} UDPTunnelPacketType;

class UDPTunnelPacketHeader
{
public:
    explicit UDPTunnelPacketHeader();
    explicit UDPTunnelPacketHeader(const QByteArray& encodedData);

    const QByteArray encode() const;

    const UDPTunnelPacketType getPacketType() const;
    void setPacketType(const UDPTunnelPacketType& packetType);
    const size_t getPacketId() const;
    void setPacketId(const size_t& packetId);
    const size_t getChunkId() const;
    void setChunkId(const size_t& chunkId);
    const size_t getChunkIdStart() const;
    void setChunkIdStart(const size_t& chunkIdStart);
    const size_t getChunkIdEnd() const;
    void setChunkIdEnd(const size_t& chunkIdEnd);

    const size_t getHeaderSize() const;
    const QString toString() const;

private:
    static const QByteArray encode(const UDPTunnelPacketType& packetType, const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd);
    static const QByteArray encode(const UDPTunnelPacketType& packetType, const size_t& packetId, const size_t& chunkId);
    static const QByteArray encode(const UDPTunnelPacketType& packetType, const size_t& packetId);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> decodeUdpAnnouncementPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> decodeUdpAnnouncementAcknowledgementPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t> decodeUdpDataPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t> decodeUdpDataAcknowledgementPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t> decodeUdpDataReceivedRequestPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t> decodeUdpDataReceivedResponsePacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t> decodeUdpDataFlushPacketHeader(const QByteArray& encodedData);
    static const std::tuple<UDPTunnelPacketType, size_t, size_t> decodeUdpDataFlushAcknowledgementPacketHeader(const QByteArray& encodedData);

    UDPTunnelPacketType packetType = UDPTunnelPacketType::INVALID;
    size_t packetId = 0;
    // Optional field, not used by announcement packets
    size_t chunkId = 0;
    // Optional fields, only used by announcement packets
    size_t chunkIdStart = 0;
    size_t chunkIdEnd = 0;

    friend class test_udptunnelpacketheader;
};

#endif // UDPTUNNELPACKETHEADER_H
