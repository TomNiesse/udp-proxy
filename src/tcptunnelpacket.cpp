#include "tcptunnelpacket.h"

const QByteArray TCPTunnelPacket::encode(TCPTunnelPacketHeader& header, const QByteArray& payload)
{
    QByteArray out;
    out.push_back(header.encode());
    out.push_back(payload);
    return out;
}

const std::pair<TCPTunnelPacketHeader, QByteArray> TCPTunnelPacket::decode(const QByteArray& encodedData)
{
    const qsizetype& headerSize = TCPTunnelPacketHeader::headerSize(encodedData);
    const qsizetype& minimalHeaderSize = TCPTunnelPacketHeader::minimalHeaderSize();

    if(encodedData.size() < minimalHeaderSize)
    {
        TCPTunnelPacketHeader header;
        header.setPacketType(TCPTunnelPacketHeaderType::TCP_INVALID);
        return std::make_pair(header, QByteArray());
    }

    const TCPTunnelPacketHeader header(encodedData);
    const QByteArray& payload = encodedData.mid(headerSize);
    return std::make_pair(header, payload);
}
