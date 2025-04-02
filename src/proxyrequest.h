#ifndef PROXYREQUEST_H
#define PROXYREQUEST_H
#include <QString>

class ProxyRequest
{
public:
    static void executeRequest();
    static bool validate(const QString proxyRequest);
    static const QString extractCommand(const QString proxyRequest);
    static const std::pair<QString, quint16> extractHostAndPort(const QString proxyRequest);
};

#endif // PROXYREQUEST_H
