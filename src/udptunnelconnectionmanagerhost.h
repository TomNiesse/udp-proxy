#ifndef UDPTUNNELCONNECTIONMANAGERHOST_H
#define UDPTUNNELCONNECTIONMANAGERHOST_H

#include <QObject>

class UDPTunnelConnectionManagerHost : public QObject
{
    Q_OBJECT
public:
    explicit UDPTunnelConnectionManagerHost(QObject *parent = nullptr);

signals:
};

#endif // UDPTUNNELCONNECTIONMANAGERHOST_H
