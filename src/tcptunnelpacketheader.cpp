#include "tcptunnelpacketheader.h"
#include <QString>
#include <memory>

TCPTunnelPacketHeader::TCPTunnelPacketHeader()
{
    this->packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    this->port = -1;
    this->host = "";
}

TCPTunnelPacketHeader::TCPTunnelPacketHeader(const QByteArray& data)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader = TCPTunnelPacketHeader::decode(data);
    this->setPacketType(tcpTunnelPacketHeader.getPacketType());
    this->setHost(tcpTunnelPacketHeader.getHost());
    this->setPort(tcpTunnelPacketHeader.getPort());
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
    switch(this->packetType)
    {
    case TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION:
        return encodeConnectRequest(this->packetType, this->host, this->port);
    case TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION:
        return encodeDisconnectRequest(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_SEND_DATA:
        return encodeWriteRequest(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED:
        return encodeConnectResponse(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED:
        return encodeDisconnectResponse(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_TIMEOUT:
        return encodeTimeoutResponse(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_DATA_SENT:
        return encodeBytesWritten(this->packetType);
    case TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA:
        return encodeBytesReceived(this->packetType);
    }

    return {};
}

const size_t TCPTunnelPacketHeader::headerSize() const
{
    // TODO: improve this! this function can be much faster with a switch-case!
    return this->encode().size();
}

// Private

const QByteArray TCPTunnelPacketHeader::encodeConnectRequest(const TCPTunnelPacketHeaderType& packetType, const QByteArray& host, const uint16_t& port)
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

const QByteArray TCPTunnelPacketHeader::encodeDisconnectRequest(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeWriteRequest(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeConnectResponse(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeDisconnectResponse(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeTimeoutResponse(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeBytesWritten(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const QByteArray TCPTunnelPacketHeader::encodeBytesReceived(const TCPTunnelPacketHeaderType& packetType)
{
    char packetTypeBytes[sizeof(packetType)] = {0};
    memcpy(packetTypeBytes, (void*)&packetType, sizeof(packetType));
    QByteArray encodedPacketType = QByteArray(packetTypeBytes, sizeof(packetType));

    QByteArray out;
    out.push_back(encodedPacketType);

    return out;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decode(const QByteArray& data)
{
    if(data.size() > 0)
    {
        TCPTunnelPacketHeaderType packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
        memcpy(&packetType, data.mid(0).constData(), sizeof(TCPTunnelPacketHeaderType));
        switch(packetType)
        {
        case TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION:
            return decodeConnectRequest(packetType, data);
        case TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION:
            return decodeDisconnectRequest(packetType);
        case TCPTunnelPacketHeaderType::TCP_SEND_DATA:
            return decodeWriteRequest(packetType);
        case TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED:
            return decodeConnectResponse(packetType);
        case TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED:
            return decodeDisconnectResponse(packetType);
        case TCPTunnelPacketHeaderType::TCP_CONNECTION_TIMEOUT:
            return decodeTimeout(packetType);
        case TCPTunnelPacketHeaderType::TCP_DATA_SENT:
            return decodeBytesWritten(packetType);
        case TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA:
            return decodeBytesReceived(packetType);
        }
    }

    return {};
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeConnectRequest(const TCPTunnelPacketHeaderType& packetType, const QByteArray& data)
{
    uint16_t port = 0;
    memcpy(&port, data.mid(sizeof(TCPTunnelPacketHeaderType)).constData(), sizeof(port));

    size_t hostStringSize = 0;
    memcpy(&hostStringSize, data.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(port)).constData(), sizeof(hostStringSize));

    const QByteArray& host = data.mid(sizeof(TCPTunnelPacketHeaderType)+sizeof(port)+sizeof(hostStringSize), hostStringSize);

    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    tcpTunnelPacketHeader.setHost(host);
    tcpTunnelPacketHeader.setPort(port);
    return tcpTunnelPacketHeader;
}

// TODO: The following functions need some sort of abstraction to prevent copy pasta'ing the same code
const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeDisconnectRequest(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeWriteRequest(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeConnectResponse(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeDisconnectResponse(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeTimeout(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeBytesWritten(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}

const TCPTunnelPacketHeader TCPTunnelPacketHeader::decodeBytesReceived(const TCPTunnelPacketHeaderType& packetType)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(packetType);
    return tcpTunnelPacketHeader;
}