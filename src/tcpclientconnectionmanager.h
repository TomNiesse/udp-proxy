#ifndef TCPCLIENTCONNECTIONMANAGER_H
#define TCPCLIENTCONNECTIONMANAGER_H

#include "udptunnelconnectionsettings.h"
#include "udptunnelconnection.h"
#include "tcptunnelconnectionsettings.h"
#include "tcptunnelclientconnection.h"
#include <QObject>
#include <QHostAddress>
#include <QMutex>

class TCPClientConnectionManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TCPClientConnectionManager)
public:
    explicit TCPClientConnectionManager(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings);
    ~TCPClientConnectionManager();

private:
    void handleReceivedData(const QByteArray& data);
    void clientConnectionThread(const TCPTunnelConnectionSettings& proxyTunnelConnectionSettings);

    UDPTunnelConnection udpTunnelConnection;
};

#endif // TCPCLIENTCONNECTIONMANAGER_H
