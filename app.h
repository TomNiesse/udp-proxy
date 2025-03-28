#ifndef APP_H
#define APP_H

#include <QObject>
#include <QCoreApplication>
#include "proxyserver.h"
#include "proxyclient.h"

class App : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(App)
public:
    App(int& argc, char** argv);

private:
    std::unique_ptr<ProxyServer> proxyServer;
    std::unique_ptr<ProxyClient> proxyClient;
};

#endif // APP_H
