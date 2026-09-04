#include "tcpclientconnectionmanager.h"
#include "proxytunnelclient.h"
#include "udptunnelconnection.h"
#include "tcptunnelpacket.h"
#include <thread>
#include <QDateTime>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>

TCPClientConnectionManager::TCPClientConnectionManager(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings)
    : udpTunnelConnection(UDPTunnelConnection(senderUdpTunnelConnectionSettings, receiverUdpTunnelConnectionSettings))
{
    QObject::connect(&udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPClientConnectionManager::handleReceivedData);
}

TCPClientConnectionManager::~TCPClientConnectionManager()
{
    QObject::disconnect(&udpTunnelConnection, &UDPTunnelConnection::receivedData, this, &TCPClientConnectionManager::handleReceivedData);
}

// Private

void TCPClientConnectionManager::handleReceivedData(const QByteArray& data)
{
    qDebug() << "RECEIVED DATA!";
    TCPTunnelConnectionSettings t;
    t.fromBytes(data);
    qDebug() << t.toString();

    QTimer::singleShot(0, this, [this, data](){
        TCPTunnelConnectionSettings proxyTunnelConnectionSettings;
        proxyTunnelConnectionSettings.fromBytes(data);
        clientConnectionThread(proxyTunnelConnectionSettings);
    });
}

void TCPClientConnectionManager::clientConnectionThread(const TCPTunnelConnectionSettings& initialTcpTunnelConnectionSettings)
{
    TCPTunnelConnectionSettings tcpTunnelConnectionSettings = initialTcpTunnelConnectionSettings;

    // Set the ingress addressing to the settings from the TCPClientConnectionManager class
    UDPTunnelConnectionSettings clientSenderUdpTunnelConnectionSettings = this->udpTunnelConnection.getSenderUdpTunnelConnectionSettings();
    clientSenderUdpTunnelConnectionSettings.setIngressPort(0);
    tcpTunnelConnectionSettings.setClientSenderUdpTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettings);
    UDPTunnelConnectionSettings clientReceiverUdpTunnelConnectionSettings = this->udpTunnelConnection.getReceiverUdpTunnelConnectionSettings();
    clientReceiverUdpTunnelConnectionSettings.setIngressPort(0);
    tcpTunnelConnectionSettings.setClientReceiverUdpTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettings);

    // Create a client connection using the initial tcp tunnel connection settings

    TCPTunnelClientConnection tcpTunnelClientConnection(tcpTunnelConnectionSettings);
    tcpTunnelConnectionSettings = tcpTunnelClientConnection.getTcpTunnelConnectionSettings();

    // Send the TCP connection settings to the TCP host connection manager, so the TCP connection can be established
    const auto& encodedHostProxyTunnelConnectionSettings = tcpTunnelConnectionSettings.encode();
    this->udpTunnelConnection.sendData(encodedHostProxyTunnelConnectionSettings);

    const auto& connectionId = tcpTunnelConnectionSettings.getConnectionId();

    qDebug() << "Thread for connection" << connectionId << "was started!";

    QEventLoop& eventLoop = tcpTunnelClientConnection.getEventLoop();
    eventLoop.exec();

    qDebug() << "Thread for connection" << connectionId << "has stopped!";
}