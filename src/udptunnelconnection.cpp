#include "udptunnelconnection.h"
#include "udptunnelpacket.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QElapsedTimer>

#include <QDebug>

UDPTunnelConnection::UDPTunnelConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, const QString& name)
    : senderUdpTunnelConnectionSettings(senderUdpTunnelConnectionSettings)
    , receiverUdpTunnelConnectionSettings(receiverUdpTunnelConnectionSettings)
    , name(name)
{
    // Create a sender and receiver
    this->udpTunnelPacketSender = std::make_unique<UDPTunnelPacketSender>(this->senderUdpTunnelConnectionSettings);
    this->udpTunnelPacketReceiver = std::make_unique<UDPTunnelPacketReceiver>(this->receiverUdpTunnelConnectionSettings);

    // When the sender has sent data, emit a signal
    QObject::connect(this->udpTunnelPacketSender.get(), &UDPTunnelPacketSender::dataSent, this, &UDPTunnelConnection::dataSent);

    // When the sender doesn't get a response within the given time, emit a timeout signal
    QObject::connect(this->udpTunnelPacketSender.get(), &UDPTunnelPacketSender::responseTimeoutOccurred, this, &UDPTunnelConnection::responseTimeoutOccurred);

    // When the receiver receives data, emit a signal
    QObject::connect(this->udpTunnelPacketReceiver.get(), &UDPTunnelPacketReceiver::receivedData, this, &UDPTunnelConnection::receivedData);
}

UDPTunnelConnection::~UDPTunnelConnection()
{
    QObject::disconnect(this->udpTunnelPacketSender.get(), &UDPTunnelPacketSender::dataSent, this, &UDPTunnelConnection::dataSent);
    QObject::disconnect(this->udpTunnelPacketSender.get(), &UDPTunnelPacketSender::responseTimeoutOccurred, this, &UDPTunnelConnection::responseTimeoutOccurred);
    QObject::disconnect(this->udpTunnelPacketReceiver.get(), &UDPTunnelPacketReceiver::receivedData, this, &UDPTunnelConnection::receivedData);
}

const UDPTunnelConnectionSettings UDPTunnelConnection::getSenderUdpTunnelConnectionSettings() const
{
    return this->udpTunnelPacketSender->getUdpTunnelConnectionSettings();
}

const UDPTunnelConnectionSettings UDPTunnelConnection::getReceiverUdpTunnelConnectionSettings() const
{
    return this->udpTunnelPacketReceiver->getUdpTunnelConnectionSettings();
}

void UDPTunnelConnection::setSenderUdpTunnelConnectionEgressAddress(const QByteArray& senderUdpTunnelConnectionEgressAddress)
{
    this->senderUdpTunnelConnectionSettings.setEgressAddress(senderUdpTunnelConnectionEgressAddress);
    this->udpTunnelPacketSender->setEgressAddress(senderUdpTunnelConnectionEgressAddress);
}

void UDPTunnelConnection::setSenderUdpTunnelConnectionEgressPort(const int& senderUdpTunnelConnectionEgressPort)
{
    this->senderUdpTunnelConnectionSettings.setEgressPort(senderUdpTunnelConnectionEgressPort);
    this->udpTunnelPacketSender->setEgressPort(senderUdpTunnelConnectionEgressPort);
}

void UDPTunnelConnection::setReceiverUdpTunnelConnectionEgressAddress(const QByteArray& receiverUdpTunnelConnectionEgressAddress)
{
    this->receiverUdpTunnelConnectionSettings.setEgressAddress(receiverUdpTunnelConnectionEgressAddress);
    this->udpTunnelPacketReceiver->setEgressAddress(receiverUdpTunnelConnectionEgressAddress);
}

void UDPTunnelConnection::setReceiverUdpTunnelConnectionEgressPort(const int& receiverUdpTunnelConnectionEgressPort)
{
    this->receiverUdpTunnelConnectionSettings.setEgressPort(receiverUdpTunnelConnectionEgressPort);
    this->udpTunnelPacketReceiver->setEgressPort(receiverUdpTunnelConnectionEgressPort);
}

void UDPTunnelConnection::sendData(const QByteArray payload)
{
    this->udpTunnelPacketSender->sendData(payload);
}