#include "tcptunnelclientconnection.h"

TCPTunnelClientConnection::TCPTunnelClientConnection(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject *parent)
    : QObject{parent}
    , socket(QTcpSocket())
    , tcpTunnelConnectionSettings(tcpTunnelConnectionSettings)
    , udpTunnelConnection(tcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings(), tcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings())
{
    // Update the tcp connection settings
    auto clientSenderUdpTunnelConnectionSettings = this->tcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings();
    clientSenderUdpTunnelConnectionSettings.setIngressPort(this->udpTunnelConnection.getSenderUdpTunnelConnectionSettings().getIngressPort());
    clientSenderUdpTunnelConnectionSettings.setEgressPort(this->tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings().getIngressPort());
    this->tcpTunnelConnectionSettings.setClientSenderUdpTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettings);

    auto clientReceiverUdpTunnelConnectionSettings = this->tcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings();
    clientReceiverUdpTunnelConnectionSettings.setIngressPort(this->udpTunnelConnection.getReceiverUdpTunnelConnectionSettings().getIngressPort());
    clientReceiverUdpTunnelConnectionSettings.setEgressPort(this->tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings().getIngressPort());
    this->tcpTunnelConnectionSettings.setClientReceiverUdpTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettings);

    auto hostSenderUdpTunnelConnectionSettings = tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings();
    hostSenderUdpTunnelConnectionSettings.setEgressPort(clientReceiverUdpTunnelConnectionSettings.getIngressPort());
    this->tcpTunnelConnectionSettings.setHostSenderUdpTunnelConnectionSettings(hostSenderUdpTunnelConnectionSettings);

    auto hostReceiverUdpTunnelConnectionSettings = tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings();
    hostReceiverUdpTunnelConnectionSettings.setEgressPort(clientSenderUdpTunnelConnectionSettings.getIngressPort());
    this->tcpTunnelConnectionSettings.setHostReceiverUdpTunnelConnectionSettings(hostReceiverUdpTunnelConnectionSettings);

    // Update the egress ports of the udp tunnel connection
    this->udpTunnelConnection.setSenderUdpTunnelConnectionEgressAddress(this->tcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings().getEgressAddress());
    this->udpTunnelConnection.setSenderUdpTunnelConnectionEgressPort(this->tcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings().getEgressPort());
    this->udpTunnelConnection.setReceiverUdpTunnelConnectionEgressAddress(this->tcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings().getEgressAddress());
    this->udpTunnelConnection.setReceiverUdpTunnelConnectionEgressPort(this->tcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings().getEgressPort());

    QObject::connect(&this->socket, &QTcpSocket::connected, this, &TCPTunnelClientConnection::handleConnected);
    QObject::connect(&this->socket, &QTcpSocket::disconnected, this, &TCPTunnelClientConnection::handleDisconnected);
    QObject::connect(&this->socket, &QTcpSocket::bytesWritten, this, &TCPTunnelClientConnection::handleBytesWritten);
    QObject::connect(&this->socket, &QTcpSocket::readyRead, this, &TCPTunnelClientConnection::handleBytesAvailable);
    QObject::connect(&this->connectionTimeoutTimer, &QTimer::timeout, this, &TCPTunnelClientConnection::handleConnectionTimeout);

    QObject::connect(this, &TCPTunnelClientConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    QObject::connect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPTunnelClientConnection::handleTcpTunnelPacket);
    QObject::connect(&this->connectionTimeoutTimer, &QTimer::timeout, this, &TCPTunnelClientConnection::handleConnectionTimeout);
}

TCPTunnelClientConnection::~TCPTunnelClientConnection()
{
    QObject::disconnect(&this->socket, &QTcpSocket::connected, this, &TCPTunnelClientConnection::handleConnected);
    QObject::disconnect(&this->socket, &QTcpSocket::disconnected, this, &TCPTunnelClientConnection::handleDisconnected);
    QObject::disconnect(&this->socket, &QTcpSocket::bytesWritten, this, &TCPTunnelClientConnection::handleBytesWritten);
    QObject::disconnect(&this->socket, &QTcpSocket::readyRead, this, &TCPTunnelClientConnection::handleBytesAvailable);
    QObject::disconnect(&this->connectionTimeoutTimer, &QTimer::timeout, this, &TCPTunnelClientConnection::handleConnectionTimeout);

    QObject::disconnect(this, &TCPTunnelClientConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    QObject::disconnect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPTunnelClientConnection::handleTcpTunnelPacket);
    QObject::disconnect(&this->connectionTimeoutTimer, &QTimer::timeout, this, &TCPTunnelClientConnection::handleConnectionTimeout);
}

const TCPTunnelConnectionSettings TCPTunnelClientConnection::getTcpTunnelConnectionSettings() const
{
    return this->tcpTunnelConnectionSettings;
}

