#include "app.h"
#include "limits.h"
#include "version.h"
#include "tcptunnelpacketheader.h"
#include "tcptunnelpacket.h"
#include <thread>
#include <iostream>
#include <QTimer>
#include <QFile>

#define OPTION_OPERATION_MODE "m"
#define OPTION_PROXY_LISTEN_ADDRESS "l"
#define OPTION_PROXY_LISTEN_PORT "p"
#define OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS "s"
#define OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT "c"
#define OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS "e"
#define OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT "d"
#define OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS "r"
#define OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT "f"
#define OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS "o"
#define OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT "k"

App::App(int& argc, char** argv) : QCoreApplication(argc, argv)
{

}

void App::initialize()
{
    const auto& operationMode = this->parser.value(OPTION_OPERATION_MODE);
    const auto& proxyListenAddress = this->parser.value(OPTION_PROXY_LISTEN_ADDRESS);
    const auto& proxyListenPort = this->parser.value(OPTION_PROXY_LISTEN_PORT);
    const auto& udpTunnelSenderListenAddress = this->parser.value(OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS);
    const auto& udpTunnelSenderListenPort = this->parser.value(OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT);
    const auto& udpTunnelSenderEgressAddress = this->parser.value(OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS);
    const auto& udpTunnelSenderEgressPort = this->parser.value(OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT);
    const auto& udpTunnelReceiverListenAddress = this->parser.value(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS);
    const auto& udpTunnelReceiverListenPort = this->parser.value(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT);
    const auto& udpTunnelReceiverEgressAddress = this->parser.value(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS);
    const auto& udpTunnelReceiverEgressPort = this->parser.value(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT);

    qDebug().noquote() << "Application settings:";
    qDebug().noquote() << "Operation mode =" << operationMode;
    if(operationMode == "proxy")
    {
        qDebug().noquote() << "Proxy listen address =" << proxyListenAddress;
        qDebug().noquote() << "Proxy listen port =" << proxyListenPort;
    }
    qDebug().noquote() << "UDP tunnel sender listen address =" << udpTunnelSenderListenAddress;
    qDebug().noquote() << "UDP tunnel sender listen port =" << udpTunnelSenderListenPort;
    qDebug().noquote() << "UDP tunnel sender egress address =" << udpTunnelSenderEgressAddress;
    qDebug().noquote() << "UDP tunnel sender egress port =" << udpTunnelSenderEgressPort;
    qDebug().noquote() << "UDP tunnel receiver listen address =" << udpTunnelReceiverListenAddress;
    qDebug().noquote() << "UDP tunnel receiver listen port =" << udpTunnelReceiverListenPort;
    qDebug().noquote() << "UDP tunnel receiver egress address =" << udpTunnelReceiverEgressAddress;
    qDebug().noquote() << "UDP tunnel receiver egress port =" << udpTunnelReceiverEgressPort;

    UDPTunnelConnectionSettings udpTunnelConnectionSettings;
    udpTunnelConnectionSettings.senderIngressAddress = QHostAddress(udpTunnelSenderListenAddress);
    udpTunnelConnectionSettings.senderIngressPort = udpTunnelSenderListenPort.toInt();
    udpTunnelConnectionSettings.senderEgressAddress = QHostAddress(udpTunnelSenderEgressAddress);
    udpTunnelConnectionSettings.senderEgressPort = udpTunnelSenderEgressPort.toInt();
    udpTunnelConnectionSettings.receiverIngressAddress = QHostAddress(udpTunnelReceiverListenAddress);
    udpTunnelConnectionSettings.receiverIngressPort = udpTunnelReceiverListenPort.toInt();
    udpTunnelConnectionSettings.receiverEgressAddress = QHostAddress(udpTunnelReceiverEgressAddress);
    udpTunnelConnectionSettings.receiverEgressPort = udpTunnelReceiverEgressPort.toInt();

    if(operationMode == "proxy")
    {
        // Create a proxy server with a UDP tunnel
        this->proxyServer = std::make_unique<ProxyServer>(udpTunnelConnectionSettings);

        // When a client connects, keep listening for new connections
        QObject::connect(this->proxyServer.get(), &ProxyServer::newConnection, this, [this](){
            this->proxyServer->waitForNewConnection();
        });

        // Start listening for proxy client connections
        const bool& result = proxyServer->listen(QHostAddress(proxyListenAddress), static_cast<quint16>(proxyListenPort.toInt()));
        if(result)
        {
            qDebug().noquote() << "Proxy server is listening on port" << proxyListenPort;
        }
    }
    else if(operationMode == "client")
    {
        // Create a proxy client with a UDP tunnel
        this->proxyClient = std::make_unique<ProxyClient>(udpTunnelConnectionSettings);
    }
    else
    {
        qDebug() << "Invalid operation mode was set. Exiting.";
        std::exit(EXIT_FAILURE);
    }
}

