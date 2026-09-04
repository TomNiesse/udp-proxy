#ifndef TCPHOSTCONNECTIONMANAGER_H
#define TCPHOSTCONNECTIONMANAGER_H

#include "udptunnelconnection.h"
#include "udptunnelconnectionsettings.h"
#include "proxytunnelhost.h"
#include "tcptunnelconnectionsettings.h"
#include <QObject>
#include <QMutex>


// TODO: maak de negotiator die de verbindingen tussen host en client proxy manager regelt!


class TCPHostConnectionManager : public QObject
{
    Q_OBJECT
public:
    TCPHostConnectionManager(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings, QObject* parent = nullptr);
    ~TCPHostConnectionManager();

    const size_t createTcpTunnelConnection();
    void removeTcpTunnelConnection(const size_t& connectionId);
    std::shared_ptr<TCPTunnelHostConnection> getTcpTunnelConnection(const size_t& connectionId) const;
    void initializeTcpTunnelConnection(const size_t& connectionId);

signals:
    void tcpTunnelConnectionInitialized(const size_t connectionId);

private:
    inline void handleReceivedTCPTunnelConnectionSettings(const TCPTunnelConnectionSettings& proxyTunnelConnectionSettings);
    inline void setTcpTunnelConnectionSettings(const TCPTunnelConnectionSettings& tcpTunnelConnectionSettings);

    UDPTunnelConnection udpTunnelConnection;
    // TODO: replace this map with an std vector and get the connection id from the tcp tunnel settings!!!!!
    std::map<size_t, std::shared_ptr<TCPTunnelHostConnection>> tcpTunnelConnections;
    mutable QMutex tcpTunnelConnectionsLock;

    size_t connectionId;
};

#endif // TCPHOSTCONNECTIONMANAGER_H
