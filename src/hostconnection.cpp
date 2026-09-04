#include "hostconnection.h"
#include "tcptunnelpacket.h"
#include <QTimer>

// TODO: delete this and client connection: they are replaced by ProxyTunnelHost and ProxyTunnelClient

ProxyTunnelHostConnection::ProxyTunnelHostConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings)
    : senderUdpTunnelConnectionSettings(senderUdpTunnelConnectionSettings)
    , receiverUdpTunnelConnectionSettings(receiverUdpTunnelConnectionSettings)
{
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(this->senderUdpTunnelConnectionSettings, this->receiverUdpTunnelConnectionSettings);
    QObject::connect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ProxyTunnelHostConnection::handleUdpTunnelPacket);
}

ProxyTunnelHostConnection::~ProxyTunnelHostConnection()
{
    QObject::disconnect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ProxyTunnelHostConnection::handleUdpTunnelPacket);
}

void ProxyTunnelHostConnection::connectToHost(const QString& host, const quint16& port)
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION);
    header.setHost(QByteArray::fromStdString(host.toStdString()));
    header.setPort(port);
    // const auto& packet = TCPTunnelPacket::encode(/*header, {}*/);

    // this->udpTunnelConnection->sendData(packet);
}

void ProxyTunnelHostConnection::send(const QByteArray& data)
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_SEND_DATA);
    // const auto& packet = TCPTunnelPacket::encode(/*header, data*/);

    // this->udpTunnelConnection->sendData(packet);
}

void ProxyTunnelHostConnection::disconnect()
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION);
    // const auto& packet = TCPTunnelPacket::encode(/*header, {}*/);

    // this->udpTunnelConnection->sendData(packet);
}

void ProxyTunnelHostConnection::quit()
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::CLIENT_CLOSE_CONNECTION);
    // const auto& packet = TCPTunnelPacket::encode(/*header, {}*/);

    // this->udpTunnelConnection->sendData(packet);

    // QTimer::singleShot(0, this, [this](){
        emit this->clientHasQuit();
    // });
}

// Private

void ProxyTunnelHostConnection::handleUdpTunnelPacket(const QByteArray& data)
{
    const auto& packet = TCPTunnelPacket(data);
    const auto& header = packet.getHeader();
    const auto& payload = packet.getPayload();

    switch(header.getPacketType())
    {
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED:
        this->handleTcpConnect();
        break;
    case TCPTunnelPacketHeaderType::TCP_RECEIVED_DATA:
        this->handleTcpData(payload);
        break;
    case TCPTunnelPacketHeaderType::TCP_CONNECTION_CLOSED:
        this->handleTcpDisconnect();
        break;
    case TCPTunnelPacketHeaderType::CLIENT_CLOSED_CONNECTION:
        this->handleClientQuit();
        break;
    default:
        break;
    }
}

void ProxyTunnelHostConnection::handleTcpConnect()
{
    // QTimer::singleShot(0, this, [this](){
        emit clientIsConnected();
    // });
}

void ProxyTunnelHostConnection::handleTcpData(const QByteArray& data)
{
    // QTimer::singleShot(0, this, [this, data](){
        emit this->receivedData(data);
    // });
}

void ProxyTunnelHostConnection::handleTcpDisconnect()
{
    // QTimer::singleShot(0, this, [this](){
        emit this->clientWasDisconnected();
    // });
}

void ProxyTunnelHostConnection::handleClientQuit()
{
    // QTimer::singleShot(0, this, [this](){
        emit this->clientHasQuit();
    // });
}
