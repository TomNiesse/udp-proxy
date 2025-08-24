#include "proxyserver.h"
#include "proxyrequest.h"
#include "hostconnection.h"
#include <thread>
#include <QTcpSocket>
#include <QTimer>

ProxyServer::ProxyServer(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->hostConnectionManager = std::make_unique<HostConnectionManager>(udpTunnelConnectionSettings);
    this->portManager = std::make_unique<UDPTunnelConnectionPortManager>(UDPTunnelConnectionPortManager::generatePortList(60000, 65000));
}

void ProxyServer::incomingConnection(const qintptr socketDescriptor)
{
    QTimer::singleShot(0, this, [this, socketDescriptor](){
        const auto& hostManagerUdpTunnelConnectionSettings = this->hostConnectionManager->getUdpTunnelConnectionSettings();
        const auto& udpTunnelConnectionSettingsTemplate = UDPTunnelConnectionSettings(hostManagerUdpTunnelConnectionSettings.getIngressAddress(), 0, hostManagerUdpTunnelConnectionSettings.getEgressAddress(), 0);
        const auto& udpTunnelConnectionSettings = this->portManager->generateUDPTunnelConnectionSettings(udpTunnelConnectionSettingsTemplate);
        const auto& hostUdpTunnelConnectionSettings = udpTunnelConnectionSettings.first;
        const auto& clientUdpTunnelConnectionSettings = udpTunnelConnectionSettings.second;

        this->hostConnectionManager->openConnection(clientUdpTunnelConnectionSettings);
        std::thread(&ProxyServer::communicationThread, this, socketDescriptor, hostUdpTunnelConnectionSettings).detach();
    });
}

// Private

void ProxyServer::communicationThread(const qintptr socketDescriptor, const UDPTunnelConnectionSettings udpTunnelConnectionSettings)
{
    auto proxyConnectionState = ProxyConnectionState::WAITING;
    QEventLoop eventLoop;
    HostConnection hostConnection(udpTunnelConnectionSettings);

    QTcpSocket proxyServerSocket;
    proxyServerSocket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    proxyServerSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    proxyServerSocket.setSocketDescriptor(socketDescriptor);

    // Handle proxy client command or forward proxy client data
    QObject::connect(&proxyServerSocket, &QTcpSocket::readyRead, this, [this, &proxyServerSocket, &hostConnection, &proxyConnectionState, &eventLoop](){
        while(proxyServerSocket.bytesAvailable())
        {
            const auto& data = proxyServerSocket.readAll();
            if(proxyConnectionState == ProxyConnectionState::WAITING)
            {
                if(ProxyRequest::validate(data))
                {
                    const auto& hostAndPort = ProxyRequest::extractHostAndPort(data);
                    const auto& host = hostAndPort.first;
                    const auto& port = hostAndPort.second;
                        hostConnection.connectToHost(host, port);
                    proxyConnectionState = ProxyConnectionState::COMMUNICATION;
                }
                else
                {
                    eventLoop.quit();
                }
            }
            else if(proxyConnectionState == ProxyConnectionState::COMMUNICATION)
            {
                hostConnection.send(data);
            }
        }
    }, Qt::DirectConnection);

    // When the TCP connection to the requested host is established, let the proxy client know
    QObject::connect(&hostConnection, &HostConnection::clientIsConnected, this, [this, &proxyServerSocket](){
        const auto& connectionEstablished = QByteArray("HTTP/1.1 200 Connection Established\r\n\r\n");
        proxyServerSocket.write(connectionEstablished);
    }, Qt::DirectConnection);

    // Forward received TCP data to the proxy client
    QObject::connect(&hostConnection, &HostConnection::receivedData, this, [this, &proxyServerSocket](const QByteArray& data){
        proxyServerSocket.write(data);
    }, Qt::DirectConnection);

    // When the proxy client disconnects, this thread needs to stop
    QObject::connect(&hostConnection, &HostConnection::clientWasDisconnected, &eventLoop, &QEventLoop::quit, Qt::DirectConnection);
    QObject::connect(&proxyServerSocket, &QTcpSocket::disconnected, &eventLoop, &QEventLoop::quit, Qt::DirectConnection);

    // Run event loop
    eventLoop.exec();

    // Disconnect from the signals that are used during proxy communication
    QObject::disconnect(&proxyServerSocket, &QTcpSocket::readyRead, this, nullptr);
    QObject::disconnect(&hostConnection, &HostConnection::clientIsConnected, this, nullptr);
    QObject::disconnect(&hostConnection, &HostConnection::receivedData, this, nullptr);
    QObject::disconnect(&hostConnection, &HostConnection::clientWasDisconnected, &eventLoop, &QEventLoop::quit);
    QObject::disconnect(&proxyServerSocket, &QTcpSocket::disconnected, &eventLoop, &QEventLoop::quit);

    // Stop the thread that handles the TCP client traffic
    proxyConnectionState = ProxyConnectionState::STOP_CLIENT;
    hostConnection.quit();
    QObject::connect(&hostConnection, &HostConnection::clientHasQuit, &eventLoop, &QEventLoop::quit, Qt::DirectConnection);
    eventLoop.exec();
    QObject::disconnect(&hostConnection, &HostConnection::clientHasQuit, &eventLoop, &QEventLoop::quit);
}
