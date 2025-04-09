#ifndef TCPTUNNELPACKETHEADER_H
#define TCPTUNNELPACKETHEADER_H

#include <QByteArray>
#include <QString>
#include <cstdint>

typedef enum TCPTunnelPacketHeaderType
{
    TCP_INVALID,
    TCP_COMMAND_OPEN_CONNECTION,
    TCP_COMMAND_CLOSE_CONNECTION,
    TCP_COMMAND_SEND_DATA,
    TCP_RESPONSE_OPENED_CONNECTION,
    TCP_RESPONSE_CLOSED_CONNECTION,
    TCP_RESPONSE_SENT_DATA,
    TCP_RESPONSE_RECEIVED_DATA
} TCPTunnelPacketHeaderType;

class TCPTunnelPacketHeader
{
public:
    explicit TCPTunnelPacketHeader();
    explicit TCPTunnelPacketHeader(const QByteArray& data);
    const QByteArray encode();

    TCPTunnelPacketHeaderType getPacketType() const;
    void setPacketType(const TCPTunnelPacketHeaderType& packetType);
    size_t getConnectionId() const;
    void setConnectionId(const size_t& connectionId);
    QByteArray getHost() const;
    void setHost(const QByteArray& host);
    quint16 getPort() const;
    void setPort(const quint16& port);

    static const QByteArray encode(const TCPTunnelPacketHeaderType& packetType, const size_t& connectionId, const QByteArray& host = "", const quint16& port = -1);
    static const std::tuple<TCPTunnelPacketHeaderType, size_t, quint16, size_t, QByteArray> decode(const QByteArray& encodedData);
    static qsizetype headerSize(const QByteArray& encodedData);
    static qsizetype minimalHeaderSize();
private:
    TCPTunnelPacketHeaderType packetType = TCPTunnelPacketHeaderType::TCP_INVALID;
    size_t connectionId = -1;
    QByteArray host = "";
    quint16 port = -1;
};

#endif // TCPTUNNELPACKETHEADER_H
