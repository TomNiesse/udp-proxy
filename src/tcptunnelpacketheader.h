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
    TCP_CONNECTION_OPENED,
    TCP_RECEIVED_DATA,
    TCP_CONNECTION_CLOSED,
    CLIENT_CLOSE_CONNECTION,
    CLIENT_CLOSED_CONNECTION
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

    static const QByteArray encode(const TCPTunnelPacketHeaderType& packetType, const QByteArray& host = "", const uint16_t& port = -1);
    static const std::tuple<TCPTunnelPacketHeaderType, uint16_t, size_t, QByteArray> decode(const QByteArray& encodedData);
    static size_t headerSize(const QByteArray& encodedData);
    static size_t minimalHeaderSize();

private:
    TCPTunnelPacketHeaderType packetType;
    QByteArray host = "";
    uint16_t port = 0;
};

#endif // TCPTUNNELPACKETHEADER_H
