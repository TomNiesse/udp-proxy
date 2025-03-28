#include "udptunnelpacketheader.h"
#include <QDebug>
#include <cstring>

UDPTunnelPacketHeader::UDPTunnelPacketHeader()
{
    this->packetType = UDPTunnelPacketType::INVALID;
    this->packetId = 0;
    this->lastSegment = false;
}

UDPTunnelPacketHeader::UDPTunnelPacketHeader(const QByteArray data)
{
    const auto decodedData = UDPTunnelPacketHeader::decode(data);
    this->packetType = std::get<0>(decodedData);
    this->lastSegment = std::get<1>(decodedData);
    this->packetId = std::get<2>(decodedData);
    this->chunkId = std::get<3>(decodedData);
}

const QByteArray UDPTunnelPacketHeader::encode() const
{
    return UDPTunnelPacketHeader::encode(this->packetType, this->lastSegment, this->packetId, this->chunkId);
}

const UDPTunnelPacketType& UDPTunnelPacketHeader::getPacketType() const
{
    return this->packetType;
}

void UDPTunnelPacketHeader::setPacketType(const UDPTunnelPacketType packetType)
{
    this->packetType = packetType;
}

const bool& UDPTunnelPacketHeader::getLastSegment() const
{
    return this->lastSegment;
}

void UDPTunnelPacketHeader::setLastSegment(const bool lastSegment)
{
    this->lastSegment = lastSegment;
}

qsizetype UDPTunnelPacketHeader::getHeaderSize()
{
    return UDPTunnelPacketHeader::encode(UDPTunnelPacketType::INVALID, false, 0, 0).size();
}

const size_t& UDPTunnelPacketHeader::getPacketId() const
{
    return this->packetId;
}

void UDPTunnelPacketHeader::setPacketId(const size_t packetId)
{
    this->packetId = packetId;
}

const size_t& UDPTunnelPacketHeader::getChunkId() const
{
    return this->chunkId;
}

void UDPTunnelPacketHeader::setChunkId(const size_t chunkId)
{
    this->chunkId = chunkId;
}

// Protected

const QByteArray UDPTunnelPacketHeader::encode(UDPTunnelPacketType packetType, const bool lastSegment, const size_t packetId, const size_t chunkId)
{
    QByteArray out;
    out.push_back(packetType);
    out.push_back(lastSegment);
    uint8_t encodedPacketId[sizeof(packetId)] = {0};
    memcpy(encodedPacketId, &packetId, sizeof(packetId));
    out.append(QByteArray((char*)encodedPacketId, sizeof(packetId)));
    uint8_t encodedChunkId[sizeof(chunkId)] = {0};
    memcpy(encodedChunkId, &chunkId, sizeof(chunkId));
    out.append(QByteArray((char*)encodedChunkId, sizeof(chunkId)));
    return out;
}

const std::tuple<UDPTunnelPacketType, bool, size_t, size_t> UDPTunnelPacketHeader::decode(const QByteArray& data)
{
    if(data.size() < UDPTunnelPacketHeader::getHeaderSize())
    {
        return std::make_tuple(UDPTunnelPacketType::INVALID, false, 0, 0);
    }

    const auto packetType = data.at(0);
    const auto lastSegment = data.at(1);
    size_t packetId = 0;
    memcpy(&packetId, data.mid(sizeof(packetType)+sizeof(lastSegment), sizeof(packetId)), sizeof(packetId));
    size_t chunkId = 0;
    memcpy(&chunkId, data.mid(sizeof(packetType)+sizeof(lastSegment)+sizeof(packetId), sizeof(chunkId)), sizeof(chunkId));
    return std::make_tuple((UDPTunnelPacketType)packetType, (bool)lastSegment, (size_t)packetId, (size_t)chunkId);
}
