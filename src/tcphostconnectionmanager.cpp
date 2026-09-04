#include "tcphostconnectionmanager.h"
#include <QTimer>

#include <QDebug>

TCPHostConnectionManager::TCPHostConnectionManager(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject* parent)
    : QObject{parent}
    , udpTunnelConnection(UDPTunnelConnection(tcpTunnelConnectionSettings.getHostSenderUdpTunnelConnectionSettings(), tcpTunnelConnectionSettings.getHostReceiverUdpTunnelConnectionSettings()))
    , connectionId(0)
{
    connect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, [this](const QByteArray data){
        TCPTunnelConnectionSettings tcpTunnelConnectionSettings;
        tcpTunnelConnectionSettings.fromBytes(data);
        this->handleReceivedTCPTunnelConnectionSettings(tcpTunnelConnectionSettings);
    });
}

TCPHostConnectionManager::~TCPHostConnectionManager()
{
    disconnect(&this->udpTunnelConnection, &UDPTunnelConnection::receivedData, this, nullptr);
}

const size_t TCPHostConnectionManager::createTcpTunnelConnection()
{
    const QMutexLocker lock(&this->tcpTunnelConnectionsLock);

    // Create a TCP tunnel host connection
    TCPTunnelConnectionSettings initialTcpTunnelConnectionSettings;

    // Use the UDP tunnel connection address settings as setting for the other TCP connections
    UDPTunnelConnectionSettings hostSenderUdpTunnelConnectionSettings = this->udpTunnelConnection.getSenderUdpTunnelConnectionSettings();
    hostSenderUdpTunnelConnectionSettings.setIngressPort(0);
    hostSenderUdpTunnelConnectionSettings.setEgressPort(0);
    initialTcpTunnelConnectionSettings.setHostSenderUdpTunnelConnectionSettings(hostSenderUdpTunnelConnectionSettings);
    UDPTunnelConnectionSettings hostReceiverUdpTunnelConnectionSettings = this->udpTunnelConnection.getReceiverUdpTunnelConnectionSettings();
    hostReceiverUdpTunnelConnectionSettings.setIngressPort(0);
    hostReceiverUdpTunnelConnectionSettings.setEgressPort(0);
    initialTcpTunnelConnectionSettings.setHostReceiverUdpTunnelConnectionSettings(hostReceiverUdpTunnelConnectionSettings);

    const auto connectionId = this->connectionId++;
    qDebug() << "Connection ID is now:" << connectionId;
    this->tcpTunnelConnections.emplace(connectionId, std::make_shared<TCPTunnelHostConnection>(initialTcpTunnelConnectionSettings));

    return connectionId;
}

std::shared_ptr<TCPTunnelHostConnection> TCPHostConnectionManager::getTcpTunnelConnection(const size_t& connectionId) const
{
    const QMutexLocker lock(&this->tcpTunnelConnectionsLock);

    if(this->tcpTunnelConnections.contains(connectionId))
    {
        return this->tcpTunnelConnections.at(connectionId);
    }

    return nullptr;
}

void TCPHostConnectionManager::initializeTcpTunnelConnection(const size_t& connectionId)
{
    // Extract it's settings and update the client settings
    this->tcpTunnelConnectionsLock.lock();
    auto tcpHostConnection = this->tcpTunnelConnections.at(connectionId);
    this->tcpTunnelConnectionsLock.unlock();

    // The client sender settings should match the host receiver settings
    auto initialTcpTunnelConnectionSettings = tcpHostConnection->getTcpTunnelConnectionSettings();
    initialTcpTunnelConnectionSettings.setConnectionId(connectionId);

    auto clientSenderUdpTunnelConnectionSettings = initialTcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings();
    clientSenderUdpTunnelConnectionSettings.setEgressPort(tcpHostConnection->getTcpTunnelConnectionSettings().getHostReceiverUdpTunnelConnectionSettings().getIngressPort());
    initialTcpTunnelConnectionSettings.setClientSenderUdpTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettings);

    auto clientReceiverUdpTunnelConnectionSettings = initialTcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings();
    clientReceiverUdpTunnelConnectionSettings.setEgressPort(tcpHostConnection->getTcpTunnelConnectionSettings().getHostSenderUdpTunnelConnectionSettings().getIngressPort());
    initialTcpTunnelConnectionSettings.setClientReceiverUdpTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettings);

    qDebug() << initialTcpTunnelConnectionSettings.toString();

    // Send the listener settings over to the client
    this->udpTunnelConnection.sendData(initialTcpTunnelConnectionSettings.encode());
}

// TODO: add this to the auto clean functions? or have proxy host manager handle the delete of this thing?
void TCPHostConnectionManager::removeTcpTunnelConnection(const size_t& connectionId)
{
    const QMutexLocker lock(&this->tcpTunnelConnectionsLock);

    if(this->tcpTunnelConnections.contains(connectionId))
    {
        this->tcpTunnelConnections.erase(connectionId);
    }
}

// Private

void TCPHostConnectionManager::handleReceivedTCPTunnelConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings)
{
    qDebug() << "Received tcp tunnel connection settings:" << tcpTunnelConnectionSettings.toString();

    QTimer::singleShot(0, this, [this, tcpTunnelConnectionSettings](){
        setTcpTunnelConnectionSettings(tcpTunnelConnectionSettings);
    });
}

void TCPHostConnectionManager::setTcpTunnelConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings)
{
    const auto& connectionId = tcpTunnelConnectionSettings.getConnectionId();

    // TODO (!): Set the correct UDP send settings to the already created(!) host connection
    // Get host udp tunnel settings and place settings inside ProxyTunnelSettings
    this->tcpTunnelConnectionsLock.lock();
    auto hostConnection = this->tcpTunnelConnections.at(connectionId);
    this->tcpTunnelConnectionsLock.unlock();

    hostConnection->setTcpTunnelConnectionSettings(tcpTunnelConnectionSettings);

    emit this->tcpTunnelConnectionInitialized(connectionId);
}