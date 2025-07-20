#include "udptunnelpacket.h"

UDPTunnelPacket::UDPTunnelPacket()
{

}

UDPTunnelPacket::UDPTunnelPacket(std::pair<UDPTunnelPacketHeader, QByteArray>& decodedPacketData)
{
    this->header = decodedPacketData.first;
    this->payload = decodedPacketData.second;
}

UDPTunnelPacket::UDPTunnelPacket(const UDPTunnelPacketHeader& header, const QByteArray& payload)
{
    this->header = header;
    this->payload = payload;
}

UDPTunnelPacket::UDPTunnelPacket(const QByteArray& encoded)
{
    const auto& decoded = UDPTunnelPacket::decode(encoded);
    this->header = decoded.first;
    this->payload = decoded.second;
}

const UDPTunnelPacketHeader& UDPTunnelPacket::getHeader() const
{
    return this->header;
}

void UDPTunnelPacket::setHeader(const UDPTunnelPacketHeader& header)
{
    this->header = header;
}

const QByteArray& UDPTunnelPacket::getPayload() const
{
    return this->payload;
}

void UDPTunnelPacket::setPayload(const QByteArray& payload)
{
    this->payload = payload;
}

const QByteArray UDPTunnelPacket::encode() const
{
    return UDPTunnelPacket::encode(this->header, this->payload);
}

const QByteArray UDPTunnelPacket::encode(const UDPTunnelPacketHeader& header, const QByteArray& payload)
{
    QByteArray out;
    out.append(header.encode());
    out.append(payload);
    return out;
}

const std::pair<UDPTunnelPacketHeader, QByteArray> UDPTunnelPacket::decode(const QByteArray& data)
{
    const UDPTunnelPacketHeader packetHeader(data.mid(0, UDPTunnelPacketHeader::getHeaderSize()));
    const QByteArray& payload = data.mid(UDPTunnelPacketHeader::getHeaderSize(), data.length());
    return std::make_pair(packetHeader, payload);
}

const std::vector<QByteArray> UDPTunnelPacket::split(const QByteArray& payload, const size_t& chunkSize)
{
    std::vector<QByteArray> out;
    for(qsizetype pos = 0; pos < payload.size(); pos += chunkSize)
    {
        const auto& slice = payload.mid(pos, chunkSize);
        out.push_back(slice);
    }
    return out;
}

const std::vector<QByteArray> UDPTunnelPacket::addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& payloads)
{
    std::vector<QByteArray> out;
    auto headerCopy = header;
    for(size_t payloadId = 0; payloadId < payloads.size(); payloadId++)
    {
        const auto& payload = payloads[payloadId];
        const auto& lastPayload = (payloadId == (payloads.size()-1));
        headerCopy.setLastSegment(lastPayload);
        headerCopy.setPacketType(UDPTunnelPacketType::UDP_DATA);
        out.push_back(UDPTunnelPacket::encode(headerCopy, payload));
        headerCopy.setChunkId(headerCopy.getChunkId()+1);
    }
    headerCopy.setPacketType(UDPTunnelPacketType::UDP_DATA_FLUSH);
    out.push_back(UDPTunnelPacket::encode(headerCopy, {}));
    return out;
}

const QByteArray UDPTunnelPacket::payloadFromSegmentedEncodedPackets(const std::vector<QByteArray>& segmentedEncodedPackets)
{
    QByteArray outputPayload;
    for(const auto& packet : segmentedEncodedPackets)
    {
        const auto& decodedPacket = UDPTunnelPacket::decode(packet);
        const auto& payload = decodedPacket.second;
        outputPayload.append(payload);
    }
    return outputPayload;
}
