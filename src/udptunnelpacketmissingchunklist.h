#ifndef UDPTUNNELPACKETMISSINGCHUNKLIST_H
#define UDPTUNNELPACKETMISSINGCHUNKLIST_H

#include <QObject>
#include "udptunnelpacketheader.h"

class UDPTunnelPacketMissingChunkList : public QObject
{
    Q_OBJECT
public:
    UDPTunnelPacketMissingChunkList();
    UDPTunnelPacketMissingChunkList(const std::vector<size_t>& missingChunks);
    UDPTunnelPacketMissingChunkList(const QByteArray& encodedData);

    const std::vector<size_t> getMissingChunks() const;
    void setMissingChunks(const std::vector<size_t>& missingChunks);

    static const QByteArray encode(const std::vector<size_t>& missingChunks);
    static const std::vector<QByteArray> encode(const std::vector<std::vector<size_t>>& missingChunks);
    static const std::vector<std::vector<size_t>> split(const std::vector<size_t>& missingChunks, const size_t& numberOfMissingChunkIdsPerList);

private:
    std::vector<size_t> missingChunks;

    friend class test_udptunnelpacketmissingchunklist;
};

#endif // UDPTUNNELPACKETMISSINGCHUNKLIST_H
