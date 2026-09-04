#include "udptunnelpacket.h"
#include "udptunnelpacketmissingchunklist.h"

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

UDPTunnelPacket::UDPTunnelPacket(const QByteArray& encodedData)
{
    const auto& udpTunnelPacketHeader = UDPTunnelPacketHeader(encodedData);
    const auto& udpTunnelPacketHeaderSize = udpTunnelPacketHeader.getHeaderSize(); // TODO: reimplement and test!
    const QByteArray& udpTunnelPacketPayload = encodedData.mid(udpTunnelPacketHeaderSize, encodedData.length());

    this->header = udpTunnelPacketHeader;
    this->payload = udpTunnelPacketPayload;
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

const QString UDPTunnelPacket::toString() const
{
    QString out;

    out.push_back(QString("%1, ").arg(this->header.toString()));
    out.push_back(QString("%1").arg(this->payload));

    return out;
}

const UDPTunnelPacket UDPTunnelPacket::createAnnouncementPacket(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_ANNOUNCEMENT);
    udpTunnelPacketHeader.setPacketId(packetId);
    udpTunnelPacketHeader.setChunkIdStart(chunkIdStart);
    udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

const UDPTunnelPacket UDPTunnelPacket::createAnnouncementAcknowledgementPacket(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT);
    udpTunnelPacketHeader.setPacketId(packetId);
    udpTunnelPacketHeader.setChunkIdStart(chunkIdStart);
    udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

const UDPTunnelPacket UDPTunnelPacket::createUdpDataPacket(const size_t& packetId, const size_t& chunkId, const QByteArray& payload)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_DATA);
    udpTunnelPacketHeader.setPacketId(packetId);
    udpTunnelPacketHeader.setChunkId(chunkId);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);
    udpTunnelPacket.setPayload(payload);

    return udpTunnelPacket;
}

const UDPTunnelPacket UDPTunnelPacket::createUdpDataAcknowledgementPacket(const size_t& packetId, const size_t& chunkId)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT);
    udpTunnelPacketHeader.setPacketId(packetId);
    udpTunnelPacketHeader.setChunkId(chunkId);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

const UDPTunnelPacket UDPTunnelPacket::createUdpDataReceivedRequestPacket(const size_t& packetId)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST);
    udpTunnelPacketHeader.setPacketId(packetId);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

// TODO: hoort deze functie hier thuis?!
const std::vector<UDPTunnelPacket> UDPTunnelPacket::createUdpDataReceivedResponsePackets(const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd, const std::vector<size_t>& missingChunkIds)
{
    std::vector<UDPTunnelPacket> out;

    // Create a header here, so the encoded header size can be calculated in advance
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    const auto& packetType = UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE;
    udpTunnelPacketHeader.setPacketType(packetType);

    // Split chunks based on the calculated header size
    const auto& headerSize = udpTunnelPacketHeader.getHeaderSize();
    const auto& splitMissingChunksLists = UDPTunnelPacketMissingChunkList::split(missingChunkIds, headerSize);
    const auto& encodedSplitMissingChunksLists = UDPTunnelPacketMissingChunkList::encode(splitMissingChunksLists);

    for(const auto& encodedSplitMissingChunksList : encodedSplitMissingChunksLists)
    {
        udpTunnelPacketHeader.setPacketType(packetType);
        udpTunnelPacketHeader.setPacketId(packetId);
        udpTunnelPacketHeader.setChunkIdStart(chunkIdStart);
        udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd);

        UDPTunnelPacket udpTunnelPacket;
        udpTunnelPacket.setHeader(udpTunnelPacketHeader);
        udpTunnelPacket.setPayload(encodedSplitMissingChunksList);

        out.push_back(udpTunnelPacket);
    }

    return out;
}


const UDPTunnelPacket UDPTunnelPacket::createUdpDataFlushPacket(const size_t& packetId)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_DATA_FLUSH);
    udpTunnelPacketHeader.setPacketId(packetId);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

const UDPTunnelPacket UDPTunnelPacket::createUdpDataFlushAcknowledgementPacket(const size_t& packetId)
{
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketType(UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT);
    udpTunnelPacketHeader.setPacketId(packetId);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    return udpTunnelPacket;
}

const std::vector<QByteArray> UDPTunnelPacket::split(const QByteArray& payload, const size_t& chunkSize)
{
    std::vector<QByteArray> out;
    for(qsizetype pos = 0; pos < payload.size(); pos += chunkSize)
    {
        const auto& slice = payload.mid(pos, chunkSize);
        out.push_back(slice);
    }

    // Add at least one item to the list, even if the input string is empty
    if(out.size() <= 0)
    {
        out.push_back({});
    }

    return out;
}

const std::vector<UDPTunnelPacket> UDPTunnelPacket::addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& splitPayload)
{
    std::vector<UDPTunnelPacket> out;
    auto headerCopy = header;
    for(size_t payloadId = 0; payloadId < splitPayload.size(); payloadId++)
    {
        const auto& payload = splitPayload.at(payloadId);
        headerCopy.setPacketType(UDPTunnelPacketType::UDP_DATA);
        out.push_back(UDPTunnelPacket(headerCopy, payload));
        headerCopy.setChunkId(headerCopy.getChunkId()+1);
    }
    return out;
}

const QByteArray UDPTunnelPacket::payloadFromChunks(const std::vector<UDPTunnelPacket>& chunks)
{
    QByteArray outputPayload;
    for(const auto& chunk : chunks)
    {
        const auto& payload = chunk.getPayload();
        outputPayload.append(payload);
    }
    return outputPayload;
}

// Private

const QByteArray UDPTunnelPacket::encode(const UDPTunnelPacketHeader& header, const QByteArray& payload)
{
    QByteArray out;
    out.append(header.encode());
    out.append(payload);
    return out;
}