#ifndef APP_H
#define APP_H

#include <QObject>
#include <QCoreApplication>
#include <QCommandLineParser>
#include "proxyserver.h"
#include "clientconnectionmanager.h"

class App : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(App)
public:
    explicit App(int& argc, char** argv);
    void initialize();
    void parseCommandLineOptions();

private:
    static void checkCommandLineOptions(QCommandLineParser& parser);
    static void printVersionInformation();
    static void printHelpInformation(const std::vector<QCommandLineOption>& commandLineOptions);
    static void printLicenceInformation();
    QCommandLineParser parser;

    std::unique_ptr<ProxyServer> proxyServer;
    std::unique_ptr<ClientConnectionManager> clientConnectionManager;
};

#endif // APP_H
