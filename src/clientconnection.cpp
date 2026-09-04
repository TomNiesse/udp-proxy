#include "clientconnection.h"
#include "tcptunnelpacket.h"
#include <QTimer>



#include <QDebug>
#include <QDateTime>

ClientConnection::ClientConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings)
    : senderUdpTunnelConnectionSettings(senderUdpTunnelConnectionSettings)
    , receiverUdpTunnelConnectionSettings(receiverUdpTunnelConnectionSettings)
{
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(this->senderUdpTunnelConnectionSettings, this->receiverUdpTunnelConnectionSettings);

    QObject::connect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ClientConnection::handleUdpTunnelPacket);
    QObject::connect(&this->tcpSocket, &QTcpSocket::connected, this, &ClientConnection::handleTcpConnect);
    QObject::connect(&this->tcpSocket, &QTcpSocket::readyRead, this, &ClientConnection::handleTcpData);
    QObject::connect(&this->tcpSocket, &QTcpSocket::disconnected, this, &ClientConnection::handleTcpDisconnect);
}

ClientConnection::~ClientConnection()
{
    QObject::disconnect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ClientConnection::handleUdpTunnelPacket);
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
    const auto& tcpTunnelPacket = TCPTunnelPacket(data);
    const auto& tcpTunnelPacketHeader = tcpTunnelPacket.getHeader();
    const auto& tcpTunnelPacketPayload = tcpTunnelPacket.getPayload();

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
    qDebug() << QDateTime::currentDateTime() << "Handle TCP connect";

    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED);
    TCPTunnelPacket tcpTunnelPacket;
    const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(/*tcpTunnelPacketHeader, {}*/);

    this->udpTunnelConnection->sendData(tcpTunnelPacketBytes);
}

void ClientConnection::handleTcpData()
{
    while(this->tcpSocket.bytesAvailable())
    {
        const auto& payload = this->tcpSocket.readAll();

        qDebug() << QDateTime::currentDateTime() << "Handle received:" << payload.size();

        TCPTunnelPacketHeader tcpTunnelPacketHeader;
        tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA);
        TCPTunnelPacket tcpTunnelPacket;
        const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(/*tcpTunnelPacketHeader, payload*/);

        this->udpTunnelConnection->sendData(tcpTunnelPacketBytes);
    }
}

void ClientConnection::handleTcpDisconnect()
{
    qDebug() << QDateTime::currentDateTime() << "Handle TCP disconnect";

    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED);
    TCPTunnelPacket tcpTunnelPacket;
    const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(/*tcpTunnelPacketHeader, {}*/);

    this->udpTunnelConnection->sendData(tcpTunnelPacketBytes);
}

// Private

void ClientConnection::connectToHost(const QString& host, const quint16& port)
{
    qDebug() << QDateTime::currentDateTime() << "Connect to" << host << port;

    this->tcpSocket.connectToHost(host, port);
    this->tcpSocket.waitForConnected(1000);
}

void ClientConnection::write(const QByteArray& data)
{
    if(this->tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << QDateTime::currentDateTime() << "Write data:" << data.size();
        this->tcpSocket.write(data);
    }
}

void ClientConnection::disconnect()
{
    if(this->tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << QDateTime::currentDateTime() << "Disconnect";
        this->tcpSocket.disconnectFromHost();
    }
}

void ClientConnection::quit()
{
    qDebug() << QDateTime::currentDateTime() << "Quit";

    this->disconnect();
    this->eventLoop.quit();
}