void App::parseCommandLineOptions()
{
    this->parser.setApplicationDescription("HTTPS proxy with configurable data paths");

    std::vector<QCommandLineOption> commandLineOptions = {
        QCommandLineOption(OPTION_OPERATION_MODE, "Operation mode (\"proxy\" or \"client\")", "operationMode"),
        QCommandLineOption(OPTION_PROXY_LISTEN_ADDRESS, "Proxy listen address (required when operation mode is \"proxy\")", "proxyListenAddress"),
        QCommandLineOption(OPTION_PROXY_LISTEN_PORT, "Proxy listen port (required when operation mode is \"proxy\")", "proxyListenPort"),
        QCommandLineOption(OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS, "UDP tunnel sender listen address", "udpTunnelSenderListenAddress"),
        QCommandLineOption(OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT, "UDP tunnel sender listen port", "udpTunnelSenderListenPort"),
        QCommandLineOption(OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS, "UDP tunnel sender egress address", "udpTunnelSenderEgressAddress"),
        QCommandLineOption(OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT, "UDP tunnel sender egress port", "udpTunnelSenderEgressPort"),
        QCommandLineOption(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS, "UDP tunnel receiver listen address", "udpTunnelReceiverListenAddress"),
        QCommandLineOption(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT, "UDP tunnel receiver listen port", "udpTunnelReceiverListenPort"),
        QCommandLineOption(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS, "UDP tunnel receiver egress address", "udpTunnelReceiverEgressAddress"),
        QCommandLineOption(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT, "UDP tunnel receiver egress port", "udpTunnelReceiverEgressPort")
    };
    for(const auto& commandLineOption : commandLineOptions)
    {
        this->parser.addOption(commandLineOption);
    }
    const QCommandLineOption& helpOption = this->parser.addHelpOption();
    const QCommandLineOption& versionOption = this->parser.addVersionOption();
    this->parser.parse(this->arguments());
    if (this->parser.isSet(versionOption))
    {
        App::printVersionInformation();
        std::exit(EXIT_SUCCESS);
    }
    if (this->parser.isSet(helpOption))
    {
        App::printLicenceInformation();
        qDebug() << "";
        App::printHelpInformation(commandLineOptions);
        std::exit(EXIT_SUCCESS);
    }

    // Make sure all required command line options are set
    App::checkCommandLineOptions(this->parser);
}

void App::checkCommandLineOptions(QCommandLineParser& parser)
{
    // Check if the operation mode is set
    if(!parser.isSet(OPTION_OPERATION_MODE))
    {
        qDebug().noquote() << "No operation mode was given. Exiting.";
        std::exit(EXIT_FAILURE);
    }

    // Check if the listen address and listen port are given when running the application in proxy mode
    if(parser.isSet(OPTION_OPERATION_MODE))
    {
        // Check if the operation mode is valid
        if(parser.value(OPTION_OPERATION_MODE) != "proxy" && parser.value(OPTION_OPERATION_MODE) != "client")
        {
            qDebug().noquote() << "Invalid operation mode given. Expected \"proxy\" or \"client\". Exiting.";
            std::exit(EXIT_FAILURE);
        }

        if(parser.value(OPTION_OPERATION_MODE) == "proxy")
        {
            if(!parser.isSet(OPTION_PROXY_LISTEN_ADDRESS))
            {
                qDebug().noquote() << "Operation mode is set to \"proxy\", but no proxy listen address was given. Exiting.";
                std::exit(EXIT_FAILURE);
            }
            if(!parser.isSet(OPTION_PROXY_LISTEN_PORT))
            {
                qDebug().noquote() << "Operation mode is set to \"proxy\", but no proxy listen port was given. Exiting.";
                std::exit(EXIT_FAILURE);
            }
        }
    }

    // Check if the listen address and listen port are given for the UDP tunnel sender
    if(!parser.isSet(OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS))
    {
        qDebug().noquote() << "No listen address for the UDP tunnel packet sender was given. Exiting";
        std::exit(EXIT_FAILURE);
    }
    if(!parser.isSet(OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT))
    {
        qDebug().noquote() << "No listen port for the UDP tunnel packet sender was given. Exiting";
        std::exit(EXIT_FAILURE);
    }

    // Check if the egress address and egress port are given for the UDP tunnel sender
    if(!parser.isSet(OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS))
    {
        qDebug().noquote() << "No egress address for the UDP tunnel packet sender was given. Exiting";
        std::exit(EXIT_FAILURE);
    }
    if(!parser.isSet(OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT))
    {
        qDebug().noquote() << "No egress port for the UDP tunnel packet sender was given. Exiting";
        std::exit(EXIT_FAILURE);
    }

    // Check if the listen address and listen port are given for the UDP tunnel receiver
    if(!parser.isSet(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS))
    {
        qDebug().noquote() << "No listen address for the UDP tunnel packet receiver was given. Exiting";
        std::exit(EXIT_FAILURE);
    }
    if(!parser.isSet(OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT))
    {
        qDebug().noquote() << "No listen port for the UDP tunnel packet receiver was given. Exiting";
        std::exit(EXIT_FAILURE);
    }

    // Check if the egress address and egress port are given for the UDP tunnel receiver
    if(!parser.isSet(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS))
    {
        qDebug().noquote() << "No egress address for the UDP tunnel packet receiver was given. Exiting";
        std::exit(EXIT_FAILURE);
    }
    if(!parser.isSet(OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT))
    {
        qDebug().noquote() << "No egress port for the UDP tunnel packet receiver was given. Exiting";
        std::exit(EXIT_FAILURE);
    }
}

