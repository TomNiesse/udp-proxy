#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>

class TCPClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TCPClient)
public:
    explicit TCPClient();

signals:
    void connected(const size_t& connectionId);
    void disconnected(const size_t& connectionId);
    void bytesWritten(const size_t& connectionId);
    void bytesReceived(const size_t& connectionId, const QByteArray& packet);

public slots:
    void handleUdpTunnelPacket(const QByteArray& packet);

private:
    void connectToHost(const size_t& connectionId, const QString& address, const quint16& port);
    void disconnect(const size_t& connectionId);
    void write(const size_t& connectionId, const QByteArray& packet);

private:
    std::map<size_t, std::unique_ptr<QTcpSocket>> tcpConnections;
    QMutex lock;
};

#endif // TCPCLIENT_H
