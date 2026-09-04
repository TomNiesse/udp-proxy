#ifndef TCPTUNNELCONNECTIONSETTINGS_H
#define TCPTUNNELCONNECTIONSETTINGS_H

#include <QObject>
#include <QByteArray>
#include "udptunnelconnectionsettings.h"

class TCPTunnelConnectionSettings
{
public:
    TCPTunnelConnectionSettings();
    void fromBytes(const QByteArray& data);

    const size_t getConnectionId() const;
    void setConnectionId(const size_t& connectionId);
    const UDPTunnelConnectionSettings getHostSenderUdpTunnelConnectionSettings() const;
    void setHostSenderUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& hostSenderUdpTunnelConnectionSettings);
    const UDPTunnelConnectionSettings getHostReceiverUdpTunnelConnectionSettings() const;
    void setHostReceiverUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& hostReceiverUdpTunnelConnectionSettings);
    const UDPTunnelConnectionSettings getClientSenderUdpTunnelConnectionSettings() const;
    void setClientSenderUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& clientSenderUdpTunnelConnectionSettings);
    const UDPTunnelConnectionSettings getClientReceiverUdpTunnelConnectionSettings() const;
    void setClientReceiverUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& clientReceiverUdpTunnelConnectionSettings);

    const QByteArray encode() const;

    const QString toString() const;

private:
    static const QByteArray encodeUInt8(const uint8_t& value);
    static const uint8_t decodeUInt8(const QByteArray& bytes);
    static const QByteArray encodeInt(const int& value);
    static const int decodeInt(const QByteArray& bytes);
    static const QByteArray encodeSize(const size_t& value);
    static const size_t decodeSize(const QByteArray& bytes);

    size_t connectionId;
    UDPTunnelConnectionSettings hostSenderUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings hostReceiverUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings clientSenderUdpTunnelConnectionSettings;
    UDPTunnelConnectionSettings clientReceiverUdpTunnelConnectionSettings;
};

#endif // TCPTUNNELCONNECTIONSETTINGS_H
