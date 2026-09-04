#ifndef UDPTUNNELPACKETBUFFER_H
#define UDPTUNNELPACKETBUFFER_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <memory>
#include "udptunnelpacket.h"

class UDPTunnelPacketBuffer : public QObject
{
    Q_OBJECT
public:
    explicit UDPTunnelPacketBuffer(QObject *parent = nullptr);

    void setUdpTunnelPackets(const std::vector<UDPTunnelPacket>& udpTunnelPackets);
    void clearUdpTunnelPackets();

    const std::vector<UDPTunnelPacket> getInitialPacketChunks();
    const std::vector<UDPTunnelPacket> handleReceivedPacket(const UDPTunnelPacket& udpTunnelPacket);
    void increaseBufferSize(const size_t& amount);
    void decreaseBufferSize(const size_t& amount);
    void packetSent(const UDPTunnelPacket& udpTunnelPacket);

    const size_t getBufferSize() const;
    const size_t getChunkIdStart() const;
    const size_t getChunkIdEnd() const;

signals:
    void resendPacket(const UDPTunnelPacket& udpTunnelPacket);

private:
    void removeFromBuffer(const UDPTunnelPacket& udpTunnelPacket);
    const std::vector<UDPTunnelPacket> updateBufferAndGetNewChunks();

    size_t bufferSize = 2;
    size_t chunkIndex;
    std::vector<std::pair<UDPTunnelPacket, std::unique_ptr<QTimer>>> buffer;
    std::vector<UDPTunnelPacket> udpTunnelPackets;
    mutable QMutex lock;

};

#endif // UDPTUNNELPACKETBUFFER_H
