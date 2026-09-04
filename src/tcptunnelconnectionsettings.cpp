#include "tcptunnelconnectionsettings.h"




#include <QDebug>

TCPTunnelConnectionSettings::TCPTunnelConnectionSettings()
{

}

void TCPTunnelConnectionSettings::fromBytes(const QByteArray& data)
{
    size_t offset = 0;

    size_t connectionId = 0;
    const auto& connectionIdBytes = data.mid(offset, sizeof(connectionId));
    memcpy(&connectionId, connectionIdBytes.constData(), sizeof(connectionId));
    offset += sizeof(connectionId);
    this->connectionId = connectionId;

    // qDebug() << "connection ID:" << this->connectionId;
    // qDebug() << "offset is now:" << offset;

    size_t hostSenderUdpTunnelConnectionSettingsBytesSize = 0;
    const auto& hostSenderUdpTunnelConnectionSettingsBytesSizeBytes = data.mid(offset, sizeof(hostSenderUdpTunnelConnectionSettingsBytesSize));
    memcpy(&hostSenderUdpTunnelConnectionSettingsBytesSize, hostSenderUdpTunnelConnectionSettingsBytesSizeBytes.constData(), sizeof(hostSenderUdpTunnelConnectionSettingsBytesSize));
    offset += sizeof(hostSenderUdpTunnelConnectionSettingsBytesSize);

    // qDebug() << "host sender settings size:" << hostSenderUdpTunnelConnectionSettingsBytesSize;
    // qDebug() << "offset is now:" << offset;

    const QByteArray& hostSenderUdpTunnelConnectionSettingsBytes = data.mid(offset, hostSenderUdpTunnelConnectionSettingsBytesSize);
    this->hostSenderUdpTunnelConnectionSettings = UDPTunnelConnectionSettings(hostSenderUdpTunnelConnectionSettingsBytes);
    offset += hostSenderUdpTunnelConnectionSettingsBytesSize;

    // qDebug() << "host sender settings:" << this->hostSenderUdpTunnelConnectionSettings.toString();
    // qDebug() << "offset is now:" << offset;

    size_t hostReceiverUdpTunnelConnectionSettingsBytesSize = 0;
    const auto& hostReceiverUdpTunnelConnectionSettingsBytesSizeBytes = data.mid(offset, sizeof(hostReceiverUdpTunnelConnectionSettingsBytesSize));
    memcpy(&hostReceiverUdpTunnelConnectionSettingsBytesSize, hostReceiverUdpTunnelConnectionSettingsBytesSizeBytes.constData(), sizeof(hostReceiverUdpTunnelConnectionSettingsBytesSize));
    offset += sizeof(hostReceiverUdpTunnelConnectionSettingsBytesSize);

    // qDebug() << "host receiver settings size:" << hostReceiverUdpTunnelConnectionSettingsBytesSize;
    // qDebug() << "offset is now:" << offset;

    const QByteArray& hostReceiverUdpTunnelConnectionSettingsBytes = data.mid(offset, hostReceiverUdpTunnelConnectionSettingsBytesSize);
    this->hostReceiverUdpTunnelConnectionSettings = UDPTunnelConnectionSettings(hostReceiverUdpTunnelConnectionSettingsBytes);
    offset += hostReceiverUdpTunnelConnectionSettingsBytesSize;

    // qDebug() << "host receiver settings:" << this->hostReceiverUdpTunnelConnectionSettings.toString();
    // qDebug() << "offset is now:" << offset;

    size_t clientSenderUdpTunnelConnectionSettingsBytesSize = 0;
    const auto& clientSenderUdpTunnelConnectionSettingsBytesSizeBytes = data.mid(offset, sizeof(clientSenderUdpTunnelConnectionSettingsBytesSize));
    memcpy(&clientSenderUdpTunnelConnectionSettingsBytesSize, clientSenderUdpTunnelConnectionSettingsBytesSizeBytes.constData(), sizeof(clientSenderUdpTunnelConnectionSettingsBytesSize));
    offset += sizeof(clientSenderUdpTunnelConnectionSettingsBytesSize);

    // qDebug() << "client sender settings size:" << clientSenderUdpTunnelConnectionSettingsBytesSize;
    // qDebug() << "offset is now:" << offset;

    const QByteArray& clientSenderUdpTunnelConnectionSettingsBytes = data.mid(offset, clientSenderUdpTunnelConnectionSettingsBytesSize);
    this->clientSenderUdpTunnelConnectionSettings = UDPTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettingsBytes);
    offset += clientSenderUdpTunnelConnectionSettingsBytesSize;

    // qDebug() << "client sender settings:" << this->clientSenderUdpTunnelConnectionSettings.toString();
    // qDebug() << "offset is now:" << offset;

    size_t clientReceiverUdpTunnelConnectionSettingsBytesSize = 0;
    const auto& clientReceiverUdpTunnelConnectionSettingsBytesSizeBytes = data.mid(offset, sizeof(clientReceiverUdpTunnelConnectionSettingsBytesSize));
    memcpy(&clientReceiverUdpTunnelConnectionSettingsBytesSize, clientReceiverUdpTunnelConnectionSettingsBytesSizeBytes.constData(), sizeof(clientReceiverUdpTunnelConnectionSettingsBytesSize));
    offset += sizeof(clientReceiverUdpTunnelConnectionSettingsBytesSize);

    // qDebug() << "client receiver settings size:" << clientReceiverUdpTunnelConnectionSettingsBytesSize;
    // qDebug() << "offset is now:" << offset;

    const QByteArray& clientReceiverUdpTunnelConnectionSettingsBytes = data.mid(offset, clientReceiverUdpTunnelConnectionSettingsBytesSize);
    this->clientReceiverUdpTunnelConnectionSettings = UDPTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettingsBytes);
    offset += clientReceiverUdpTunnelConnectionSettingsBytesSize;
}

