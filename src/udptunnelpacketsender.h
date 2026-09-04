#ifndef UDPTUNNELPACKETSENDER_H
#define UDPTUNNELPACKETSENDER_H

#include <QObject>
#include <QMutex>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include "iudptunnelpackethandler.h"
#include "udptunnelconnectionsettings.h"
#include "udptunnelpacketbuffer.h"

class UDPTunnelPacketSender : public IUDPTunnelPacketHandler
{
    Q_OBJECT
public:
    explicit UDPTunnelPacketSender(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, QObject *parent = nullptr);
    ~UDPTunnelPacketSender();

    void sendData(const QByteArray& data);

    const UDPTunnelConnectionSettings getUdpTunnelConnectionSettings() const;
    void setEgressAddress(const QByteArray& egressAddress);
    void setEgressPort(const int& egressPort);

signals:
    void dataSent();
    // The following signal is only used by unit tests
    void responseTimeoutOccurred();

private slots:
    void handleResponseTimeout();

private:
    const std::vector<UDPTunnelPacket> handleUdpTunnelPacket(const UDPTunnelPacket& udpTunnelPacket) override;

    void bufferData(const QByteArray& data);
    void startSendingData();
    void incrementPacketId();
    void sendUdpTunnelPackets();

    const bool responseReceived(const UDPTunnelPacket& udpTunnelPacket) const;
    void startResponseTimeoutTimer();
    void stopResponseTimeoutTimer();
    void restartResponseTimeoutTimer();

    void increaseResponseTimeout(const int& factor);
    void decreaseResponseTimeout(const int& amount);
    void setResponseTimeout(const int& responseTimeout);

    static const std::vector<UDPTunnelPacket> dataToUdpTunnelPackets(const QByteArray& data, const size_t& packetId);

    size_t packetId;
    QByteArrayList sendBuffer;
    QMutex lock;
    QMutex sendBufferLock;
    QMutex sendLock;
    bool sendingData = false;

    QMutex receiveBufferLock;
    std::vector<QNetworkDatagram> receiveBuffer;
    bool handlingReceivedData = false;

    QUdpSocket ingressSocket;
    QUdpSocket egressSocket;

    UDPTunnelConnectionSettings udpTunnelConnectionSettings;

    UDPTunnelPacket lastSentUdpTunnelPacket;
    QTimer responseTimeoutTimer;
    int responseTimeout = 1;

    UDPTunnelPacketBuffer udpTunnelPacketBuffer;

    friend class test_udptunnelconnection;
};

#endif // UDPTUNNELPACKETSENDER_H