QEventLoop& TCPTunnelClientConnection::getEventLoop()
{
    return this->eventLoop;
}

void TCPTunnelClientConnection::handleTcpTunnelPacket(const QByteArray data)
{
    const QMutexLocker lock(&this->lock);

    TCPTunnelPacket tcpTunnelPacket(data);

    const auto& tcpTunnelPacketHeader = tcpTunnelPacket.getHeader();
    const auto& tcpTunnelPacketType = tcpTunnelPacketHeader.getPacketType();
    const auto& host = tcpTunnelPacketHeader.getHost();
    const auto& port = tcpTunnelPacketHeader.getPort();

    switch(tcpTunnelPacketType)
    {
    case TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION:
        qDebug() << "TCPTunnelClientConnection: Received connect request";
        this->connect(host, port);
        break;
    case TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION:
        this->disconnect();
        break;
    case TCPTunnelPacketHeaderType::TCP_SEND_DATA:
        this->write(tcpTunnelPacket.getPayload());
        break;
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED:
        break;
    case TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA:
        break;
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED:
        break;
    case TCPTunnelPacketHeaderType::TCP_INVALID:
        break;
    default:
        break;
    }
}

// Private slots

void TCPTunnelClientConnection::handleConnected()
{
    const QMutexLocker lock(&this->lock);

    this->stopConnectionTimeoutTimer();
    const auto& connectedResponse = TCPTunnelPacket::createConnectedResponse(QByteArray::fromStdString(this->socket.peerAddress().toString().toStdString()), this->socket.peerPort());
    const auto& encodedConnectedResponse = connectedResponse.encode();
    emit this->writeTcpTunnelPacket(encodedConnectedResponse);
}

void TCPTunnelClientConnection::handleDisconnected()
{
    const QMutexLocker lock(&this->lock);

    this->stopConnectionTimeoutTimer();
    const auto& disconnectedResponse = TCPTunnelPacket::createDisconnectedResponse();
    emit this->writeTcpTunnelPacket(disconnectedResponse.encode());
}

void TCPTunnelClientConnection::handleBytesWritten()
{
    const QMutexLocker lock(&this->lock);

    const auto& bytesWrittenResponse = TCPTunnelPacket::createBytesWrittenResponse();
    emit this->writeTcpTunnelPacket(bytesWrittenResponse.encode());
}

void TCPTunnelClientConnection::handleBytesAvailable()
{
    const QMutexLocker lock(&this->lock);

    while(this->socket.bytesAvailable())
    {
        const auto& receivedData = this->socket.readAll();
        const auto& receivedDataResponse = TCPTunnelPacket::createReceivedDataResponse(receivedData);
        emit this->writeTcpTunnelPacket(receivedDataResponse.encode());
    }
}

void TCPTunnelClientConnection::handleConnectionTimeout()
{
    const QMutexLocker lock(&this->lock);

    const auto& connectionTimeoutResponse = TCPTunnelPacket::createConnectionTimeoutResponse();
    qDebug() << "TCPTunnelClientConnection: send timeout response:" << connectionTimeoutResponse.encode();
    emit this->writeTcpTunnelPacket(connectionTimeoutResponse.encode());
}

// Private

void TCPTunnelClientConnection::connect(const QByteArray& host, const int& port, const int& connectionTimeout)
{
    qDebug() << "TCPTunnelClientConnection: CONNECT TO" << host << port;
    this->socket.connectToHost(host, port);
    this->startConnectionTimeoutTimer(connectionTimeout);
}

void TCPTunnelClientConnection::disconnect()
{
    this->socket.close();
}

void TCPTunnelClientConnection::write(const QByteArray& data)
{
    bool dataSent = false;
    bool connected = false;
    do
    {
        connected = TCPTunnelClientConnection::socketConnected(this->socket);
        if(connected)
        {
            const auto& bytesWritten = this->socket.write(data);
            dataSent = (bytesWritten == data.size());
        }
    } while(!dataSent && connected);

    if(dataSent)
    {
        const auto& bytesWrittenResponse = TCPTunnelPacket::createBytesWrittenResponse();
        emit this->writeTcpTunnelPacket(bytesWrittenResponse.encode());
    }
}

const bool TCPTunnelClientConnection::socketConnected(const QTcpSocket& socket)
{
    return (socket.state() == QAbstractSocket::SocketState::ConnectedState);
}

void TCPTunnelClientConnection::startConnectionTimeoutTimer(const int& interval)
{
    this->connectionTimeoutTimer.setSingleShot(true);
    this->connectionTimeoutTimer.setInterval(interval);
    this->connectionTimeoutTimer.start();
}

void TCPTunnelClientConnection::stopConnectionTimeoutTimer()
{
    this->connectionTimeoutTimer.stop();
}