const size_t TCPTunnelConnectionSettings::getConnectionId() const
{
    return this->connectionId;
}

void TCPTunnelConnectionSettings::setConnectionId(const size_t& connectionId)
{
    this->connectionId = connectionId;
}

const UDPTunnelConnectionSettings TCPTunnelConnectionSettings::getHostSenderUdpTunnelConnectionSettings() const
{
    return this->hostSenderUdpTunnelConnectionSettings;
}

void TCPTunnelConnectionSettings::setHostSenderUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& hostSenderUdpTunnelConnectionSettings)
{
    this->hostSenderUdpTunnelConnectionSettings = hostSenderUdpTunnelConnectionSettings;
}

const UDPTunnelConnectionSettings TCPTunnelConnectionSettings::getHostReceiverUdpTunnelConnectionSettings() const
{
    return this->hostReceiverUdpTunnelConnectionSettings;
}

void TCPTunnelConnectionSettings::setHostReceiverUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& hostReceiverUdpTunnelConnectionSettings)
{
    this->hostReceiverUdpTunnelConnectionSettings = hostReceiverUdpTunnelConnectionSettings;
}

const UDPTunnelConnectionSettings TCPTunnelConnectionSettings::getClientSenderUdpTunnelConnectionSettings() const
{
    return this->clientSenderUdpTunnelConnectionSettings;
}

void TCPTunnelConnectionSettings::setClientSenderUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& clientSenderUdpTunnelConnectionSettings)
{
    this->clientSenderUdpTunnelConnectionSettings = clientSenderUdpTunnelConnectionSettings;
}

const UDPTunnelConnectionSettings TCPTunnelConnectionSettings::getClientReceiverUdpTunnelConnectionSettings() const
{
    return this->clientReceiverUdpTunnelConnectionSettings;
}

