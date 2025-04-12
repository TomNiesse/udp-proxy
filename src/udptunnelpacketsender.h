#ifndef UDPTUNNELPACKETSENDER_H
#define UDPTUNNELPACKETSENDER_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QMutex>
#include <QEventLoop>

class UDPTunnelPacketSender : public QObject
{
    Q_OBJECT
public:
    explicit UDPTunnelPacketSender(const QHostAddress& listenAddress, const quint16& listenPort, const QHostAddress& egressAddress, const quint16& egressPort);
    void write(const QByteArray& packet);

signals:
    void receivedResponse();

private slots:
    void handleResponse();

private:
    QHostAddress listenAddress;
    quint16 listenPort = 0;
    QHostAddress egressAddress;
    quint16 egressPort = 0;
    std::unique_ptr<QUdpSocket> ingressSocket;
    std::unique_ptr<QUdpSocket> egressSocket;
    QMutex lock;
};

#endif // UDPTUNNELPACKETSENDER_H
