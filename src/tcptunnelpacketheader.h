#ifndef TCPTUNNELPACKETHEADER_H
#define TCPTUNNELPACKETHEADER_H

#include <QByteArray>
#include <QString>

typedef enum TCPTunnelPacketHeaderType
{
    TCP_INVALID,
    TCP_OPEN_CONNECTION,
    TCP_CLOSE_CONNECTION,
    TCP_SEND_DATA,
    TCP_DATA_SENT,
    TCP_CONNECTION_OPENED,
    TCP_RECEIVED_DATA,
    TCP_CONNECTION_CLOSED,
    TCP_CONNECTION_TIMEOUT,
    CLIENT_CLOSE_CONNECTION, // TODO: remove
    CLIENT_CLOSED_CONNECTION // TODO: remove
} TCPTunnelPacketHeaderType;

class TCPTunnelPacketHeader
{
public:
    TCPTunnelPacketHeader();
    TCPTunnelPacketHeader(const QByteArray& data);

    const TCPTunnelPacketHeaderType& getPacketType() const;
    void setPacketType(const TCPTunnelPacketHeaderType& packetType);
    const QByteArray& getHost() const;
    void setHost(const QByteArray& host);
    const uint16_t& getPort() const;
    void setPort(const uint16_t& port);

    const QByteArray encode() const;
    const size_t headerSize() const;

private:
    static const QByteArray encodeConnectRequest(const TCPTunnelPacketHeaderType& packetType, const QByteArray& host = "", const uint16_t& port = -1);
    static const QByteArray encodeDisconnectRequest(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeWriteRequest(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeConnectResponse(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeDisconnectResponse(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeTimeoutResponse(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeBytesWritten(const TCPTunnelPacketHeaderType& packetType);
    static const QByteArray encodeBytesReceived(const TCPTunnelPacketHeaderType& packetType);

    static const TCPTunnelPacketHeader decode(const QByteArray& data);
    static const TCPTunnelPacketHeader decodeConnectRequest(const TCPTunnelPacketHeaderType& packetType, const QByteArray& data);
    static const TCPTunnelPacketHeader decodeDisconnectRequest(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeWriteRequest(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeConnectResponse(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeDisconnectResponse(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeTimeout(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeBytesWritten(const TCPTunnelPacketHeaderType& packetType);
    static const TCPTunnelPacketHeader decodeBytesReceived(const TCPTunnelPacketHeaderType& packetType);

    TCPTunnelPacketHeaderType packetType;
    QByteArray host = "";
    uint16_t port = 0;
};

#endif // TCPTUNNELPACKETHEADER_H
