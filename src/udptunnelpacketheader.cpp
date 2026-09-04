#include "udptunnelpacketheader.h"

UDPTunnelPacketHeader::UDPTunnelPacketHeader()
{
    this->packetType = UDPTunnelPacketType::INVALID;
    this->packetId = 0;
    this->chunkId = 0;
    this->chunkIdStart = 0;
    this->chunkIdEnd = 0;
}

UDPTunnelPacketHeader::UDPTunnelPacketHeader(const QByteArray& encodedData)
{
    const auto& packetType = encodedData.at(0);

    std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> udpAnnouncementPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> udpAnnouncementAcknowledgementPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t, size_t> udpDataPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t, size_t> udpDataAcknowledgementPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t> udpDataReceivedRequestPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t> udpDataReceivedResponsePacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t, size_t> udpDataFlushPacketHeaderData;
    std::tuple<UDPTunnelPacketType, size_t, size_t> udpDataFlushAcknowledgementPacketHeaderData;

    switch(packetType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
        udpAnnouncementPacketHeaderData = decodeUdpAnnouncementPacketHeader(encodedData);
        this->packetType = std::get<0>(udpAnnouncementPacketHeaderData);
        this->packetId = std::get<1>(udpAnnouncementPacketHeaderData);
        this->chunkIdStart = std::get<2>(udpAnnouncementPacketHeaderData);
        this->chunkIdEnd = std::get<3>(udpAnnouncementPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        udpAnnouncementAcknowledgementPacketHeaderData = decodeUdpAnnouncementAcknowledgementPacketHeader(encodedData);
        this->packetType = std::get<0>(udpAnnouncementAcknowledgementPacketHeaderData);
        this->packetId = std::get<1>(udpAnnouncementAcknowledgementPacketHeaderData);
        this->chunkIdStart = std::get<2>(udpAnnouncementAcknowledgementPacketHeaderData);
        this->chunkIdEnd = std::get<3>(udpAnnouncementAcknowledgementPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA:
        udpDataPacketHeaderData = decodeUdpDataPacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataPacketHeaderData);
        this->packetId = std::get<1>(udpDataPacketHeaderData);
        this->chunkId = std::get<2>(udpDataPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
        udpDataAcknowledgementPacketHeaderData = decodeUdpDataAcknowledgementPacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataAcknowledgementPacketHeaderData);
        this->packetId = std::get<1>(udpDataAcknowledgementPacketHeaderData);
        this->chunkId = std::get<2>(udpDataAcknowledgementPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
        udpDataReceivedRequestPacketHeaderData = decodeUdpDataReceivedRequestPacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataReceivedRequestPacketHeaderData);
        this->packetId = std::get<1>(udpDataReceivedRequestPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        udpDataReceivedResponsePacketHeaderData = decodeUdpDataReceivedResponsePacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataReceivedResponsePacketHeaderData);
        this->packetId = std::get<1>(udpDataReceivedResponsePacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
        udpDataFlushPacketHeaderData = decodeUdpDataPacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataFlushPacketHeaderData);
        this->packetId = std::get<1>(udpDataFlushPacketHeaderData);
        this->chunkId = std::get<2>(udpDataFlushPacketHeaderData);
        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        udpDataFlushAcknowledgementPacketHeaderData = decodeUdpDataAcknowledgementPacketHeader(encodedData);
        this->packetType = std::get<0>(udpDataFlushAcknowledgementPacketHeaderData);
        this->packetId = std::get<1>(udpDataFlushAcknowledgementPacketHeaderData);
        this->chunkId = std::get<2>(udpDataFlushAcknowledgementPacketHeaderData);
        break;
    }
}

const QByteArray UDPTunnelPacketHeader::encode() const
{
    QByteArray encodedData;

    switch(this->packetType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        encodedData = encode(this->packetType, this->packetId, this->chunkIdStart, this->chunkIdEnd);
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        encodedData = encode(this->packetType, this->packetId);
        break;
    case UDPTunnelPacketType::UDP_DATA:
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        encodedData = encode(this->packetType, this->packetId, this->chunkId);
        break;
    default:
        break;
    }

    return encodedData;
}

const UDPTunnelPacketType UDPTunnelPacketHeader::getPacketType() const
{
    return this->packetType;
}

void UDPTunnelPacketHeader::setPacketType(const UDPTunnelPacketType& packetType)
{
    this->packetType = packetType;
}

const size_t UDPTunnelPacketHeader::getPacketId() const
{
    return this->packetId;
}

void UDPTunnelPacketHeader::setPacketId(const size_t& packetId)
{
    this->packetId = packetId;
}

const size_t UDPTunnelPacketHeader::getChunkId() const
{
    return this->chunkId;
}

void UDPTunnelPacketHeader::setChunkId(const size_t& chunkId)
{
    this->chunkId = chunkId;
}

const size_t UDPTunnelPacketHeader::getChunkIdStart() const
{
    return this->chunkIdStart;
}

void UDPTunnelPacketHeader::setChunkIdStart(const size_t& chunkIdStart)
{
    this->chunkIdStart = chunkIdStart;
}

const size_t UDPTunnelPacketHeader::getChunkIdEnd() const
{
    return this->chunkIdEnd;
}

void UDPTunnelPacketHeader::setChunkIdEnd(const size_t& chunkIdEnd)
{
    this->chunkIdEnd = chunkIdEnd;
}

const size_t UDPTunnelPacketHeader::getHeaderSize() const
{
    switch(this->packetType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        return sizeof(static_cast<uint8_t>(this->packetType)) + sizeof(this->packetId) + sizeof(this->chunkIdStart) + sizeof(this->chunkIdEnd);
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        return sizeof(static_cast<uint8_t>(this->packetType)) + sizeof(this->packetId);
    case UDPTunnelPacketType::UDP_DATA:
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        return sizeof(static_cast<uint8_t>(this->packetType)) + sizeof(this->packetId) + sizeof(this->chunkId);
    default:
        return 0;
    }
}

const QString UDPTunnelPacketHeader::toString() const
{
    QString out;

    QString packetTypeString;
    switch(this->packetType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
        packetTypeString = "UDPTunnelPacketType::UDP_ANNOUNCEMENT";
        break;
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        packetTypeString = "UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT";
        break;
    case UDPTunnelPacketType::UDP_DATA:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA";
        break;
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT";
        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA_FLUSH";
        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT";
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST";
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        packetTypeString = "UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE";
        break;
    case UDPTunnelPacketType::INVALID:
        packetTypeString = "UDPTunnelPacketType::INVALID";
        break;
    default:
        packetTypeString = "<UNKNOWN>";
        break;
    }

    out.push_back(QString("PacketType: %1").arg(packetTypeString));
    out.push_back(QString(", PacketId: %1").arg(this->packetId));
    out.push_back(QString(", chunkId: %1").arg(this->chunkId));
    out.push_back(QString(", chunkIdStart: %1").arg(this->chunkIdStart));
    out.push_back(QString(", chunkIdEnd: %1").arg(this->chunkIdEnd));

    return out;
}

// Protected

const QByteArray UDPTunnelPacketHeader::encode(const UDPTunnelPacketType& packetType, const size_t& packetId, const size_t& chunkIdStart, const size_t& chunkIdEnd)
{
    QByteArray out;

    out.push_back(static_cast<uint8_t>(packetType));

    uint8_t encodedPacketId[sizeof(packetId)] = {0};
    memcpy(encodedPacketId, &packetId, sizeof(packetId));
    out.append(QByteArray((char*)encodedPacketId, sizeof(packetId)));

    uint8_t encodedChunkIdStart[sizeof(chunkIdStart)] = {0};
    memcpy(encodedChunkIdStart, &chunkIdStart, sizeof(chunkIdStart));
    out.append(QByteArray((char*)encodedChunkIdStart, sizeof(chunkIdStart)));

    uint8_t encodedChunkIdEnd[sizeof(chunkIdEnd)] = {0};
    memcpy(encodedChunkIdEnd, &chunkIdEnd, sizeof(chunkIdEnd));
    out.append(QByteArray((char*)encodedChunkIdEnd, sizeof(chunkIdEnd)));

    return out;
}

const QByteArray UDPTunnelPacketHeader::encode(const UDPTunnelPacketType& packetType, const size_t& packetId, const size_t& chunkId)
{
    QByteArray out;

    out.push_back(packetType);

    uint8_t encodedPacketId[sizeof(packetId)] = {0};
    memcpy(encodedPacketId, &packetId, sizeof(packetId));
    out.append(QByteArray((char*)encodedPacketId, sizeof(packetId)));

    uint8_t encodedChunkId[sizeof(chunkId)] = {0};
    memcpy(encodedChunkId, &chunkId, sizeof(chunkId));
    out.append(QByteArray((char*)encodedChunkId, sizeof(chunkId)));

    return out;
}

const QByteArray UDPTunnelPacketHeader::encode(const UDPTunnelPacketType& packetType, const size_t& packetId)
{
    QByteArray out;

    out.push_back(packetType);

    uint8_t encodedPacketId[sizeof(packetId)] = {0};
    memcpy(encodedPacketId, &packetId, sizeof(packetId));
    out.append(QByteArray((char*)encodedPacketId, sizeof(packetId)));

    return out;
}

const std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> UDPTunnelPacketHeader::decodeUdpAnnouncementPacketHeader(const QByteArray& encodedData)
{
    const auto& packetType = encodedData.at(0);

    size_t packetId = 0;
    memcpy(&packetId, encodedData.mid(sizeof(packetType), sizeof(packetId)), sizeof(packetId));

    size_t chunkIdStart = 0;
    memcpy(&chunkIdStart, encodedData.mid(sizeof(packetType)+sizeof(packetId), sizeof(chunkIdStart)), sizeof(chunkIdStart));

    size_t chunkIdEnd = 0;
    memcpy(&chunkIdEnd, encodedData.mid(sizeof(packetType)+sizeof(packetId)+sizeof(chunkIdStart), sizeof(chunkIdEnd)), sizeof(chunkIdEnd));

    return std::make_tuple((UDPTunnelPacketType)packetType, packetId, chunkIdStart, chunkIdEnd);
}

const std::tuple<UDPTunnelPacketType, size_t, size_t, size_t> UDPTunnelPacketHeader::decodeUdpAnnouncementAcknowledgementPacketHeader(const QByteArray& encodedData)
{
    return decodeUdpAnnouncementPacketHeader(encodedData);
}

const std::tuple<UDPTunnelPacketType, size_t, size_t> UDPTunnelPacketHeader::decodeUdpDataPacketHeader(const QByteArray& encodedData)
{
    const auto& packetType = encodedData.at(0);

    size_t packetId = 0;
    memcpy(&packetId, encodedData.mid(sizeof(packetType), sizeof(packetId)), sizeof(packetId));

    size_t chunkId = 0;
    memcpy(&chunkId, encodedData.mid(sizeof(packetType)+sizeof(packetId), sizeof(chunkId)), sizeof(chunkId));

    return std::make_tuple((UDPTunnelPacketType)packetType, packetId, chunkId);
}

const std::tuple<UDPTunnelPacketType, size_t, size_t> UDPTunnelPacketHeader::decodeUdpDataAcknowledgementPacketHeader(const QByteArray& encodedData)
{
    return decodeUdpDataPacketHeader(encodedData);
}

const std::tuple<UDPTunnelPacketType, size_t> UDPTunnelPacketHeader::decodeUdpDataReceivedRequestPacketHeader(const QByteArray& encodedData)
{
    const auto& packetType = encodedData.at(0);

    size_t packetId = 0;
    memcpy(&packetId, encodedData.mid(sizeof(packetType), sizeof(packetId)), sizeof(packetId));

    return std::make_tuple((UDPTunnelPacketType)packetType, packetId);
}

const std::tuple<UDPTunnelPacketType, size_t> UDPTunnelPacketHeader::decodeUdpDataReceivedResponsePacketHeader(const QByteArray& encodedData)
{
    return decodeUdpDataReceivedRequestPacketHeader(encodedData);
}

const std::tuple<UDPTunnelPacketType, size_t, size_t> UDPTunnelPacketHeader::decodeUdpDataFlushPacketHeader(const QByteArray& encodedData)
{
    return decodeUdpDataPacketHeader(encodedData);
}

const std::tuple<UDPTunnelPacketType, size_t, size_t> UDPTunnelPacketHeader::decodeUdpDataFlushAcknowledgementPacketHeader(const QByteArray& encodedData)
{
    return decodeUdpDataPacketHeader(encodedData);
}