#ifndef IUDPTUNNELPACKETHANDLER_H
#define IUDPTUNNELPACKETHANDLER_H

#include <QObject>
#include "udptunnelpacket.h"

class IUDPTunnelPacketHandler : public QObject
{
    Q_OBJECT
public:
    explicit IUDPTunnelPacketHandler(QObject *parent = nullptr);

private:
    virtual const std::vector<UDPTunnelPacket> handleUdpTunnelPacket(const UDPTunnelPacket& udpTunnelPacket) = 0;
};

#endif // IUDPTUNNELPACKETHANDLER_H
