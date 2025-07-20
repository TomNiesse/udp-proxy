#include "udptunnelpacketchunkmananger.h"

void UDPTunnelPacketChunkMananger::addChunk(const UDPTunnelPacket& chunk)
{
    const QMutexLocker lock(&this->chunksMutex);

    const auto& packetId = chunk.getHeader().getPacketId();
    if(this->chunks.find(packetId) == this->chunks.end())
    {
        this->chunks.emplace(packetId, std::vector<UDPTunnelPacket>());
    }

    if(!this->chunkAlreadyExists(chunk))
    {
        this->chunks.at(packetId).push_back(chunk);
    }
}

const std::vector<QByteArray> UDPTunnelPacketChunkMananger::split(const QByteArray& payload, const size_t& chunkSize)
{
    return UDPTunnelPacket::split(payload, chunkSize);
}

const QByteArray UDPTunnelPacketChunkMananger::convertChunksToPayload(const size_t& packetId)
{
    const QMutexLocker lock(&this->chunksMutex);
    if(this->chunks.find(packetId) == this->chunks.end())
    {
        return {};
    }

    QByteArray out;
    const auto& chunks = this->chunks.at(packetId);
    for(const auto& chunk : chunks)
    {
        out.append(chunk.getPayload());
    }

    this->chunks.erase(packetId);

    return out;
}

// Private

bool UDPTunnelPacketChunkMananger::chunkAlreadyExists(const UDPTunnelPacket& chunk)
{
    const auto& packetId = chunk.getHeader().getPacketId();
    const auto& chunkId = chunk.getHeader().getChunkId();

    if(this->chunks.find(packetId) != this->chunks.end())
    {
        const auto& bufferedChunks = this->chunks.at(packetId);
        for(const auto& bufferedChunk : bufferedChunks)
        {
            if(bufferedChunk.getHeader().getPacketId() == packetId && bufferedChunk.getHeader().getChunkId() == chunkId)
            {
                return true;
            }
        }
    }

    return false;
}
