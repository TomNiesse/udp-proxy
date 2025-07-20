#ifndef UDPTUNNELCONNECTIONSETTINGS_H
#define UDPTUNNELCONNECTIONSETTINGS_H

#include <QByteArray>

class UDPTunnelConnectionSettings
{
public:
    UDPTunnelConnectionSettings();
    UDPTunnelConnectionSettings(const QByteArray& ingressAddress, const uint16_t& ingressPort, const QByteArray& egressAddress, const uint16_t& egressPort);
    UDPTunnelConnectionSettings(const QByteArray& data);
    const QByteArray encode() const;

    const QByteArray getIngressAddress() const;
    void setIngressAddress(const QByteArray& ingressAddress);
    uint16_t getIngressPort() const;
    void setIngressPort(const uint16_t& ingressPort);
    const QByteArray getEgressAddress() const;
    void setEgressAddress(const QByteArray& egressAddress);
    uint16_t getEgressPort() const;
    void setEgressPort(const uint16_t& egressPort);

private:
    static const QByteArray encode(const QByteArray& ingressAddress, const uint16_t& igressPort, const QByteArray& egressAddress, const uint16_t& egressPort);
    static const std::tuple<QByteArray, uint16_t, QByteArray, uint16_t> decode(const QByteArray data);

    QByteArray ingressAddress = "0.0.0.0";
    uint16_t ingressPort = 0;
    QByteArray egressAddress = "127.0.0.1";
    uint16_t egressPort = 0;
};

#endif // UDPTUNNELCONNECTIONSETTINGS_H
