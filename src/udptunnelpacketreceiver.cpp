#include "udptunnelpacketreceiver.h"
#include "udptunnelpacket.h"
#include "limits.h"
#include <thread>
#include <QNetworkDatagram>
#include <QTimer>
#include <QDateTime>

UDPTunnelPacketReceiver::UDPTunnelPacketReceiver(const QHostAddress& listenAddress, const quint16& listenPort, const QHostAddress& egressAddress, const quint16& egressPort)
{
    this->listenAddress = listenAddress;
    this->listenPort = listenPort;
    this->egressAddress = egressAddress;
    this->egressPort = egressPort;

    QObject::connect(&this->ingressSocket, &QUdpSocket::readyRead, this, &UDPTunnelPacketReceiver::handleReceivedBytes);

    const bool& listeningForTraffic = ingressSocket.bind(this->listenAddress, this->listenPort);
    if(!listeningForTraffic)
    {
        qDebug() << QString("Could not open %1:%2. Exiting").arg(this->listenAddress.toString()).arg(this->listenPort);
        exit(EXIT_FAILURE);
    }
    else
    {
        qDebug() << "Receiver: listening for UDP tunnel traffic on" << this->listenAddress << this->listenPort;
    }
}

UDPTunnelPacketReceiver::~UDPTunnelPacketReceiver()
{
    QObject::disconnect(&this->ingressSocket, &QUdpSocket::readyRead, this, &UDPTunnelPacketReceiver::handleReceivedBytes);

}

// Private

void UDPTunnelPacketReceiver::handleReceivedBytes()
{
    while(ingressSocket.hasPendingDatagrams())
    {
        const auto& datagram = ingressSocket.receiveDatagram();
        const auto& message = datagram.data();

        auto packet = UDPTunnelPacket(message);
        auto header = packet.getHeader();

        QByteArray restoredPacket;

        switch(header.getPacketType())
        {
        case UDP_DATA:
            // Set return packet type
            header.setPacketType(UDP_DATA_RECEIVED_ACKNOWLEDGE);

            // Give the packet to the chunk manager
            this->lock.lock();
            this->chunkManager.addChunk(packet);
            this->lock.unlock();
            break;
        case UDP_DATA_FLUSH:
            // Set return packet type
            header.setPacketType(UDP_DATA_FLUSH_RECEIVED_ACKNOWLEDGE);

            // Tell the chunk manager to convert all received chunks to a TCP tunnel payload
            this->lock.lock();
            restoredPacket = this->chunkManager.convertChunksToPayload(packet.getHeader().getPacketId());
            this->lock.unlock();
            // Let other software components know that a TCP tunnel packet has arrived
            QTimer::singleShot(0, this, [this, restoredPacket](){
                emit this->dataReceived(restoredPacket);
            });
            break;
        default:
            break;
        }

        // Send a response
        packet.setHeader(header);
        const auto& encodedPacket = packet.encode();

        QUdpSocket egressSocket;
        egressSocket.setSocketOption(QAbstractSocket::LowDelayOption, true);
        egressSocket.writeDatagram(encodedPacket, this->egressAddress, this->egressPort);
        egressSocket.flush();
    }
}
