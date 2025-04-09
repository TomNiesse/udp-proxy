#include "proxyrequest.h"
#include <QDebug>

bool ProxyRequest::validate(const QString& proxyRequest)
{
    // Check if the request contains at least a command and a hostname:port combination
    const QStringList proxyRequestSplit = proxyRequest.split(" ");
    if(proxyRequestSplit.size() < 2 || !proxyRequestSplit[1].contains(":"))
    {
        return false;
    }

    // Check if the CONNECT request is given. If not, disregard the request.
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

const std::pair<QString, quint16> ProxyRequest::extractHostAndPort(const QString& proxyRequest)
{
    // Extract the host:port combo first
    const QStringList& proxyRequestSplit = proxyRequest.split(" ");
    const QString& hostAndPort = proxyRequestSplit[1];

    // Split the hostname and port
    const QStringList& hostAndPortSplit = hostAndPort.split(":");
    const QString& host = hostAndPortSplit[0];
    const quint16& port = static_cast<quint16>(hostAndPortSplit[1].toInt());

    return std::make_pair(host, port);
}
