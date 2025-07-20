#include "clientconnectionmanager.h"
#include "clientconnection.h"
#include "udptunnelconnection.h"
#include "tcptunnelpacket.h"
#include <thread>
#include <QDateTime>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>

ClientConnectionManager::ClientConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->udpTunnelConnection = std::make_unique<UDPTunnelConnection>(udpTunnelConnectionSettings);
    QObject::connect(udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ClientConnectionManager::handleReceivedData, Qt::DirectConnection);
}

ClientConnectionManager::~ClientConnectionManager()
{
    QObject::disconnect(udpTunnelConnection.get(), &UDPTunnelConnection::receivedData, this, &ClientConnectionManager::handleReceivedData);
}

// Private

void ClientConnectionManager::handleReceivedData(const QByteArray& data)
{
    const auto& udpTunnelConnectionSettings = UDPTunnelConnectionSettings(data);
    std::thread(clientConnectionThread, this, udpTunnelConnectionSettings).detach();

    TCPTunnelPacketHeader tcpTunnelPacketHeader;
    tcpTunnelPacketHeader.setPacketType(TCPTunnelPacketHeaderType::TCP_CONNECTION_OPENED);
    TCPTunnelPacket tcpTunnelPacket;
    const auto& tcpTunnelPacketBytes = tcpTunnelPacket.encode(tcpTunnelPacketHeader, {});

    this->udpTunnelConnection->send(tcpTunnelPacketBytes);
}

void ClientConnectionManager::clientConnectionThread(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    ClientConnection clientConnection(udpTunnelConnectionSettings);
    auto& eventLoop = clientConnection.getEventLoop();
    eventLoop.exec();
}
