#include "proxytunnelclient.h"

ProxyTunnelClient::ProxyTunnelClient(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, QObject *parent)
    : QObject{parent}
    , udpTunnelConnection(UDPTunnelConnection(senderUdpTunnelConnectionSettings, receiverUdpTunnelConnectionSettings))
    // , tcpTunnelConnection(TCPTunnelClientConnection())
{
    // QObject::connect(&this->tcpTunnelConnection, &TCPTunnelClientConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    // QObject::connect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, &this->tcpTunnelConnection, &TCPTunnelClientConnection::handleTcpTunnelPacket);
}

ProxyTunnelClient::~ProxyTunnelClient()
{
    // QObject::disconnect(&this->tcpTunnelConnection, &TCPTunnelClientConnection::writeTcpTunnelPacket, &this->udpTunnelConnection, &UDPTunnelConnection::sendData);
    // QObject::disconnect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, &this->tcpTunnelConnection, &TCPTunnelClientConnection::handleTcpTunnelPacket);
}

const UDPTunnelConnectionSettings ProxyTunnelClient::getReceiverUdpTunnelConnectionSettings() const
{
    return this->udpTunnelConnection.getReceiverUdpTunnelConnectionSettings();
}

QEventLoop& ProxyTunnelClient::getEventLoop()
{
    return this->eventLoop;
}