void App::printVersionInformation()
{
    qDebug().noquote() << QString("%1 version: %2").arg(APPLICATION_NAME, APPLICATION_VERSION);
}

void App::printHelpInformation(const std::vector<QCommandLineOption>& commandLineOptions)
{
    for(const auto& commandLineOption : commandLineOptions)
    {
        qDebug().noquote() << QString("-%1").arg(commandLineOption.names().at(0)) << "\t" << "\t" << commandLineOption.description();
    }
    qDebug().noquote() << "";
    qDebug().noquote() << "Example usage (proxy server mode with network traffic going to/from 127.0.0.1):";
    qDebug().noquote() << QString("./udpproxy -%1 proxy -%2 0.0.0.0 -%3 8080 -%4 0.0.0.0 -%5 1111 -%6 127.0.0.1 -%7 2222 -%8 0.0.0.0 -%9 3333 -%10 127.0.0.1 -%11 4444").arg(OPTION_OPERATION_MODE, OPTION_PROXY_LISTEN_ADDRESS, OPTION_PROXY_LISTEN_PORT, OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS, OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT, OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS, OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT, OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS, OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT, OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS, OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT);
    qDebug().noquote() << "";
    qDebug().noquote() << "Example usage (proxy client mode with network traffic going to/from 127.0.0.1):";
    qDebug().noquote() << QString("./udpproxy -%1 client -%2 0.0.0.0 -%3 4444 -%4 127.0.0.1 -%5 3333 -%6 0.0.0.0 -%7 2222 -%8 127.0.0.1 -%9 1111").arg(OPTION_OPERATION_MODE, OPTION_UDP_TUNNEL_SENDER_LISTEN_ADDRESS, OPTION_UDP_TUNNEL_SENDER_LISTEN_PORT, OPTION_UDP_TUNNEL_SENDER_EGRESS_ADDRESS, OPTION_UDP_TUNNEL_SENDER_EGRESS_PORT, OPTION_UDP_TUNNEL_RECEIVER_LISTEN_ADDRESS, OPTION_UDP_TUNNEL_RECEIVER_LISTEN_PORT, OPTION_UDP_TUNNEL_RECEIVER_EGRESS_ADDRESS, OPTION_UDP_TUNNEL_RECEIVER_EGRESS_PORT);
    qDebug().noquote() << "";
}

void App::printLicenceInformation()
{
    const std::vector<QString> lines = {
        "UDP proxy: A HTTPS proxy with configurable network paths",
        "Copyright (C) 2025  Tom Niesse",
        "",
        "This program is free software: you can redistribute it and/or modify",
        "it under the terms of the GNU General Public License as published by",
        "the Free Software Foundation, either version 3 of the License, or",
        "(at your option) any later version.",
        "",
        "This program is distributed in the hope that it will be useful,",
        "but WITHOUT ANY WARRANTY; without even the implied warranty of",
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
        "GNU General Public License for more details.",
        "",
        "You should have received a copy of the GNU General Public License",
        "along with this program.  If not, see <https://www.gnu.org/licenses/>."
    };

    for(const auto& line : lines)
    {
        qDebug().noquote() << line;
    }
}
