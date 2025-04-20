#include "proxyclient.h"
#include "tcptunnelpacket.h"
#include "udptunnelconnection.h"
#include "limits.h"
#include <thread>
#include <cstdio>
#include <ctime>
#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QTimer>

ProxyClient::ProxyClient(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(udpTunnelConnectionSettings);
    connect(this->udpTunnelConnection.get(), &UDPTunnelConnection::bytesReceived, this, [this](const QByteArray& payload){
        this->tcpClient.handleUdpTunnelPacket(payload);
    });
    connect(&this->tcpClient, &TCPClient::connected, this, [this](const size_t& connectionId){
        TCPTunnelPacketHeader header;
        header.setConnectionId(connectionId);
        header.setPacketType(TCPTunnelPacketHeaderType::TCP_RESPONSE_OPENED_CONNECTION);
        const auto& packet = TCPTunnelPacket::encode(header, {});
        this->udpTunnelConnection->send(packet);

    });
    connect(&this->tcpClient, &TCPClient::disconnected, this, [this](const size_t& connectionId){
        TCPTunnelPacketHeader header;
        header.setConnectionId(connectionId);
        header.setPacketType(TCPTunnelPacketHeaderType::TCP_RESPONSE_CLOSED_CONNECTION);
        const auto& packet = TCPTunnelPacket::encode(header, {});
        this->udpTunnelConnection->send(packet);
    });
    connect(&this->tcpClient, &TCPClient::bytesWritten, this, [this](const size_t& connectionId){
        TCPTunnelPacketHeader header;
        header.setConnectionId(connectionId);
        header.setPacketType(TCPTunnelPacketHeaderType::TCP_RESPONSE_SENT_DATA);
        const auto& packet = TCPTunnelPacket::encode(header, {});
        this->udpTunnelConnection->send(packet);
    });
    connect(&this->tcpClient, &TCPClient::bytesReceived, this, [this](const size_t& connectionId, const QByteArray& payload){
        TCPTunnelPacketHeader header;
        header.setConnectionId(connectionId);
        header.setPacketType(TCPTunnelPacketHeaderType::TCP_RESPONSE_RECEIVED_DATA);
        const auto& packet = TCPTunnelPacket::encode(header, payload);
        this->udpTunnelConnection->send(packet);
    });
}

ProxyClient::~ProxyClient()
{
    disconnect(this->udpTunnelConnection.get(), &UDPTunnelConnection::bytesReceived, this, nullptr);
    disconnect(&this->tcpClient, &TCPClient::connected, this, nullptr);
    disconnect(&this->tcpClient, &TCPClient::disconnected, this, nullptr);
    disconnect(&this->tcpClient, &TCPClient::bytesWritten, this, nullptr);
    disconnect(&this->tcpClient, &TCPClient::bytesReceived, this, nullptr);
}
