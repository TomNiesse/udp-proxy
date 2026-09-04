#ifndef UDPTUNNELCONNECTION_H
#define UDPTUNNELCONNECTION_H

#include "udptunnelconnectionsettings.h"
#include "udptunnelpacketsender.h"
#include "udptunnelpacketreceiver.h"
#include "udptunnelconnectionsettings.h"
#include <QByteArray>
#include <QMutex>

class UDPTunnelConnection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(UDPTunnelConnection)
public:
    UDPTunnelConnection(const UDPTunnelConnectionSettings& senderUdpTunnelConnectionSettings, const UDPTunnelConnectionSettings& receiverUdpTunnelConnectionSettings, const QString& name = "UDPTunnelConnection");
    ~UDPTunnelConnection();

    const UDPTunnelConnectionSettings getSenderUdpTunnelConnectionSettings() const;
    const UDPTunnelConnectionSettings getReceiverUdpTunnelConnectionSettings() const;

    void setSenderUdpTunnelConnectionEgressAddress(const QByteArray& senderUdpTunnelConnectionEgressAddress);
    void setSenderUdpTunnelConnectionEgressPort(const int& senderUdpTunnelConnectionEgressPort);
    void setReceiverUdpTunnelConnectionEgressAddress(const QByteArray& ReceiverUdpTunnelConnectionEgressAddress);
    void setReceiverUdpTunnelConnectionEgressPort(const int& ReceiverUdpTunnelConnectionEgressPort);

public slots:
    void sendData(const QByteArray payload);

signals:
    void dataSent();
    void receivedData(const QByteArray data);
    // The following signal is only used by unit tests
    void responseTimeoutOccurred();

private:
    QString name;
    UDPTunnelConnectionSettings senderUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings receiverUdpTunnelConnectionSettings;
    std::unique_ptr<UDPTunnelPacketSender> udpTunnelPacketSender;
    std::unique_ptr<UDPTunnelPacketReceiver> udpTunnelPacketReceiver;
    size_t packetId = 0;
    uint16_t maxChunks = 0;

    friend class test_udptunnelconnection;
};

#endif // UDPTUNNELCONNECTION_H
