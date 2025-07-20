#include "tcptunnelpacketheader.h"
#include <QString>
#include <memory>

TCPTunnelPacketHeader::TCPTunnelPacketHeader()
{
    this->packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    this->port = -1;
    this->host = "";
}

TCPTunnelPacketHeader::TCPTunnelPacketHeader(const QByteArray& encodedData)
{
    const auto& decodedData = TCPTunnelPacketHeader::decode(encodedData);
    this->packetType = std::get<0>(decodedData);
    this->port = std::get<1>(decodedData);
    // const auto hostStringSize = std::get<2>(decodedData);
    this->host = std::get<3>(decodedData);
}

const TCPTunnelPacketHeaderType& TCPTunnelPacketHeader::getPacketType() const
{
    return this->packetType;
}

void TCPTunnelPacketHeader::setPacketType(const TCPTunnelPacketHeaderType& packetType)
{
    this->packetType = packetType;
}

const QByteArray& TCPTunnelPacketHeader::getHost() const
{
    return this->host;
}

void TCPTunnelPacketHeader::setHost(const QByteArray& host)
{
    this->host = host;
}

const uint16_t& TCPTunnelPacketHeader::getPort() const
{
    return this->port;
}

void TCPTunnelPacketHeader::setPort(const uint16_t& port)
{
    this->port = port;
}

const QByteArray TCPTunnelPacketHeader::encode() const
{
    return TCPTunnelPacketHeader::encode(this->packetType, this->host, this->port);
}

// Private

const QByteArray TCPTunnelPacketHeader::encode(const TCPTunnelPacketHeaderType& packetType, const QByteArray& host, const uint16_t& port)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    char portBytes[sizeof(port)];
    memcpy(portBytes, (void*)&port, sizeof(port));
    QByteArray encodedPort = QByteArray(portBytes, sizeof(port));

    const size_t hostSize = host.length();
    char hostSizeBytes[sizeof(hostSize)];
    memcpy(hostSizeBytes, (void*)&hostSize, sizeof(hostSize));
    const QByteArray encodedHostSize = QByteArray(hostSizeBytes, sizeof(hostSize));

    QByteArray out;
    out.push_back(encodedPacketType);
    out.append(encodedPort);
    out.append(encodedHostSize);
    out.append(host);

    return out;
}

const std::tuple<TCPTunnelPacketHeaderType, uint16_t, size_t, QByteArray> TCPTunnelPacketHeader::decode(const QByteArray& encodedData)
{
    TCPTunnelPacketHeaderType packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    memcpy(&packetType, encodedData.mid(0).constData(), sizeof(TCPTunnelPacketHeaderType));

    uint16_t port = 0;
    memcpy(&port, encodedData.mid(sizeof(TCPTunnelPacketHeaderType)).constData(), sizeof(port));

    size_t hostStringSize = 0;
    memcpy(&hostStringSize, encodedData.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(port)).constData(), sizeof(hostStringSize));

    const QByteArray& host = encodedData.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(port)+sizeof(hostStringSize), hostStringSize);

    return std::make_tuple(packetType, port, hostStringSize, host);
}

size_t TCPTunnelPacketHeader::headerSize(const QByteArray& encodedData)
{
    size_t headerSize = 0;

    const auto& decodedData = TCPTunnelPacketHeader::decode(encodedData);
    const auto& packetType = std::get<0>(decodedData);
    const auto& port = std::get<1>(decodedData);
    const auto& hostStringSize = std::get<2>(decodedData);
    const auto& host = std::get<3>(decodedData);

    headerSize += sizeof(packetType);
    headerSize += sizeof(port);
    headerSize += sizeof(hostStringSize);
    headerSize += host.length();

    return headerSize;
}

size_t TCPTunnelPacketHeader::minimalHeaderSize()
{
    size_t minimalHeaderSize = 0;

    const auto& encodedData = TCPTunnelPacketHeader::encode(TCPTunnelPacketHeaderType::TCP_INVALID, {}, 0);
    const auto& decodedData = TCPTunnelPacketHeader::decode(encodedData);
    const auto& packetType = std::get<0>(decodedData);
    const auto& port = std::get<1>(decodedData);
    const auto& hostStringSize = std::get<2>(decodedData);
    const auto& host = std::get<3>(decodedData);

    minimalHeaderSize += sizeof(packetType);
    minimalHeaderSize += sizeof(port);
    minimalHeaderSize += sizeof(hostStringSize);
    minimalHeaderSize += host.length();

    return minimalHeaderSize;
}
