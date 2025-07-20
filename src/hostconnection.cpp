#include "hostconnection.h"
#include "tcptunnelpacket.h"
#include <QTimer>

HostConnection::HostConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->udpTunnelConnectionSettings = std::make_unique<UDPTunnelConnectionSettings>(udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.getEgressPort());
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(*this->udpTunnelConnectionSettings);

    QObject::connect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &HostConnection::handleUdpTunnelPacket);
}

HostConnection::~HostConnection()
{
    QObject::disconnect(this->udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &HostConnection::handleUdpTunnelPacket);
}

void HostConnection::connectToHost(const QString& host, const quint16& port)
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_OPEN_CONNECTION);
    header.setHost(QByteArray::fromStdString(host.toStdString()));
    header.setPort(port);
    const auto& packet = TCPTunnelPacket::encode(header, {});

    this->udpTunnelConnection->send(packet);
}

void HostConnection::send(const QByteArray& data)
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_SEND_DATA);
    const auto& packet = TCPTunnelPacket::encode(header, data);

    this->udpTunnelConnection->send(packet);
}

void HostConnection::disconnect()
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION);
    const auto& packet = TCPTunnelPacket::encode(header, {});

    this->udpTunnelConnection->send(packet);
}

void HostConnection::quit()
{
    TCPTunnelPacketHeader header;
    header.setPacketType(TCPTunnelPacketHeaderType::CLIENT_CLOSE_CONNECTION);
    const auto& packet = TCPTunnelPacket::encode(header, {});

    this->udpTunnelConnection->send(packet);

    QTimer::singleShot(0, this, [this](){
        emit this->clientHasQuit();
    });
}

// Private

void HostConnection::handleUdpTunnelPacket(const QByteArray& data)
{
    const auto& packet = TCPTunnelPacket::decode(data);
    const auto& header = packet.first;
    const auto& payload = packet.second;

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

void HostConnection::handleTcpConnect()
{
    QTimer::singleShot(0, this, [this](){
        emit clientIsConnected();
    });
}

void HostConnection::handleTcpData(const QByteArray& data)
{
    QTimer::singleShot(0, this, [this, data](){
        emit this->receivedData(data);
    });
}

void HostConnection::handleTcpDisconnect()
{
    QTimer::singleShot(0, this, [this](){
        emit this->clientWasDisconnected();
    });
}

void HostConnection::handleClientQuit()
{
    QTimer::singleShot(0, this, [this](){
        emit this->clientHasQuit();
    });
}
