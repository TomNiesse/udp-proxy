#ifndef TCPTUNNELPACKET_H
#define TCPTUNNELPACKET_H

#include "tcptunnelpacketheader.h"
#include <QByteArray>

class TCPTunnelPacket
{
public:
    explicit TCPTunnelPacket();
    explicit TCPTunnelPacket(const TCPTunnelPacketHeader& header, const QByteArray& payload);
    explicit TCPTunnelPacket(const QByteArray& data);

    const TCPTunnelPacketHeader getHeader() const;
    void setHeader(const TCPTunnelPacketHeader& header);
    const QByteArray getPayload() const;
    void setPayload(const QByteArray& payload);

    const QByteArray encode() const;

    static const TCPTunnelPacket createConnectRequest(const QByteArray& host, const int& port);
    static const TCPTunnelPacket createDisconnectRequest();
    static const TCPTunnelPacket createWriteRequest(const QByteArray& data);
    static const TCPTunnelPacket createConnectedResponse(const QByteArray& host, const int& port);
    static const TCPTunnelPacket createDisconnectedResponse();
    static const TCPTunnelPacket createBytesWrittenResponse();
    static const TCPTunnelPacket createReceivedDataResponse(const QByteArray& receivedData);
    static const TCPTunnelPacket createConnectionTimeoutResponse();

private:
    static const QByteArray encode(const TCPTunnelPacketHeader& header, const QByteArray& payload);
    static const std::pair<TCPTunnelPacketHeader, QByteArray> decode(const QByteArray& encodedData);

    TCPTunnelPacketHeader header;
    QByteArray payload;
};

#endif // TCPTUNNELPACKET_H
