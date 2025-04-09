#ifndef UDPTUNNELCONNECTION_H
#define UDPTUNNELCONNECTION_H

#include "udptunnelpacketsender.h"
#include "udptunnelpacketreceiver.h"
#include <vector>
#include <QByteArray>
#include <QHostAddress>
#include <QMutex>

typedef struct UDPTunnelConnectionSettings
{
    QHostAddress senderIngressAddress;
    int senderIngressPort;
    QHostAddress senderEgressAddress;
    int senderEgressPort;
    QHostAddress receiverIngressAddress;
    int receiverIngressPort;
    QHostAddress receiverEgressAddress;
    int receiverEgressPort;
} UDPTunnelConnectionSettings;

class UDPTunnelConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelConnection)
public:
    explicit UDPTunnelConnection(const UDPTunnelConnectionSettings& connectionSettings, const QString& name = "UDPTunnelConnection");
    void send(const QByteArray& payload);

    const std::unique_ptr<UDPTunnelPacketSender>& getSender();
    const std::unique_ptr<UDPTunnelPacketReceiver>& getReceiver();

signals:
    void bytesReceived(const QByteArray& payload);

private:
    QString name;
    std::unique_ptr<UDPTunnelPacketSender> sender;
    std::unique_ptr<UDPTunnelPacketReceiver> receiver;

    size_t packetId = 0;
    UDPTunnelConnectionSettings connectionSettings;
    QMutex lock;
};

#endif // UDPTUNNELCONNECTION_H