void TCPTunnelConnectionSettings::setClientReceiverUdpTunnelConnectionSettings(const UDPTunnelConnectionSettings& clientReceiverUdpTunnelConnectionSettings)
{
    this->clientReceiverUdpTunnelConnectionSettings = clientReceiverUdpTunnelConnectionSettings;
}

const QByteArray TCPTunnelConnectionSettings::encode() const
{
    QByteArray out;

    const auto& connectionIdBytes = encodeSize(this->connectionId);
    out.append(connectionIdBytes);

    const auto& hostSenderUdpTunnelConnectionSettingsBytes = this->hostSenderUdpTunnelConnectionSettings.encode();
    out.append(encodeSize(hostSenderUdpTunnelConnectionSettingsBytes.size()));
    out.append(hostSenderUdpTunnelConnectionSettingsBytes);

    const auto& hostReceiverUdpTunnelConnectionSettingsBytes = this->hostReceiverUdpTunnelConnectionSettings.encode();
    out.append(encodeSize(hostReceiverUdpTunnelConnectionSettingsBytes.size()));
    out.append(hostReceiverUdpTunnelConnectionSettingsBytes);

    const auto& clientSenderUdpTunnelConnectionSettingsBytes = this->clientSenderUdpTunnelConnectionSettings.encode();
    out.append(encodeSize(clientSenderUdpTunnelConnectionSettingsBytes.size()));
    out.append(clientSenderUdpTunnelConnectionSettingsBytes);

    const auto& clientReceiverUdpTunnelConnectionSettingsBytes = this->clientReceiverUdpTunnelConnectionSettings.encode();
    out.append(encodeSize(clientReceiverUdpTunnelConnectionSettingsBytes.size()));
    out.append(clientReceiverUdpTunnelConnectionSettingsBytes);

    return out;
}

const QString TCPTunnelConnectionSettings::toString() const
{
    return QString("Connection ID: %1\nHost sender settings: %5\nHost receiver settings: %6\nClient sender settings: %7\n Client receiver settings: %8\n").arg(this->connectionId).arg(this->hostSenderUdpTunnelConnectionSettings.toString()).arg(this->hostReceiverUdpTunnelConnectionSettings.toString()).arg(this->clientSenderUdpTunnelConnectionSettings.toString()).arg(this->clientReceiverUdpTunnelConnectionSettings.toString());
}

// Private

const QByteArray TCPTunnelConnectionSettings::encodeUInt8(const uint8_t& value)
{
    char valueBytes[sizeof(value)] = {0};
    memcpy(valueBytes, (void*)&value, sizeof(value));
    QByteArray encodedValue = QByteArray(valueBytes, sizeof(value));
    return encodedValue;
}

const uint8_t TCPTunnelConnectionSettings::decodeUInt8(const QByteArray& bytes)
{
    uint8_t value = 0;
    memcpy(&value, bytes.mid(0).constData(), sizeof(value));
    return value;
}

const QByteArray TCPTunnelConnectionSettings::encodeInt(const int& value)
{
    char valueBytes[sizeof(value)] = {0};
    memcpy(valueBytes, (void*)&value, sizeof(value));
    QByteArray encodedValue = QByteArray(valueBytes, sizeof(value));
    return encodedValue;
}

const int TCPTunnelConnectionSettings::decodeInt(const QByteArray& bytes)
{
    int value = 0;
    memcpy(&value, bytes.mid(0).constData(), sizeof(value));
    return value;
}

const QByteArray TCPTunnelConnectionSettings::encodeSize(const size_t& value)
{
    char valueBytes[sizeof(value)] = {0};
    memcpy(valueBytes, (void*)&value, sizeof(value));
    QByteArray encodedValue = QByteArray(valueBytes, sizeof(value));
    return encodedValue;
}

const size_t TCPTunnelConnectionSettings::decodeSize(const QByteArray& bytes)
{
    size_t value = 0;
    memcpy(&value, bytes.mid(0).constData(), sizeof(value));
    return value;
}