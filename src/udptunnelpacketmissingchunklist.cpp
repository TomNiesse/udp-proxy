#include "udptunnelpacketmissingchunklist.h"

#include <QDebug>
#include <QDateTime>

UDPTunnelPacketMissingChunkList::UDPTunnelPacketMissingChunkList()
{

}

UDPTunnelPacketMissingChunkList::UDPTunnelPacketMissingChunkList(const std::vector<size_t>& missingChunks)
{
    this->missingChunks = missingChunks;
}

UDPTunnelPacketMissingChunkList::UDPTunnelPacketMissingChunkList(const QByteArray& encodedData)
{
    if(encodedData.size() > 0)
    {
        size_t expectedNumberOfMissingChunks = 0;
        memcpy(&expectedNumberOfMissingChunks, encodedData.mid(0, sizeof(expectedNumberOfMissingChunks)), sizeof(expectedNumberOfMissingChunks));

        if(expectedNumberOfMissingChunks > 0)
        {
            for(size_t missingChunkIndex = 1; missingChunkIndex < encodedData.size() / sizeof(expectedNumberOfMissingChunks); missingChunkIndex++)
            {
                size_t missingChunkId = 0;

                const auto& missingChunkIndexStart = sizeof(missingChunkId) * missingChunkIndex;
                const auto& missingChunkIndexEnd = sizeof(missingChunkId) * (missingChunkIndex + 1);
                memcpy(&missingChunkId, encodedData.mid(missingChunkIndexStart, missingChunkIndexEnd), sizeof(missingChunkId));

                this->missingChunks.push_back(missingChunkId);
            }
        }

        if(this->missingChunks.size() != expectedNumberOfMissingChunks)
        {
            qDebug() << QDateTime::currentDateTime() << "Expected number of missing chunks does not match received list!" << Qt::flush;
        }
    }
}

const std::vector<size_t> UDPTunnelPacketMissingChunkList::getMissingChunks() const
{
    return this->missingChunks;
}

void UDPTunnelPacketMissingChunkList::setMissingChunks(const std::vector<size_t>& missingChunks)
{
    this->missingChunks = missingChunks;
}

const QByteArray UDPTunnelPacketMissingChunkList::encode(const std::vector<size_t>& missingChunks)
{
    QByteArray out;

    // Add the number of missing packets to the output
    const size_t numberOfMissingChunks = missingChunks.size();
    uint8_t encodedNumberOfMissingChunks[sizeof(numberOfMissingChunks)] = {0};
    memcpy(encodedNumberOfMissingChunks, &numberOfMissingChunks, sizeof(numberOfMissingChunks));
    out.append(QByteArray((char*)encodedNumberOfMissingChunks, sizeof(numberOfMissingChunks)));

    // Add the missing chunk ID to the output
    for(const auto& missingChunk : missingChunks)
    {
        uint8_t encodedMissingChunk[sizeof(missingChunk)] = {0};
        memcpy(encodedMissingChunk, &missingChunk, sizeof(missingChunk));
        out.append(QByteArray((char*)encodedMissingChunk, sizeof(missingChunk)));
    }

    return out;
}

const std::vector<QByteArray> UDPTunnelPacketMissingChunkList::encode(const std::vector<std::vector<size_t>>& missingChunksLists)
{
    std::vector<QByteArray> out;

    // Encode all missing chunk ID lists
    for(const auto& missingChunksList : missingChunksLists)
    {
        const auto& encodedMissingChunksList = encode(missingChunksList);
        out.push_back(encodedMissingChunksList);
    }

    // If there are no missing chunk ID's, encode an empty list, without any chunk ID's
    if(out.size() <= 0)
    {
        out.push_back(encode(std::vector<size_t>({})));
    }

    return out;
}

const std::vector<std::vector<size_t>> UDPTunnelPacketMissingChunkList::split(const std::vector<size_t>& missingChunks, const size_t& numberOfMissingChunkIdsPerList)
{
    std::vector<std::vector<size_t>> splitLists;

    for (size_t startIndex = 0; startIndex < missingChunks.size(); startIndex += numberOfMissingChunkIdsPerList)
    {
        std::vector<size_t> currentList;
        size_t endIndex = startIndex + numberOfMissingChunkIdsPerList;

        if (endIndex > missingChunks.size())
        {
            endIndex = missingChunks.size();
        }

        for (size_t currentIndex = startIndex; currentIndex < endIndex; ++currentIndex)
        {
            currentList.push_back(missingChunks.at(currentIndex));
        }

        splitLists.push_back(currentList);
    }

    return splitLists;
}