#ifndef PROXYTUNNELHOST_H
#define PROXYTUNNELHOST_H

#include <QObject>
#include "udptunnelconnection.h"
#include "tcptunnelhostconnection.h"

class ProxyTunnelHost : public QObject
{
    Q_OBJECT
public:
    explicit ProxyTunnelHost(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, QObject *parent = nullptr);
    ~ProxyTunnelHost();

    void connect(const QByteArray& host, const int& port);
    void disconnect();
    void write(const QByteArray& data);

signals:
    void connected();
    void disconnected();
    void bytesWritten();
    void receivedData(const QByteArray receivedData);
    void connectionTimeout();

private:
    UDPTunnelConnection udpTunnelConnection;
};

#endif // PROXYTUNNELHOST_H
