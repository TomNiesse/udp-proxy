// #ifndef UDPTUNNELPACKETTRANSCEIVER_H
// #define UDPTUNNELPACKETTRANSCEIVER_H

// #include "udptunnelpacket.h"
// #include "udptunnelpacketchunkmanager.h"
// #include <QByteArray>
// #include <QMutex>
// #include <QUdpSocket>

// class UDPTunnelPacketTransceiver : public QObject
// {
//     Q_OBJECT
//     Q_DISABLE_COPY_MOVE(UDPTunnelPacketTransceiver)
// public:
//     explicit UDPTunnelPacketTransceiver(const QString& ingressAddress, const uint16_t& ingressPort, const QString& egressAddress, const uint16_t& egressPort);
//     ~UDPTunnelPacketTransceiver();
//     void send(const QByteArray& data);

// signals:
//     void sendTimeout();
//     void acknowledgementReceived();
//     void receivedData(const QByteArray& data);

// private slots:
//     void sendData(const int& timeout = 1);
//     void handleReceivedUdpData();
//     void bufferEgressUdpData(const QByteArray& data);
//     void bufferReceivedUdpData();

// private:
//     void handleReceivedData(const UDPTunnelPacket& udpTunnelPacket);
//     void handleReceivedDataFlush(const UDPTunnelPacket& udpTunnelPacket);
//     void handleReceivedUdpAcknowledgement();
//     void sendUdpAcknowledgement(const UDPTunnelPacket& udpTunnelPacket);

//     QString ingressAddress;
//     uint16_t ingressPort = 0;
//     QString egressAddress;
//     uint16_t egressPort = 0;
//     uint16_t maxChunks = 100;

//     QMutex lock;
//     QUdpSocket ingressSocket;
//     std::vector<QByteArray> ingressBuffer;
//     QMutex ingressBufferLock;
//     bool ingressBusy = false;
//     QMutex ingressBusyLock;
//     std::vector<QByteArray> egressBuffer;
//     QMutex egressBufferLock;
//     bool egressBusy = false;
//     QMutex egressBusyLock;

//     UDPTunnelPacketChunkManager udpTunnelPacketChunkManager;
// };

// #endif // UDPTUNNELPACKETTRANSCEIVER_H