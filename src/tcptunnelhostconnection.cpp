#include "tcptunnelhostconnection.h"



#include <QDebug>

TCPTunnelHostConnection::TCPTunnelHostConnection(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject *parent)
    : QObject{parent}
    , tcpTunnelConnectionSettings(tcpTunnelConnectionSettings)
    , udpTunnelConnection(tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings(), tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings())
{
    QObject::connect(this, &TCPTunnelHostConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    QObject::connect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPTunnelHostConnection::handleTcpTunnelPacket);

    // Update the tcp connection settings with the values from the UDPTunnelConnection class
    auto senderUdpTunnelConnectionSettings = udpTunnelConnection.getSenderUdpTunnelConnectionSettings();
    auto receiverUdpTunnelConnectionSettings = udpTunnelConnection.getReceiverUdpTunnelConnectionSettings();

    this->tcpTunnelConnectionSettings.setHostSenderUdpTunnelConnectionSettings(senderUdpTunnelConnectionSettings);
    this->tcpTunnelConnectionSettings.setHostReceiverUdpTunnelConnectionSettings(receiverUdpTunnelConnectionSettings);
}

TCPTunnelHostConnection::~TCPTunnelHostConnection()
{
    QObject::disconnect(this, &TCPTunnelHostConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    QObject::disconnect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPTunnelHostConnection::handleTcpTunnelPacket);
}

void TCPTunnelHostConnection::connect(const QByteArray& host, const int& port)
{
    QMutexLocker lock(&this->lock);

    qDebug() << "TCPTunnelHostConnection: Send connect request for" << host << port;

    const auto& connectRequest = TCPTunnelPacket::createConnectRequest(host, port).encode();
    emit this->writeTcpTunnelPacket(connectRequest);
}

void TCPTunnelHostConnection::disconnect()
{
    QMutexLocker lock(&this->lock);

    emit this->writeTcpTunnelPacket(TCPTunnelPacket::createDisconnectRequest().encode());
}

void TCPTunnelHostConnection::write(const QByteArray& data)
{
    QMutexLocker lock(&this->lock);

    emit this->writeTcpTunnelPacket(TCPTunnelPacket::createWriteRequest(data).encode());
}

const TCPTunnelConnectionSettings TCPTunnelHostConnection::getTcpTunnelConnectionSettings() const
{
    QMutexLocker lock(&this->lock);

    return this->tcpTunnelConnectionSettings;
}

void TCPTunnelHostConnection::setTcpTunnelConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings)
{
    // Update the TCP tunnel connection settings member
    this->tcpTunnelConnectionSettings = tcpTunnelConnectionSettings;

    // Update the egress ports of the udp tunnel connection
    this->udpTunnelConnection.setSenderUdpTunnelConnectionEgressAddress(this->tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings().getEgressAddress());
    this->udpTunnelConnection.setSenderUdpTunnelConnectionEgressPort(this->tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings().getEgressPort());
    this->udpTunnelConnection.setReceiverUdpTunnelConnectionEgressAddress(this->tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings().getEgressAddress());
    this->udpTunnelConnection.setReceiverUdpTunnelConnectionEgressPort(this->tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings().getEgressPort());
}

// Public slots

void TCPTunnelHostConnection::handleTcpTunnelPacket(const QByteArray data)
{
    QMutexLocker lock(&this->lock);

    // qDebug() << "TCPTunnelHostConnection received data:" << data.size();

    TCPTunnelPacket tcpTunnelPacket(data);

    const auto& tcpTunnelPacketHeader = tcpTunnelPacket.getHeader();
    const auto& tcpTunnelPacketType = tcpTunnelPacketHeader.getPacketType();

    switch(tcpTunnelPacketType)
    {
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED:
        qDebug() << "TCPHostConnection: Client was connected! connection ID:" << this->tcpTunnelConnectionSettings.getConnectionId();
        this->handleConnected();
        break;
    case TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA:
        qDebug() << "TCPHostConnection: Client received data! connection ID:" << this->tcpTunnelConnectionSettings.getConnectionId();
        this->handleReceivedData(tcpTunnelPacket.getPayload());
        break;
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED:
        qDebug() << "TCPHostConnection: Client connection closed! connection ID:" << this->tcpTunnelConnectionSettings.getConnectionId();
        this->handleDisconnected();
        break;
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_TIMEOUT:
        qDebug() << "TCPHostConnection: Client connection TIMEOUT! connection ID:" << this->tcpTunnelConnectionSettings.getConnectionId();
        this->handleConnectionTimeout();
        break;
    case TCPTunnelPacketHeaderType::TCP_DATA_SENT:
        this->handleBytesWritten();
        break;
    case TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION:
        break;
    case TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION:
        break;
    case TCPTunnelPacketHeaderType::TCP_SEND_DATA:
        break;
    case TCPTunnelPacketHeaderType::TCP_INVALID:
        break;
    default:
        break;
    }
}

// Private slots

void TCPTunnelHostConnection::handleConnected()
{
    emit this->connected();
}

void TCPTunnelHostConnection::handleDisconnected()
{
    emit this->disconnected();
}

void TCPTunnelHostConnection::handleBytesWritten()
{
    emit this->bytesWritten();
}

void TCPTunnelHostConnection::handleReceivedData(const QByteArray& receivedData)
{
    emit this->receivedData(receivedData);
}

void TCPTunnelHostConnection::handleConnectionTimeout()
{
    emit this->connectionTimeout();
}