#ifndef TCPTUNNELPACKET_H
#define TCPTUNNELPACKET_H

#include "tcptunnelpacketheader.h"
#include <QByteArray>

class TCPTunnelPacket
{
public:
    static const QByteArray encode(const TCPTunnelPacketHeader& header, const QByteArray& payload);
    static const std::pair<TCPTunnelPacketHeader, QByteArray> decode(const QByteArray& encodedData);
};

#endif // TCPTUNNELPACKET_H
