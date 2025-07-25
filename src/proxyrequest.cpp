#include "proxyrequest.h"
#include <QStringList>

bool ProxyRequest::validate(const QString& proxyRequest)
{
    const QStringList& proxyRequestSplit = proxyRequest.split(" ");
    if(proxyRequestSplit.size() < 2 || !proxyRequestSplit[1].contains(":"))
    {
        return false;
    }

    if(ProxyRequest::extractCommand(proxyRequest) == "CONNECT")
    {
        return true;
    }
    return false;
}

const QString ProxyRequest::extractCommand(const QString& proxyRequest)
{
    for(qsizetype pos = 0; pos < proxyRequest.length(); pos++)
    {
        if(proxyRequest[pos] == ' ')
        {
            const auto& command = proxyRequest.mid(0, pos);
            return command.toUpper();
        }
    }
    return "";
}

const std::pair<QString, uint16_t> ProxyRequest::extractHostAndPort(const QString& proxyRequest)
{
    const QStringList& proxyRequestSplit = proxyRequest.split(" ");
    const QString& hostAndPort = proxyRequestSplit[1];

    const QStringList& hostAndPortSplit = hostAndPort.split(":");
    const QString& host = hostAndPortSplit[0];
    const uint16_t& port = static_cast<uint16_t>(hostAndPortSplit[1].toUInt());

    return std::make_pair(host, port);
}
