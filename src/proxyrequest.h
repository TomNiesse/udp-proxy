#ifndef PROXYREQUEST_H
#define PROXYREQUEST_H

#include <QString>

class ProxyRequest
{
public:
    static bool validate(const QString& proxyRequest);
    static const QString extractCommand(const QString& proxyRequest);
    static const std::pair<QString, uint16_t> extractHostAndPort(const QString& proxyRequest);
};

#endif // PROXYREQUEST_H
