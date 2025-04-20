#include "tcpproxyconnectionmanager.h"
#include "tcptunnelpacket.h"
#include "limits.h"
#include <thread>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QTimer>

TCPProxyConnectionManager::TCPProxyConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    const QMutexLocker lock(&this->lock);

    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(udpTunnelConnectionSettings);
    QObject::connect(this->udpTunnelConnection.get(), &UDPTunnelConnection::bytesReceived, this, &TCPProxyConnectionManager::handleReceivedBytes);
}

TCPProxyConnectionManager::~TCPProxyConnectionManager()
{
    const QMutexLocker lock(&this->lock);

    QObject::disconnect(this->udpTunnelConnection.get(), &UDPTunnelConnection::bytesReceived, this, nullptr);
}

void TCPProxyConnectionManager::connectToHost(const size_t& connectionId, const QByteArray& host, const quint16& port)
{
    const QMutexLocker lock(&this->lock);

    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_COMMAND_OPEN_CONNECTION);
    header.setConnectionId(connectionId);
    header.setHost(host);
    header.setPort(port);

    const auto encodedData = TCPTunnelPacket::encode(header, {});
    this->udpTunnelConnection->send(encodedData);
}

void TCPProxyConnectionManager::write(const size_t& connectionId, const QByteArray& payload)
{
    const QMutexLocker lock(&this->lock);

    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_COMMAND_SEND_DATA);
    header.setConnectionId(connectionId);

    const auto& encodedData = TCPTunnelPacket::encode(header, payload);
    this->udpTunnelConnection->send(encodedData);
}

void TCPProxyConnectionManager::disconnect(const size_t& connectionId)
{
    const QMutexLocker lock(&this->lock);

    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_COMMAND_CLOSE_CONNECTION);
    header.setConnectionId(connectionId);

    const auto& encodedData = TCPTunnelPacket::encode(header, {});
    this->udpTunnelConnection->send(encodedData);
}

// Private

void TCPProxyConnectionManager::handleReceivedBytes(const QByteArray& packet)
{
    const auto& decodedPacket = TCPTunnelPacket::decode(packet);
    const auto& packetHeader = decodedPacket.first;
    const auto& connectionId = packetHeader.getConnectionId();
    const auto& payload = decodedPacket.second;

    switch(packetHeader.getPacketType())
    {
    case TCPTunnelPacketHeaderType::TCP_RESPONSE_OPENED_CONNECTION:
        QTimer::singleShot(0, this, [this, connectionId](){
            emit this->connected(connectionId);
        });
        break;
    case TCPTunnelPacketHeaderType::TCP_RESPONSE_CLOSED_CONNECTION:
        QTimer::singleShot(0, this, [this, connectionId](){
            emit this->disconnected(connectionId);
        });
        break;
    case TCPTunnelPacketHeaderType::TCP_RESPONSE_RECEIVED_DATA:
        QTimer::singleShot(0, this, [this, connectionId, payload](){
            emit this->bytesReceived(connectionId, payload);
        });
        break;
    case TCPTunnelPacketHeaderType::TCP_RESPONSE_SENT_DATA:
        QTimer::singleShot(0, this, [this, connectionId](){
            emit this->bytesWritten(connectionId);
        });
        break;
    default:
        break;
    }
}
