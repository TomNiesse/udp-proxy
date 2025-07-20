#include "clientconnection.h"
#include "tcptunnelpacket.h"
#include <QTimer>

ClientConnection::ClientConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->udpTunnelConnectionSettings = std::make_unique<UDPTunnelConnectionSettings>(udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.getEgressPort());
    this->udpTunnel = std::make_unique<UDPTunnelConnection>(*this->udpTunnelConnectionSettings);

    QObject::connect(this->udpTunnel.get(), &UDPTunnelConnection::receivedData, this, &ClientConnection::handleUdpTunnelPacket, Qt::DirectConnection);
    QObject::connect(&this->tcpSocket, &QTcpSocket::connected, this, &ClientConnection::handleTcpConnect, Qt::DirectConnection);
    QObject::connect(&this->tcpSocket, &QTcpSocket::readyRead, this, &ClientConnection::handleTcpData, Qt::DirectConnection);
    QObject::connect(&this->tcpSocket, &QTcpSocket::disconnected, this, &ClientConnection::handleTcpDisconnect, Qt::DirectConnection);
}

ClientConnection::~ClientConnection()
{
    QObject::disconnect(this->udpTunnel.get(), &UDPTunnelConnection::receivedData, this, &ClientConnection::handleUdpTunnelPacket);
    QObject::disconnect(&this->tcpSocket, &QTcpSocket::connected, this, &ClientConnection::handleTcpConnect);
    QObject::disconnect(&this->tcpSocket, &QTcpSocket::readyRead, this, &ClientConnection::handleTcpData);
    QObject::disconnect(&this->tcpSocket, &QTcpSocket::disconnected, this, &ClientConnection::handleTcpDisconnect);
}

QEventLoop& ClientConnection::getEventLoop()
{
    return this->eventLoop;
}

// Protected

void ClientConnection::handleUdpTunnelPacket(const QByteArray& data)
{
    const auto& tcpTunnelPacket = TCPTunnelPacket::decode(data);
    const auto& tcpTunnelPacketHeader = tcpTunnelPacket.first;
    const auto& tcpTunnelPacketPayload = tcpTunnelPacket.second;

    switch(tcpTunnelPacketHeader.getPacketType())
    {
    case TCP_OPEN_CONNECTION:
        this->connectToHost(tcpTunnelPacketHeader.getHost(), tcpTunnelPacketHeader.getPort());
        break;
    case TCP_SEND_DATA:
        this->write(tcpTunnelPacketPayload);
        break;
    case TCP_CLOSE_CONNECTION:
        this->disconnect();
        break;
    case CLIENT_CLOSE_CONNECTION:
        this->quit();
        break;
    default:
        break;
    }
}

void ClientConnection::handleTcpConnect()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED);
    TCPTunnelPacket tcpTunnelPacket;
    const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(tcpTunnelPacketHeader, {});

    this->udpTunnel->send(tcpTunnelPacketBytes);
}

void ClientConnection::handleTcpData()
{
    while(this->tcpSocket.bytesAvailable())
    {
        const auto& payload = this->tcpSocket.readAll();

        TCPTunnelPacketHeader tcpTunnelPacketHeader;
        tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA);
        TCPTunnelPacket tcpTunnelPacket;
        const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(tcpTunnelPacketHeader, payload);

        this->udpTunnel->send(tcpTunnelPacketBytes);
    }
}

void ClientConnection::handleTcpDisconnect()
{
    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED);
    TCPTunnelPacket tcpTunnelPacket;
    const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(tcpTunnelPacketHeader, {});

    this->udpTunnel->send(tcpTunnelPacketBytes);
}

// Private

void ClientConnection::connectToHost(const QString& host, const quint16& port)
{
    this->tcpSocket.connectToHost(host, port);
    this->tcpSocket.waitForConnected(1000);
}

void ClientConnection::write(const QByteArray& data)
{
    if(this->tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        this->tcpSocket.write(data);
    }
}

void ClientConnection::disconnect()
{
    if(this->tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        this->tcpSocket.disconnectFromHost();
    }
}

void ClientConnection::quit()
{
    this->disconnect();
    this->eventLoop.quit();
}
