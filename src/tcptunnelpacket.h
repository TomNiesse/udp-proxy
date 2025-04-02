#ifndef TCPTUNNELPACKET_H
#define TCPTUNNELPACKET_H

#include <QByteArray>
#include "tcptunnelpacketheader.h"

class TCPTunnelPacket
{
public:
    static const QByteArray encode(TCPTunnelPacketHeader header, QByteArray payload);
    static const std::pair<TCPTunnelPacketHeader, QByteArray> decode(const QByteArray encodedData);
};

#endif // TCPTUNNELPACKET_H
