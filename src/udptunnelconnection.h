#ifndef UDPTUNNELCONNECTION_H
#define UDPTUNNELCONNECTION_H

#include "udptunnelpackettransceiver.h"
#include "udptunnelpacketreceiver.h"
#include "udptunnelconnectionsettings.h"
#include <vector>
#include <QByteArray>
#include <QMutex>

class UDPTunnelConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelConnection)
public:
    UDPTunnelConnection(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, const QString& name = "UDPTunnelConnection");
    ~UDPTunnelConnection();
    bool send(const QByteArray& payload);
    const UDPTunnelConnectionSettings getUdpTunnelConnectionSettings() const;

signals:
    void receivedData(const QByteArray& data);

private:
    QString name;
    std::unique_ptr<UDPTunnelConnectionSettings> udpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelPacketTransceiver> udpTunnelPacketTransceiver;
    size_t packetId = 0;
};

#endif // UDPTUNNELCONNECTION_H
