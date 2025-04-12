#include "proxyserver.h"
#include "proxyrequest.h"
#include "limits.h"
#include <thread>
#include <QTcpSocket>
#include <QDateTime>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>
#include <QTimer>
#include <QEventLoop>

ProxyServer::ProxyServer(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings)
{
    this->tcpConnectionManager = std::make_unique<TCPProxyConnectionManager>(udpTunnelConnectionSettings);
}

void ProxyServer::incomingConnection(const qintptr socketDescriptor)
{
    QTimer::singleShot(0, this, [this, socketDescriptor](){
        this->lock.lock();
        const auto connectionId = this->connectionId++;
        bool connectionActive = false;

        QTcpSocket proxyServerSocket;
        proxyServerSocket.setSocketDescriptor(socketDescriptor);

        // Connect to the signals that TCPProxyConnectionManager provides
        QObject::connect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::connected, this, [this, connectionId, &connectionActive, &proxyServerSocket](const size_t& _connectionId){
            if(_connectionId == connectionId)
            {
                connectionActive = true;

                QTimer::singleShot(0, this, [this, &proxyServerSocket](){
                    if(proxyServerSocket.state() == QAbstractSocket::ConnectedState)
                    {
                        proxyServerSocket.write(QByteArray("HTTP/1.1 200 Connection Established\r\n\r\n"));
                        proxyServerSocket.waitForBytesWritten();
                    }
                });
            }
        });
        QObject::connect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::disconnected, this, [this, connectionId, &connectionActive](const size_t& _connectionId){
            if(_connectionId == connectionId)
            {
                connectionActive = false;
            }
        });
        QObject::connect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::bytesReceived, this, [this, connectionId, &proxyServerSocket](const size_t& _connectionId, const QByteArray& payload){
            if(_connectionId == connectionId)
            {
                if(proxyServerSocket.state() == QAbstractSocket::ConnectedState)
                {
                    proxyServerSocket.write(payload, payload.size());
                    proxyServerSocket.waitForBytesWritten();
                }
            }
        });

        // Connect to the signals that the QTcpSocket proxy client socket provides
        QObject::connect(&proxyServerSocket, &QTcpSocket::disconnected, this, [this, connectionId, &connectionActive](){
            connectionActive = false;
        });
        QObject::connect(&proxyServerSocket, &QTcpSocket::readyRead, this, [this, connectionId, &proxyServerSocket, &connectionActive](){
            if(connectionActive)
            {
                while(proxyServerSocket.bytesAvailable())
                {
                    if(proxyServerSocket.state() == QAbstractSocket::ConnectedState)
                    {
                        const auto& payload = proxyServerSocket.readAll();
                        QTimer::singleShot(0, this, [this, connectionId, payload](){
                            this->tcpConnectionManager->write(connectionId, payload);
                        });
                    }
                }
            }
            else
            {
                const auto& proxyRequest = proxyServerSocket.readAll();
                const auto& proxyRequestIsValid = ProxyRequest::validate(proxyRequest);
                if(proxyRequestIsValid)
                {
                    const auto& hostAndPort = ProxyRequest::extractHostAndPort(proxyRequest);
                    const auto& host = hostAndPort.first;
                    const auto& port = hostAndPort.second;
                    QTimer::singleShot(0, this, [this, connectionId, host, port](){
                        this->tcpConnectionManager->connectToHost(connectionId, QByteArray(host.toStdString().c_str(), host.size()), port);
                    });
                }
                else
                {
                    const auto& badGateway = QByteArray("HTTP/1.1 502 Bad Gateway\r\n"
                                                       "Content-Type: text/html\r\n"
                                                       "Content-Length: 0\r\n"
                                                       "Connection: close\r\n\r\n");
                    proxyServerSocket.write(badGateway);
                    proxyServerSocket.waitForBytesWritten();
                    proxyServerSocket.close();
                }
            }
        });

        // Run in the background, until a connection is broken
        QEventLoop eventLoop;
        QObject::connect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::disconnected, &eventLoop, &QEventLoop::quit);
        QObject::connect(&proxyServerSocket, &QTcpSocket::disconnected, &eventLoop, &QEventLoop::quit);
        proxyServerSocket.waitForReadyRead(0);
        this->lock.unlock();
        eventLoop.exec();
        this->lock.lock();

        // Disconnect from the signals that TCPProxyConnectionManager provides
        QObject::disconnect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::connected, this, nullptr);
        QObject::disconnect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::disconnected, this, nullptr);
        QObject::disconnect(this->tcpConnectionManager.get(), &TCPProxyConnectionManager::bytesReceived, this, nullptr);

        // Disconnect from the signals that the QTcpSocket proxy client socket provides
        QObject::disconnect(&proxyServerSocket, &QTcpSocket::disconnected, this, nullptr);
        QObject::disconnect(&proxyServerSocket, &QTcpSocket::readyRead, this, nullptr);

        QMetaObject::invokeMethod(&proxyServerSocket, [this, &proxyServerSocket, &connectionId](){
            proxyServerSocket.disconnect();
            this->tcpConnectionManager->disconnect(connectionId);
        }, Qt::AutoConnection);
        this->lock.unlock();
    });
}
