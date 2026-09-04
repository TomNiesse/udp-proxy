#include "proxytunnelhost.h"

ProxyTunnelHost::ProxyTunnelHost(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, QObject *parent)
    : QObject{parent}
    , udpTunnelConnection(UDPTunnelConnection(senderUdpTunnelConnectionSettings, receiverUdpTunnelConnectionSettings))
{
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    // QObject::connect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, &this->tcpTunnelConnection, &TCPTunnelHostConnection::handleTcpTunnelPacket);

    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::connected, this, &ProxyTunnelHost::connected);
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::disconnected, this, &ProxyTunnelHost::disconnected);
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::bytesWritten, this, &ProxyTunnelHost::bytesWritten);
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::receivedData, this, &ProxyTunnelHost::receivedData);
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::connectionTimeout, this, &ProxyTunnelHost::connectionTimeout);
}

ProxyTunnelHost::~ProxyTunnelHost()
{
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    // QObject::disconnect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, &this->tcpTunnelConnection, &TCPTunnelHostConnection::handleTcpTunnelPacket);

    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::disconnected, this, &ProxyTunnelHost::disconnected);
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::disconnected, this, &ProxyTunnelHost::disconnected);
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::bytesWritten, this, &ProxyTunnelHost::bytesWritten);
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::receivedData, this, &ProxyTunnelHost::receivedData);
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelHostConnection::connectionTimeout, this, &ProxyTunnelHost::connectionTimeout);
}

void ProxyTunnelHost::connect(const QByteArray& host, const int& port)
{
    // this->tcpTunnelConnection.connect(host, port);
}

void ProxyTunnelHost::disconnect()
{
    // this->tcpTunnelConnection.disconnect();
}

void ProxyTunnelHost::write(const QByteArray& data)
{
    // this->tcpTunnelConnection.write(data);
}