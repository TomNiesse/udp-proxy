#ifndef UDPTUNNELPACKETTRANSCEIVER_H
#define UDPTUNNELPACKETTRANSCEIVER_H

#include "udptunnelpacket.h"
#include "udptunnelpacketchunkmananger.h"
#include <QByteArray>
#include <QMutex>
#include <QUdpSocket>

class UDPTunnelPacketTransceiver : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelPacketTransceiver)
public:
    explicit UDPTunnelPacketTransceiver(const QString& ingressAddress, const uint16_t& ingressPort, const QString& egressAddress, const uint16_t& egressPort);
    ~UDPTunnelPacketTransceiver();
    void send(const QByteArray& data);

signals:
    void sendTimeout();
    void acknowledgementReceived();
    void receivedData(const QByteArray& data);

private slots:
    void sendData(const int& timeout = 1);
    void handleReceivedUdpData();
    void bufferEgressUdpData(const QByteArray& data);
    void bufferReceivedUdpData();

private:
    void handleReceivedData(const UDPTunnelPacket& udpTunnelPacket);
    void handleReceivedDataFlush(const UDPTunnelPacket& udpTunnelPacket);
    void handleReceivedUdpAcknowledgement();
    void sendUdpAcknowledgement(const UDPTunnelPacket& udpTunnelPacket);

    QString ingressAddress;
    uint16_t ingressPort = 0;
    QString egressAddress;
    uint16_t egressPort = 0;

    QMutex lock;
    QUdpSocket ingressSocket;
    std::vector<QByteArray> ingressBuffer;
    QMutex ingressBufferLock;
    bool ingressBusy = false;
    QMutex ingressBusyLock;
    std::vector<QByteArray> egressBuffer;
    QMutex egressBufferLock;
    bool egressBusy = false;
    QMutex egressBusyLock;

    UDPTunnelPacketChunkMananger udpTunnelPacketChunkManager;
};

#endif

// #include "tcptunnelpacket.h"
// #include "udptunnelpacketchunkmananger.h"
// #include <QUdpSocket>

// class UDPTunnelPacketTransceiver : public QObject
// {
//     Q_OBJECT
//     Q_DISABLE_COPY_MOVE(UDPTunnelPacketTransceiver)
// public:
//     UDPTunnelPacketTransceiver(const QString& ingressAddress, const int& ingressPort, const QString& egressAddress, const int& egressPort);
//     ~UDPTunnelPacketTransceiver();
//     void send(const QByteArray data, const int timeout = 100);

// signals:
//     void receivedData(const QByteArray data);
//     void acknowledgementReceived();
//     void sendData(const QByteArray data);
//     void sendTimeout();

// private:
//     void handleReceivedData();
//     void handleReceivedUdpData(const UDPTunnelPacket& udpTunnelPacket);
//     void handleReceivedUdpDataFlush(const UDPTunnelPacket& udpTunnelPacket);
//     void handleReceivedUdpAcknowledgement(const UDPTunnelPacket& udpTunnelPacket);
//     void sendUdpAcknowledgement(const UDPTunnelPacket& udpTunnelPacket);

//     void sendUdpTunnelDataThread();
//     std::vector<QByteArray> egressBuffer;
//     bool busy;

//     QString ingressAddress;
//     int ingressPort;
//     QString egressAddress;
//     int egressPort;
//     QMutex lock;

//     QUdpSocket ingressSocket;
//     UDPTunnelPacketChunkMananger udpTunnelPacketChunkManager;
// };

// #endif




// // #ifndef UDPTUNNELPACKETSENDER_H
// // #define UDPTUNNELPACKETSENDER_H
// // #include <QObject>
// // #include <QHostAddress>
// // #include <QUdpSocket>
// // #include <QMutex>
// // #include <QEventLoop>

// // class UDPTunnelPacketSender : public QObject
// // {
// //     Q_OBJECT
// // public:
// //     explicit UDPTunnelPacketSender(const QHostAddress& listenAddress, const quint16& listenPort, const QHostAddress& egressAddress, const quint16& egressPort);
// //     void send(const QByteArray& packet);

// // signals:
// //     void receivedResponse();

// // private:
// //     void handleResponse();

// // private:
// //     QHostAddress listenAddress;
// //     quint16 listenPort = 0;
// //     QHostAddress egressAddress;
// //     quint16 egressPort = 0;
// //     std::unique_ptr<QUdpSocket> ingressSocket;
// //     QMutex lock;
// // };

// // #endif // UDPTUNNELPACKETTRANSCEIVER_H
