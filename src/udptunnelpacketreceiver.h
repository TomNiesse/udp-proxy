#ifndef UDPTUNNELPACKETRECEIVER_H
#define UDPTUNNELPACKETRECEIVER_H

#include "iudptunnelpackethandler.h"
#include "udptunnelpacketchunkmanager.h"
#include "udptunnelconnectionsettings.h"
#include <QObject>
#include <QUdpSocket>

class UDPTunnelPacketReceiver : public IUDPTunnelPacketHandler
{
    Q_OBJECT
public:
    explicit UDPTunnelPacketReceiver(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, QObject *parent = nullptr);
    ~UDPTunnelPacketReceiver();

    const UDPTunnelConnectionSettings getUdpTunnelConnectionSettings() const;
    void setEgressAddress(const QByteArray& egressAddress);
    void setEgressPort(const int& egressPort);

signals:
    void receivedData(const QByteArray receivedData);

private:
    const std::vector<UDPTunnelPacket> handleUdpTunnelPacket(const UDPTunnelPacket& udpTunnelPacket) override;
    const bool getReceiveData() const;
    const size_t getPacketId() const;
    const size_t getChunkIdStart() const;
    const size_t getChunkIdEnd() const;
    const UDPTunnelPacketChunkManager& getUdpTunnelPacketChunkManager() const;

    inline void startHandlingPackets();
    inline void setBufferSizes();

private:
    bool receiveData = false;
    size_t packetId = 0;
    size_t chunkIdStart = 0;
    size_t chunkIdEnd = 0;
    UDPTunnelPacketChunkManager udpTunnelPacketChunkManager;

    QMutex receiveBufferLock;
    std::vector<QNetworkDatagram> receiveBuffer;
    bool handlingReceivedData = false;

    QUdpSocket ingressSocket;
    QUdpSocket egressSocket;

    UDPTunnelConnectionSettings udpTunnelConnectionSettings;

    size_t highestContiguousChunkId = 0;
};

#endif // UDPTUNNELPACKETRECEIVER_H
