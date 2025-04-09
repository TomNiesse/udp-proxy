#include "tcptunnelpacketheader.h"
#include <QString>
#include <memory>

#include <QDebug>

TCPTunnelPacketHeader::TCPTunnelPacketHeader()
{
    this->packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    this->connectionId = -1;
    this->port = -1;
    this->host = "";
}

TCPTunnelPacketHeader::TCPTunnelPacketHeader(const QByteArray& encodedData)
{
    const auto decodedData = TCPTunnelPacketHeader::decode(encodedData);
    this->packetType = std::get<0>(decodedData);
    this->connectionId = std::get<1>(decodedData);
    this->port = std::get<2>(decodedData);
    // const auto hostStringSize = std::get<3>(decodedData);
    this->host = std::get<4>(decodedData);
}

const QByteArray TCPTunnelPacketHeader::encode()
{
    return TCPTunnelPacketHeader::encode(this->packetType, this->connectionId, this->host, this->port);
}

TCPTunnelPacketHeaderType TCPTunnelPacketHeader::getPacketType() const
{
    return this->packetType;
}

void TCPTunnelPacketHeader::setPacketType(const TCPTunnelPacketHeaderType& packetType)
{
    this->packetType = packetType;
}

size_t TCPTunnelPacketHeader::getConnectionId() const
{
    return this->connectionId;
}

void TCPTunnelPacketHeader::setConnectionId(const size_t& connectionId)
{
    this->connectionId = connectionId;
}

QByteArray TCPTunnelPacketHeader::getHost() const
{
    return this->host;
}

void TCPTunnelPacketHeader::setHost(const QByteArray& host)
{
    this->host = host;
}

quint16 TCPTunnelPacketHeader::getPort() const
{
    return this->port;
}

void TCPTunnelPacketHeader::setPort(const quint16& port)
{
    this->port = port;
}

// Private

const QByteArray TCPTunnelPacketHeader::encode(const TCPTunnelPacketHeaderType& packetType, const size_t& connectionId, const QByteArray& host, const quint16& port)
{
    // Convert the packet type to a set of bytes
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    const QByteArray& encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    // Convert the connectionId to a set of bytes
    char connectionIdBytes[sizeof(connectionId)] = {0};
    memcpy(connectionIdBytes, (void*)&connectionId, sizeof(connectionId));
    const QByteArray& encodedConnectionId = QByteArray(connectionIdBytes, sizeof(connectionId));

    // Convert the port to a set of bytes
    char portBytes[sizeof(port)] = {0};
    memcpy(portBytes, (void*)&port, sizeof(port));
    const QByteArray& encodedPort = QByteArray(portBytes, sizeof(port));

    // Convert the length of the hostname to a set of bytes
    const size_t& hostSize = host.length();
    char hostSizeBytes[sizeof(hostSize)] = {0};
    memcpy(hostSizeBytes, (void*)&hostSize, sizeof(hostSize));
    const QByteArray& encodedHostSize = QByteArray(hostSizeBytes, sizeof(hostSize));

    // Encode the class data
    QByteArray out;
    out.push_back(encodedPacketType);
    out.append(encodedConnectionId);
    out.append(encodedPort);
    out.append(encodedHostSize);
    out.append(host);

    return out;
}

const std::tuple<TCPTunnelPacketHeaderType, size_t, quint16, size_t, QByteArray> TCPTunnelPacketHeader::decode(const QByteArray& encodedData)
{
    // Extract packet type
    TCPTunnelPacketHeaderType packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    memcpy(&packetType, encodedData.mid(0).constData(), sizeof(TCPTunnelPacketHeaderType));

    // Extract connection id
    size_t connectionId = 0;
    memcpy(&connectionId, encodedData.mid(sizeof(TCPTunnelPacketHeaderType)).constData(), sizeof(connectionId));

    // Extract port
    quint16 port = 0;
    memcpy(&port, encodedData.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(connectionId)).constData(), sizeof(port));

    // Extract host string size
    size_t hostStringSize = 0;
    memcpy(&hostStringSize, encodedData.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(connectionId)+sizeof(port)).constData(), sizeof(hostStringSize));

    // Extract host
    const QByteArray& host = encodedData.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(connectionId)+sizeof(port)+sizeof(hostStringSize), hostStringSize);

    return std::make_tuple(packetType, connectionId, port, hostStringSize, host);
}

qsizetype TCPTunnelPacketHeader::headerSize(const QByteArray& encodedData)
{
    qsizetype headerSize = 0;

    const auto& decodedData = TCPTunnelPacketHeader::decode(encodedData);
    const auto& packetType = std::get<0>(decodedData);
    const auto& connectionId = std::get<1>(decodedData);
    const auto& port = std::get<2>(decodedData);
    const auto& hostStringSize = std::get<3>(decodedData);
    const auto& host = std::get<4>(decodedData);

    headerSize += sizeof(packetType);
    headerSize += sizeof(connectionId);
    headerSize += sizeof(port);
    headerSize += sizeof(hostStringSize);
    headerSize += host.length();

    return headerSize;
}


qsizetype TCPTunnelPacketHeader::minimalHeaderSize()
{
    qsizetype minimalHeaderSize = 0;

    const auto encodedData = TCPTunnelPacketHeader::encode(TCPTunnelPacketHeaderType::TCP_INVALID, 0, {}, 0);
    const auto decodedData = TCPTunnelPacketHeader::decode(encodedData);
    const auto packetType = std::get<0>(decodedData);
    const auto connectionId = std::get<1>(decodedData);
    const auto port = std::get<2>(decodedData);
    const auto hostStringSize = std::get<3>(decodedData);
    const auto host = std::get<4>(decodedData);

    minimalHeaderSize += sizeof(packetType);
    minimalHeaderSize += sizeof(connectionId);
    minimalHeaderSize += sizeof(port);
    minimalHeaderSize += sizeof(hostStringSize);
    minimalHeaderSize += host.length();

    return minimalHeaderSize;
}
