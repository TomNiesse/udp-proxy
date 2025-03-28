#ifndef UDPTUNNELPACKETCHUNKMANANGER_H
#define UDPTUNNELPACKETCHUNKMANANGER_H

#include "udptunnelpacket.h"
#include <map>
#include <QByteArray>
#include <QMutex>

class UDPTunnelPacketChunkMananger
{
public:
    UDPTunnelPacketChunkMananger();
    void addChunk(const UDPTunnelPacket chunk);
    const std::vector<QByteArray> split(const QByteArray payload, const size_t chunkSize = UDPTUNNEL_PAYLOAD_SIZE);
    const QByteArray convertChunksToPayload(const size_t packetId);
    const std::vector<UDPTunnelPacket> getChunks(const size_t packetId);

private:
    bool chunkAlreadyExists(const UDPTunnelPacket chunk);
    std::map<size_t, std::vector<UDPTunnelPacket>> chunks;
    QMutex lock;
};

#endif // UDPTUNNELPACKETCHUNKMANANGER_H
