#include "udptunnelpacketchunkmanager.h"

void UDPTunnelPacketChunkManager::addChunk(const UDPTunnelPacket& chunk)
{
    const QMutexLocker lock(&this->lock);

    const auto& packetId = chunk.getHeader().getPacketId();
    if(!this->chunks.contains(packetId))
    {
        this->chunks.emplace(packetId, std::map<size_t, UDPTunnelPacket>());
    }

    const auto& chunkId = chunk.getHeader().getChunkId();
    if(!this->chunks.at(packetId).contains(chunkId))
    {
        this->chunks.at(packetId).emplace(chunkId, chunk);
    }
}

const QByteArray UDPTunnelPacketChunkManager::convertChunksToPayload(const size_t& packetId)
{
    const QMutexLocker lock(&this->lock);

    if(!this->chunks.contains(packetId))
    {
        return {};
    }

    QByteArray out;
    const auto& chunks = this->chunks.at(packetId);
    for(const auto& chunk : chunks)
    {
        out.append(chunk.second.getPayload());
    }

    this->chunks.erase(packetId);

    return out;
}

const size_t UDPTunnelPacketChunkManager::getNumberOfChunks(const size_t& packetId) const
{
    if(this->chunks.contains(packetId))
    {
        return this->chunks.at(packetId).size();
    }

    return 0;
}

#include <QDebug>
#include <QDateTime>
const std::vector<size_t> UDPTunnelPacketChunkManager::getMissingChunkIds(const size_t& packetId, const size_t& chunkIdStart, const size_t chunkIdEnd, const size_t limit) const
{
    // qDebug() << QDateTime::currentDateTime() << "GetMissingChunks START!";

    std::vector<size_t> missingChunkIds;

    if(!this->chunks.contains(packetId))
    {
        // There is no chunks list for the given packet ID.
        // Add all the missing chunks to the output list.
        for(size_t chunkId = chunkIdStart; chunkId <= chunkIdEnd; chunkId++)
        {
            missingChunkIds.push_back(chunkId);
        }
    }
    else
    {
        const auto& packetChunks = this->chunks.at(packetId);
        for(size_t chunkId = chunkIdStart; chunkId <= chunkIdEnd; chunkId++)
        {
            if(!packetChunks.contains(chunkId))
            {
                missingChunkIds.push_back(chunkId);
            }
            if(missingChunkIds.size() >= limit && limit > 0)
            {
                break;
            }
        }
        // for(const auto& chunk : this->chunks)
        // {
        //     if()
        // }


        // std::vector<bool> receivedChunksList;
        // if(limit > 0)
        // {
        //     receivedChunksList.assign(limit, false);
        // }
        // else
        // {
        //     receivedChunksList.assign(chunkIdEnd - chunkIdStart + 1, false);
        // }

        // const auto& packetChunks = this->chunks.at(packetId);
        // for(const auto& packetChunk : packetChunks)
        // {
        //     const auto& chunkId = packetChunk.second.getHeader().getChunkId();
        //     const auto& chunkIndex = chunkId - chunkIdStart;
        //     receivedChunksList.at(chunkIndex) = true;

        //     if(chunkIndex >= limit && limit > 0)
        //     {
        //         break;
        //     }
        // }

        // for(size_t chunkId = chunkIdStart; chunkId <= receivedChunkList.size(); chunkId++)
        // {
        //     const auto& chunkIndex = chunkId - chunkIdStart;
        //     const auto& chunkReceived = receivedChunksList.at(chunkIndex);

        //     if(!chunkReceived)
        //     {
        //         missingChunkIds.push_back(chunkId);
        //     }
        // }
    }

    // qDebug() << QDateTime::currentDateTime() << "GetMissingChunks END! Number of missing chunks:" << missingChunkIds.size();
    if(missingChunkIds.size() > 0)
    {
        qDebug() << QDateTime::currentDateTime() << missingChunkIds.at(0) << missingChunkIds.at(missingChunkIds.size()-1);
    }

    return missingChunkIds;
}

const std::vector<QByteArray> UDPTunnelPacketChunkManager::split(const QByteArray& payload, const size_t& chunkSize)
{
    return UDPTunnelPacket::split(payload, chunkSize);
}

// Private

const bool UDPTunnelPacketChunkManager::chunkExists(const UDPTunnelPacket& chunk) const
{
    const auto& packetId = chunk.getHeader().getPacketId();
    const auto& chunkId = chunk.getHeader().getChunkId();

    if(this->chunks.find(packetId) != this->chunks.end())
    {
        const auto& bufferedChunks = this->chunks.at(packetId);
        for(const auto& bufferedChunk : bufferedChunks)
        {
            if(bufferedChunk.second.getHeader().getPacketId() == packetId && bufferedChunk.second.getHeader().getChunkId() == chunkId)
            {
                return true;
            }
        }
    }

    return false;
}
