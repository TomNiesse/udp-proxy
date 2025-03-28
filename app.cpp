#include "app.h"
#include "limits.h"
#include "tcptunnelpacketheader.h"
#include "tcptunnelpacket.h"
#include <QTimer>
#include <thread>
#include <QFile>

App::App(int& argc, char** argv) : QCoreApplication(argc, argv)
{
    // Create a UDP tunnel connection for the proxy server
    UDPTunnelConnectionSettings proxyServerUdpTunnelConnectionSettings;
    proxyServerUdpTunnelConnectionSettings.senderIngressAddress = QHostAddress::Any;
    proxyServerUdpTunnelConnectionSettings.senderIngressPort = 5000;
    proxyServerUdpTunnelConnectionSettings.senderEgressAddress = QHostAddress("127.0.0.1");
    proxyServerUdpTunnelConnectionSettings.senderEgressPort = 5001;
    proxyServerUdpTunnelConnectionSettings.receiverIngressAddress = QHostAddress::Any;
    proxyServerUdpTunnelConnectionSettings.receiverIngressPort = 6001;
    proxyServerUdpTunnelConnectionSettings.receiverEgressAddress = QHostAddress("127.0.0.1");
    proxyServerUdpTunnelConnectionSettings.receiverEgressPort = 6000;
    this->proxyServer = std::make_unique<ProxyServer>(proxyServerUdpTunnelConnectionSettings);

    // When a client connects, keep listening for new connections
    QObject::connect(this->proxyServer.get(), &ProxyServer::newConnection, this, [this](){
        this->proxyServer->waitForNewConnection();
    });

    UDPTunnelConnectionSettings proxyClientUdpTunnelConnectionSettings;
    proxyClientUdpTunnelConnectionSettings.senderIngressAddress = QHostAddress::Any;
    proxyClientUdpTunnelConnectionSettings.senderIngressPort = 6000;
    proxyClientUdpTunnelConnectionSettings.senderEgressAddress = QHostAddress("127.0.0.1");
    proxyClientUdpTunnelConnectionSettings.senderEgressPort = 6001;
    proxyClientUdpTunnelConnectionSettings.receiverIngressAddress = QHostAddress::Any;
    proxyClientUdpTunnelConnectionSettings.receiverIngressPort = 5001;
    proxyClientUdpTunnelConnectionSettings.receiverEgressAddress = QHostAddress("127.0.0.1");
    proxyClientUdpTunnelConnectionSettings.receiverEgressPort = 5000;
    this->proxyClient = std::make_unique<ProxyClient>(proxyClientUdpTunnelConnectionSettings);

    const bool result = proxyServer->listen(QHostAddress::Any, 8080);
    if(result)
    {
        qDebug() << "Proxy server is listening on port" << 8080;
    }
}
