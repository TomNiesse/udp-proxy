#include "tcptunnelpacket.h"

TCPTunnelPacket::TCPTunnelPacket()
    : header(TCPTunnelPacketHeader())
    , payload({})
{

}

TCPTunnelPacket::TCPTunnelPacket(const TCPTunnelPacketHeader& header, const QByteArray& payload)
    : header(header)
    , payload(payload)
{

}

TCPTunnelPacket::TCPTunnelPacket(const QByteArray& data)
    : header(TCPTunnelPacketHeader())
    , payload({})
{
    if(data.size() > 0)
    {
        const auto& decodedData = TCPTunnelPacket::decode(data);
        this->header = decodedData.first;
        this->payload = decodedData.second;
    }
}

const TCPTunnelPacketHeader TCPTunnelPacket::getHeader() const
{
    return this->header;
}

void TCPTunnelPacket::setHeader(const TCPTunnelPacketHeader& header)
{
    this->header = header;
}

const QByteArray TCPTunnelPacket::getPayload() const
{
    return this->payload;
}

void TCPTunnelPacket::setPayload(const QByteArray& payload)
{
    this->payload = payload;
}


const QByteArray TCPTunnelPacket::encode() const
{
    return TCPTunnelPacket::encode(this->header, this->payload);
}

// Private

const TCPTunnelPacket TCPTunnelPacket::createConnectRequest(const QByteArray& host, const int& port)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION);
    tcpTunnelPacketHeader.setHost(host);
    tcpTunnelPacketHeader.setPort(port);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const TCPTunnelPacket TCPTunnelPacket::createDisconnectRequest()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const TCPTunnelPacket TCPTunnelPacket::createWriteRequest(const QByteArray& data)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_SEND_DATA); // TODO: rename to WRITE!
    return TCPTunnelPacket(tcpTunnelPacketHeader, data);
}

const TCPTunnelPacket TCPTunnelPacket::createConnectedResponse(const QByteArray& host, const int& port)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED);
    tcpTunnelPacketHeader.setHost(host);
    tcpTunnelPacketHeader.setPort(port);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const TCPTunnelPacket TCPTunnelPacket::createDisconnectedResponse()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const TCPTunnelPacket TCPTunnelPacket::createBytesWrittenResponse()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_DATA_SENT);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const TCPTunnelPacket TCPTunnelPacket::createReceivedDataResponse(const QByteArray& receivedData)
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA);
    return TCPTunnelPacket(tcpTunnelPacketHeader, receivedData);
}

const TCPTunnelPacket TCPTunnelPacket::createConnectionTimeoutResponse()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_TIMEOUT);
    return TCPTunnelPacket(tcpTunnelPacketHeader, {});
}

const QByteArray TCPTunnelPacket::encode(const TCPTunnelPacketHeader& header, const QByteArray& payload)
{
    QByteArray out;
    out.append(header.encode());
    out.append(payload);
    return out;
}

const std::pair<TCPTunnelPacketHeader, QByteArray> TCPTunnelPacket::decode(const QByteArray& encodedData)
{
    if(encodedData.size() > 0)
    {
        const TCPTunnelPacketHeader tcpTunnelPacketHeader(encodedData);

        const qsizetype& tcpTunnelPacketHeaderSize = tcpTunnelPacketHeader.headerSize();
        const QByteArray& tcpTunnelPacketPayload = encodedData.mid(tcpTunnelPacketHeaderSize);

        return std::make_pair(tcpTunnelPacketHeader, tcpTunnelPacketPayload);
    }
    else
    {
        return {};
    }
}
