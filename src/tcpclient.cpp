#include "tcpclient.h"
#include "tcptunnelpacket.h"
#include "limits.h"
#include <QCoreApplication>
#include <QTimer>

TCPClient::TCPClient()
{

}

void TCPClient::handleUdpTunnelPacket(const QByteArray& packet)
{
    const QMutexLocker lock(&this->lock);

    const auto& decodedData = TCPTunnelPacket::decode(packet);
    const auto& header = decodedData.first;
    const auto& packetType = header.getPacketType();
    const auto& connectionId = header.getConnectionId();
    const auto& host = header.getHost();
    const auto& port = header.getPort();
    const auto& payload = decodedData.second;

    switch(packetType)
    {
    case TCPTunnelPacketHeaderType::TCP_COMMAND_OPEN_CONNECTION:
        QTimer::singleShot(0, this, [this, connectionId, host, port](){
            this->connectToHost(connectionId, host, port);
        });
        break;
    case TCPTunnelPacketHeaderType::TCP_COMMAND_CLOSE_CONNECTION:
        QTimer::singleShot(0, this, [this, connectionId](){
            this->disconnect(connectionId);
        });
        break;
    case TCPTunnelPacketHeaderType::TCP_COMMAND_SEND_DATA:
        QTimer::singleShot(0, this, [this, connectionId, payload](){
            this->write(connectionId, payload);
        });
        break;
    default:
        break;
    }
}

void TCPClient::connectToHost(const size_t& connectionId, const QString& address, const quint16& port)
{
    const QMutexLocker lock(&this->lock);

    if(this->tcpConnections.find(connectionId) == this->tcpConnections.end())
    {
        this->tcpConnections.emplace(connectionId, std::make_unique<QTcpSocket>());

        QObject::connect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::connected, this, [this, connectionId](){
            QTimer::singleShot(0, this, [this, connectionId](){
                emit this->connected(connectionId);
            });
        });
        QObject::connect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::disconnected, this, [this, connectionId](){
            QTimer::singleShot(0, this, [this, connectionId](){
                emit this->disconnected(connectionId);
            });
        });
        QObject::connect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::bytesWritten, this, [this, connectionId](){
            QTimer::singleShot(0, this, [this, connectionId](){
                emit this->bytesWritten(connectionId);
            });
        });
        QObject::connect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::readyRead, this, [this, connectionId](){
            while(this->tcpConnections.at(connectionId).get()->bytesAvailable())
            {
                const QByteArray& packet = this->tcpConnections.at(connectionId)->readAll();
                QTimer::singleShot(0, this, [this, connectionId, packet](){
                    emit this->bytesReceived(connectionId, packet);
                });
            }
        });

        this->tcpConnections.at(connectionId)->setSocketOption(QAbstractSocket::KeepAliveOption, true);
        this->tcpConnections.at(connectionId)->setSocketOption(QAbstractSocket::LowDelayOption, true);
        this->tcpConnections.at(connectionId)->connectToHost(address, port);
    }
}

void TCPClient::disconnect(const size_t& connectionId)
{
    const QMutexLocker lock(&this->lock);

    // Make sure the connection exists
    if(this->tcpConnections.find(connectionId) != this->tcpConnections.end())
    {
        // Close the connection
        this->tcpConnections.at(connectionId)->disconnect();
        QObject::disconnect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::connected, this, nullptr);
        QObject::disconnect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::disconnected, this, nullptr);
        QObject::disconnect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::readyRead, this, nullptr);
        QObject::disconnect(this->tcpConnections.at(connectionId).get(), &QTcpSocket::bytesWritten, this, nullptr);
        this->tcpConnections.erase(connectionId);
    }
}

void TCPClient::write(const size_t& connectionId, const QByteArray& packet)
{
    const QMutexLocker lock(&this->lock);

    if(this->tcpConnections.find(connectionId) != this->tcpConnections.end())
    {
        this->tcpConnections.at(connectionId)->write(packet, packet.size());
    }
}
