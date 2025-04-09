#include "udptunnelconnection.h"
#include "udptunnelpacket.h"
#include "limits.h"
#include <QUdpSocket>
#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QElapsedTimer>
#include <thread>
#include <QDateTime>
#include <QTimer>

UDPTunnelConnection::UDPTunnelConnection(const UDPTunnelConnectionSettings& connectionSettings, const QString& name)
{
    const QMutexLocker lock(&this->lock);

    this->connectionSettings = connectionSettings;
    this->name = name;

    const auto senderIngressAddress = this->connectionSettings.senderIngressAddress;
    const auto senderIngressPort = this->connectionSettings.senderIngressPort;
    const auto senderEgressAddress = this->connectionSettings.senderEgressAddress;
    const auto senderEgressPort = this->connectionSettings.senderEgressPort;
    const auto receiverIngressAddress = this->connectionSettings.receiverIngressAddress;
    const auto receiverIngressPort = this->connectionSettings.receiverIngressPort;
    const auto receiverEgressAddress = this->connectionSettings.receiverEgressAddress;
    const auto receiverEgressPort = this->connectionSettings.receiverEgressPort;

    this->sender = std::make_unique<UDPTunnelPacketSender>(senderIngressAddress, senderIngressPort, senderEgressAddress, senderEgressPort);
    this->receiver = std::make_unique<UDPTunnelPacketReceiver>(receiverIngressAddress, receiverIngressPort, receiverEgressAddress, receiverEgressPort);

    connect(this->receiver.get(), &UDPTunnelPacketReceiver::bytesReceived, this, [this](const QByteArray& bytes){
        emit this->bytesReceived(bytes);
    });
}

void UDPTunnelConnection::send(const QByteArray& payload)
{
    const QMutexLocker lock(&this->lock);

    // Split the payload into chunks
    const auto& payloadChunks = UDPTunnelPacket::split(payload, UDPTUNNEL_PAYLOAD_SIZE);
    UDPTunnelPacketHeader header;
    header.setPacketType(UDPTunnelPacketType::UDP_DATA);
    header.setPacketId(this->packetId++);
    header.setChunkId(0);
    header.setLastSegment(false);
    const auto& encodedChunks = UDPTunnelPacket::addHeaders(header, payloadChunks);
    this->packetId += encodedChunks.size();

    QTimer::singleShot(0, this, [this, encodedChunks](){
        for (const auto& encodedChunk : encodedChunks)
        {
            this->sender->write(encodedChunk);
        }
    });

}

const std::unique_ptr<UDPTunnelPacketSender>& UDPTunnelConnection::getSender()
{
    const QMutexLocker lock(&this->lock);

    return this->sender;
}

const std::unique_ptr<UDPTunnelPacketReceiver>& UDPTunnelConnection::getReceiver()
{
    const QMutexLocker lock(&this->lock);

    return this->receiver;
}
