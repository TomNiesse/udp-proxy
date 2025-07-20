#ifndef UDPTUNNELPACKETRECEIVER_H
#define UDPTUNNELPACKETRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include "udptunnelpacketchunkmananger.h"

class UDPTunnelPacketReceiver : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelPacketReceiver)
public:
    explicit UDPTunnelPacketReceiver(const QHostAddress& listenAddress, const quint16& listenPort, const QHostAddress& egressAddress, const quint16& egressPort);
    ~UDPTunnelPacketReceiver();

signals:
    void dataReceived(const QByteArray& bytes);

private slots:
    void handleReceivedBytes();

private:
    QUdpSocket ingressSocket;
    QHostAddress listenAddress;
    quint16 listenPort;
    QHostAddress egressAddress;
    quint16 egressPort;
    UDPTunnelPacketChunkMananger chunkManager;
    QMutex lock;
};

#endif // UDPTUNNELPACKETRECEIVER_H
