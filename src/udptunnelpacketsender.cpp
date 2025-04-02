#include "udptunnelpacketsender.h"
#include "udptunnelpacket.h"
#include "limits.h"
#include <QEventLoop>
#include <QTimer>
#include <QNetworkDatagram>

#include <QCoreApplication>
#include <QDateTime>

UDPTunnelPacketSender::UDPTunnelPacketSender(const QHostAddress listenAddress, const quint16 listenPort, const QHostAddress egressAddress, const quint16 egressPort)
{
    const QMutexLocker lock(&this->lock);

    this->listenAddress = listenAddress;
    this->listenPort = listenPort;
    this->egressAddress = egressAddress;
    this->egressPort = egressPort;

    this->ingressSocket = std::make_unique<QUdpSocket>();
    const bool listeningForTraffic = this->ingressSocket->bind(this->listenAddress, this->listenPort);
    if(!listeningForTraffic)
    {
        qDebug() << QString("Could not open %1:%2. Exiting").arg(this->listenAddress.toString()).arg(this->listenPort);
        exit(EXIT_FAILURE);
    }
}

void UDPTunnelPacketSender::write(const QByteArray packet)
{
    const QMutexLocker lock(&this->lock);

    do
    {
        QUdpSocket egressSocket;
        egressSocket.writeDatagram(packet, packet.size(), this->egressAddress, this->egressPort);
        egressSocket.flush();
    } while (!this->ingressSocket->waitForReadyRead(MAX_ROUNDTRIP_TIMEOUT));

    this->handleResponse();
}

void UDPTunnelPacketSender::handleResponse()
{
    while (this->ingressSocket->hasPendingDatagrams())
    {
        const auto datagram = this->ingressSocket->receiveDatagram();
        const auto packet = UDPTunnelPacket(datagram.data());
        switch(packet.getHeader().getPacketType())
        {
        case UDP_DATA_RECEIVED_ACKNOWLEDGE:
        case UDP_DATA_FLUSH_RECEIVED_ACKNOWLEDGE:
            QTimer::singleShot(0, this, [this](){
                emit receivedResponse();
            });
            break;
        default:
            break;
        }
    }
}
