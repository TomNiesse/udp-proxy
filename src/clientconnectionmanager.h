#ifndef CLIENTCONNECTIONMANAGER_H
#define CLIENTCONNECTIONMANAGER_H

#include "udptunnelconnectionsettings.h"
#include "udptunnelconnection.h"
#include <QObject>
#include <QHostAddress>
#include <QMutex>

class ClientConnectionManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ClientConnectionManager)
public:
    explicit ClientConnectionManager(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);
    ~ClientConnectionManager();

private:
    void handleReceivedData(const QByteArray& data);
    void clientConnectionThread(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings);

    std::unique_ptr<UDPTunnelConnection> udpTunnelConnection;
};

#endif
