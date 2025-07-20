#ifndef UDPTUNNELPACKETHEADER_H
#define UDPTUNNELPACKETHEADER_H

#include "udptunnelpacketheader.h"
#include <QString>
#include <QByteArray>

typedef enum UDPTunnelPacketType
{
    INVALID,
    UDP_DATA,
    UDP_DATA_FLUSH,
    UDP_ACKNOWLEDGEMENT
} UDPTunnelPacketType;

class UDPTunnelPacketHeader
{
public:
    UDPTunnelPacketHeader();
    UDPTunnelPacketHeader(const QByteArray& data);
    const QByteArray encode() const;
    const UDPTunnelPacketType& getPacketType() const;
    void setPacketType(const UDPTunnelPacketType& packetType);
    const bool& getLastSegment() const;
    void setLastSegment(const bool& lastSegment);
    const size_t& getPacketId() const;
    void setPacketId(const size_t& packetId);
    const size_t& getChunkId() const;
    void setChunkId(const size_t& chunkId);

    static qsizetype getHeaderSize();

private:
    static const QByteArray encode(const UDPTunnelPacketType& packetType, const bool& lastSegment, const size_t& packetId, const size_t& chunkId);
    static const std::tuple<UDPTunnelPacketType, bool, size_t, size_t> decode(const QByteArray& data);

    UDPTunnelPacketType packetType = UDPTunnelPacketType::INVALID;
    size_t packetId = -1;
    size_t chunkId = -1;
    bool firstSegment = false;
    bool lastSegment = false;
};

#endif // UDPTUNNELPACKETHEADER_H
