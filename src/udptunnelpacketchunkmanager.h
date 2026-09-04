#ifndef UDPTUNNELPACKETCHUNKMANAGER_H
#define UDPTUNNELPACKETCHUNKMANAGER_H

#include "udptunnelpacket.h"
#include <map>
#include <QByteArray>
#include <QMutex>

class UDPTunnelPacketChunkManager
{
public:
    void addChunk(const UDPTunnelPacket& chunk);
    const QByteArray convertChunksToPayload(const size_t& packetId);
    const size_t getNumberOfChunks(const size_t& packetId) const;
    const std::vector<size_t> getMissingChunkIds(const size_t& packetId, const size_t& chunkIdStart, const size_t chunkIdEnd, const size_t limit = (MAX_UDP_PACKET_PAYLOAD_SIZE / (32 / 8))) const;
    static const std::vector<QByteArray> split(const QByteArray& payload, const size_t& chunkSize);

private:
    inline const bool chunkExists(const UDPTunnelPacket& chunk) const;

    std::map<size_t, std::map<size_t, UDPTunnelPacket>> chunks;
    QMutex lock;
};

#endif // UDPTUNNELPACKETCHUNKMANAGER_H
