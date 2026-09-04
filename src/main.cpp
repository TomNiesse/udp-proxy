#include "app.h"
#include "version.h"

#include "udptunnelpacketreceiver.h"
#include "udptunnelpacketsender.h"
#include "udptunnelpacket.h"
#include "udptunnelpacketmissingchunklist.h"

#include "hostconnection.h"
#include "clientconnection.h"
#include "tcptunnelclientconnection.h"
#include "tcptunnelhostconnection.h"
#include "proxytunnelhost.h"
#include "proxytunnelclient.h"
#include "udptunnelpacketbuffer.h"
#include "tcptunnelconnectionsettings.h"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QFile>

int main(int argc, char *argv[])
{
    App app(argc, argv);

    // TCPConnectionStatus t1 = TCPConnectionStatus::CONNECTED;
    // const auto& t1Bytes = TCPTunnelConnectionSettings::encodeTcpConnectionStatus(t1);
    // TCPConnectionStatus t2 = TCPTunnelConnectionSettings::decodeTcpConnectionStatus(t1Bytes);

    // qDebug() << t1 << t2;

    // int value = 69420;
    // const auto& encoded = TCPTunnelConnectionSettings::encodeInt(value);
    // const auto& decoded = TCPTunnelConnectionSettings::decodeInt(encoded);

    // qDebug() << value << decoded;




    // const UDPTunnelConnectionSettings hostSenderUdpTunnelConnectionSettings = {"127.0.0.1b", 1111, "127.0.0.1c", 2222};
    // const UDPTunnelConnectionSettings hostReceiverUdpTunnelConnectionSettings = {"127.0.0.1d", 3333, "127.0.0.1e", 4444};
    // const UDPTunnelConnectionSettings clientSenderUdpTunnelConnectionSettings = {"127.0.0.1f", 5555, "127.0.0.1g", 6666};
    // const UDPTunnelConnectionSettings clientReceiverUdpTunnelConnectionSettings = {"127.0.0.1h", 7777, "127.0.0.1i", 8888};

    // TCPTunnelConnectionSettings s1;
    // s1.setConnectionId(69420);
    // s1.setHostSenderUdpTunnelConnectionSettings(hostSenderUdpTunnelConnectionSettings);
    // s1.setHostReceiverUdpTunnelConnectionSettings(hostReceiverUdpTunnelConnectionSettings);
    // s1.setClientSenderUdpTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettings);
    // s1.setClientReceiverUdpTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettings);

    // qDebug() << s1.toString();

    // const auto& bytes = s1.encode();
    // TCPTunnelConnectionSettings s2;
    // s2.fromBytes(bytes); // hier gaat nog iets fout!

    // qDebug() << s2.toString();
    // qDebug() << "";

    // exit(0);

    // QTcpSocket s;
    // QObject::connect(&s, &QTcpSocket::connected, [](){
    //     qDebug() << "Connected!";
    // });
    // s.connectToHost("127.0.0.1", 80);
    // QEventLoop eventLoop2;
    // eventLoop2.exec();
    // exit(0);





    const UDPTunnelConnectionSettings hostSenderUdpTunnelConnectionSettings = {"10.0.2.15", 1111, "127.0.0.1", 2222};
    const UDPTunnelConnectionSettings clientReceiverUdpTunnelConnectionSettings = {"127.0.0.1", 2222, "10.0.2.15", 1111};
    const UDPTunnelConnectionSettings clientSenderUdpTunnelConnectionSettings = {"10.0.2.15", 3333, "127.0.0.1", 4444};
    const UDPTunnelConnectionSettings hostReceiverUdpTunnelConnectionSettings = {"127.0.0.1", 4444, "10.0.2.15", 3333};

    TCPTunnelConnectionSettings connectionManagerTcpTunnelConnectionSettings;
    connectionManagerTcpTunnelConnectionSettings.setHostSenderUdpTunnelConnectionSettings(hostSenderUdpTunnelConnectionSettings);
    connectionManagerTcpTunnelConnectionSettings.setHostReceiverUdpTunnelConnectionSettings(hostReceiverUdpTunnelConnectionSettings);
    connectionManagerTcpTunnelConnectionSettings.setClientSenderUdpTunnelConnectionSettings(clientSenderUdpTunnelConnectionSettings);
    connectionManagerTcpTunnelConnectionSettings.setClientReceiverUdpTunnelConnectionSettings(clientReceiverUdpTunnelConnectionSettings);

    TCPHostConnectionManager hostConnectionManager(connectionManagerTcpTunnelConnectionSettings);
    TCPClientConnectionManager clientConnectionManager(connectionManagerTcpTunnelConnectionSettings.getClientSenderUdpTunnelConnectionSettings(), connectionManagerTcpTunnelConnectionSettings.getClientReceiverUdpTunnelConnectionSettings());

    // TODO: insert an if block here that actually checks the connection ID before assuming it's ours!!!!!
    QEventLoop eventLoop;
    for(size_t connection = 0; connection < 10; connection++)
    {
        const auto connectionId = hostConnectionManager.createTcpTunnelConnection();
        QObject::connect(&hostConnectionManager, &TCPHostConnectionManager::tcpTunnelConnectionInitialized, [&hostConnectionManager, connectionId](const size_t _connectionId){
            QTimer::singleShot(0, [&hostConnectionManager, connectionId](){
                QEventLoop eventLoop;

                qDebug() << "that's my connection ID!";
                qDebug() << "";

                auto tcpTunnelConnection = hostConnectionManager.getTcpTunnelConnection(connectionId);
                if(tcpTunnelConnection == nullptr)
                {
                    qDebug() << "hier ging iets gruwelijk fout!";
                }

                QObject::connect(tcpTunnelConnection.get(), &TCPTunnelHostConnection::connected, &eventLoop, &QEventLoop::quit);
                QObject::connect(tcpTunnelConnection.get(), &TCPTunnelHostConnection::disconnected, &eventLoop, &QEventLoop::quit);
                QObject::connect(tcpTunnelConnection.get(), &TCPTunnelHostConnection::receivedData, [](const QByteArray& receivedData){
                    qDebug() << QDateTime::currentDateTime() << receivedData.size();
                });

                qDebug() << QDateTime::currentDateTime() << "START!";

                tcpTunnelConnection->connect("127.0.0.1", 80);
                eventLoop.exec();
                tcpTunnelConnection->write("GET /100mb.bin HTTP/1.1\nHost: 127.0.0.1\nConnection: close\r\n\r\n");
                eventLoop.exec();

                qDebug() << QDateTime::currentDateTime() << "CONNECTION ID" << connectionId << " HAS FINISHED!";
            });
        });

        hostConnectionManager.initializeTcpTunnelConnection(connectionId);
    }
    eventLoop.exec();




    qDebug() << "";






    // std::vector<UDPTunnelPacket> l;
    // size_t packetId = 0;
    // for(size_t chunkId = 0; chunkId < 100; chunkId++)
    // {
    //     UDPTunnelPacketHeader h;
    //     h.setPacketId(packetId);
    //     h.setChunkId(chunkId);
    //     UDPTunnelPacket p;
    //     p.setHeader(h);
    //     l.push_back(p);
    // }

    // UDPTunnelPacketBuffer b(l);
    // const auto& v = b.getInitialPacketChunks();
    // for(const auto& i : v)
    // {
    //     qDebug() << i.getHeader().toString();
    // }
    // qDebug() << v.size();

    // size_t responseId = 0;
    // b.increaseBufferSize(10);
    // for(const auto& p : l)
    // {
    //     const auto& a = b.handleReceivedPacket(p);
    //     for(const auto& c : a)
    //     {
    //         qDebug() << "RESPONSES" << responseId << "->" << c.getHeader().toString();
    //     }
    //     responseId++;
    // }

    // exit(0);

    // UDPTunnelConnectionSettings hostSenderSettings("127.0.0.1", 6969, "127.0.0.1", 0);
    // UDPTunnelConnectionSettings hostReceiverSettings("127.0.0.1", 1111, "127.0.0.1", 2222);

    // UDPTunnelConnectionSettings clientSenderSettings("127.0.0.1", 2222, "127.0.0.1", 1111);
    // UDPTunnelConnectionSettings clientReceiverSettings("127.0.0.1", 0, "127.0.0.1", 6969);

    // ProxyTunnelClient proxyTunnelConnectionClient(clientSenderSettings, clientReceiverSettings);
    // hostSenderSettings.setEgressPort(proxyTunnelConnectionClient.getReceiverUdpTunnelConnectionSettings().getIngressPort());

    // ProxyTunnelHost proxyTunnelConnectionHost(hostSenderSettings, hostReceiverSettings);

    // QEventLoop eventLoop;
    // QObject::connect(&proxyTunnelConnectionHost, &ProxyTunnelHost::connected, &eventLoop, &QEventLoop::quit);
    // QObject::connect(&proxyTunnelConnectionHost, &ProxyTunnelHost::disconnected, &eventLoop, &QEventLoop::quit);
    // QObject::connect(&proxyTunnelConnectionHost, &ProxyTunnelHost::receivedData, [](const QByteArray& receivedData){
    //     qDebug() << QDateTime::currentDateTime() << receivedData.size();
    // });

    // qDebug() << QDateTime::currentDateTime() << "START!";

    // proxyTunnelConnectionHost.connect("127.0.0.1", 80);
    // eventLoop.exec();
    // proxyTunnelConnectionHost.write("GET /100mb.bin HTTP/1.1\nHost: 127.0.0.1\nConnection: close\r\n\r\n");
    // eventLoop.exec();

    // qDebug() << QDateTime::currentDateTime() << "FINISHED!";

    // exit(0);

    // UDPTunnelConnectionSettings hostSenderSettings("127.0.0.1", 6969, "127.0.0.1", 7070);
    // UDPTunnelConnectionSettings hostReceiverSettings("127.0.0.1", 1111, "127.0.0.1", 2222);

    // UDPTunnelConnectionSettings clientSenderSettings("127.0.0.1", 2222, "127.0.0.1", 1111);
    // UDPTunnelConnectionSettings clientReceiverSettings("127.0.0.1", 7070, "127.0.0.1", 6969);

    // UDPTunnelConnection udpHost(hostSenderSettings, hostReceiverSettings, "host");
    // UDPTunnelConnection udpClient(clientSenderSettings, clientReceiverSettings, "client");

    // QByteArray previousTcpData;

    // for(size_t id = 0; id < 1; id ++)
    // {
    //     TCPTunnelHostConnection tcpHost;
    //     TCPTunnelClientConnection tcpClient;

    //     // Send all received data over UDP
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::writeTcpTunnelPacket, &udpHost, &UDPTunnelConnection::sendData);
    //     QObject::connect(&tcpClient, &TCPTunnelClientConnection::writeTcpTunnelPacket, &udpClient, &UDPTunnelConnection::sendData);

    //     // Handle received data from UDP
    //     QObject::connect(&udpHost, &UDPTunnelConnection::receivedData, &tcpHost, &TCPTunnelHostConnection::handleTcpTunnelPacket);
    //     QObject::connect(&udpClient, &UDPTunnelConnection::receivedData, &tcpClient, &TCPTunnelClientConnection::handleTcpTunnelPacket);

    //     QByteArray tcpData;

    //     qDebug() << QDateTime::currentDateTime() << "Connection #" << (id+1) << "START!";

    //     QEventLoop eventLoop;
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::connected, [&eventLoop](){
    //         qDebug() << QDateTime::currentDateTime() << "Connected!";
    //         eventLoop.quit();
    //     });
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::connectionTimeout, [&eventLoop](){
    //         qDebug() << QDateTime::currentDateTime() << "Connection timeout";
    //         // eventLoop.quit();
    //         exit(1);
    //     });
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::receivedData, [&eventLoop, &tcpData](const QByteArray receivedData){
    //         tcpData.append(receivedData);

    //         qDebug() << QDateTime::currentDateTime() << "Bytes received:" << receivedData.size();
    //         // eventLoop.quit();
    //     });
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::bytesWritten, [&eventLoop](){
    //         qDebug() << QDateTime::currentDateTime() << "Bytes written!";
    //         // eventLoop.quit();
    //     });
    //     QObject::connect(&tcpHost, &TCPTunnelHostConnection::disconnected, [&eventLoop](){
    //         qDebug() << QDateTime::currentDateTime() << "Disconnected!";
    //         eventLoop.quit();
    //     });

    //     tcpHost.connect("127.0.0.1", 80);
    //     eventLoop.exec();
    //     tcpHost.write("GET /random.bin HTTP/1.1\nHost: 127.0.0.1\nConnection: close\r\n\r\n");
    //     eventLoop.exec();

    //     QObject::disconnect(&tcpHost, nullptr, nullptr, nullptr);
    //     QObject::disconnect(&tcpClient, nullptr, nullptr, nullptr);
    //     QObject::disconnect(&udpHost, nullptr, nullptr, nullptr);
    //     QObject::disconnect(&udpClient, nullptr, nullptr, nullptr);

    //     // // Write received data to image file
    //     // const auto& filePath = QString("/home/vm/index_%1.html").arg(id);
    //     // QFile targetFile(filePath);
    //     // if(!targetFile.open(QIODevice::WriteOnly))
    //     // {
    //     //     qDebug() << QDateTime::currentDateTime() << "NO OPEN!";
    //     //     exit(1);
    //     // }
    //     // else
    //     // {
    //     //     targetFile.write(tcpData, tcpData.size());;
    //     // }
    //     // targetFile.close();

    //     qDebug() << QDateTime::currentDateTime() << "Connection #" << (id + 1) << "END!";

    //     if(previousTcpData.size() <= 0)
    //     {
    //         previousTcpData = tcpData;
    //     }
    //     else
    //     {
    //         assert(previousTcpData.size() == tcpData.size());

    //         size_t numberOfDifferences = 0;
    //         for(size_t byteId = 0; byteId < tcpData.size(); byteId++)
    //         {
    //             if(tcpData.at(byteId) != previousTcpData.at(byteId))
    //             {
    //                 if(byteId > 270)
    //                 {
    //                     qDebug() << QDateTime::currentDateTime() << "Byte at pos" << byteId << "differs";
    //                     numberOfDifferences++;
    //                 }
    //             }
    //         }

    //         qDebug() << QDateTime::currentDateTime() << "Number of differences:" << numberOfDifferences;
    //         assert(numberOfDifferences == 0);
    //     }

    //     // QEventLoop cleanupEventLoop;
    //     // QTimer cleanupTimer;
    //     // QObject::connect(&cleanupTimer, &QTimer::timeout, &cleanupEventLoop, &QEventLoop::quit);
    //     // cleanupTimer.setSingleShot(true);
    //     // cleanupTimer.setInterval(1);
    //     // cleanupTimer.start();
    //     // cleanupEventLoop.exec();
    //     // QObject::disconnect(&cleanupTimer, &QTimer::timeout, &cleanupEventLoop, &QEventLoop::quit);
    // }

    // exit(0);



    // TCPTunnelPacketHeader header;
    // header.setPacketType(TCPTunnelPacketHeaderType::TCP_CLOSE_CONNECTION);
    // header.setHost("127.0.0.1");
    // header.setPort(6969);
    // TCPTunnelPacketHeader header2(header.encode());
    // assert(header.getPacketType() == header2.getPacketType());
    // // assert(header.getHost() == header2.getHost());
    // // assert(header.getPort() == header2.getPort());
    // qDebug() << QDateTime::currentDateTime() << "";

    // TCPTunnelPacket packet;
    // packet.setHeader(header);
    // packet.setPayload("abcd");
    // TCPTunnelPacket packet2(packet.encode());

    // assert(packet.getHeader().encode() == packet2.getHeader().encode());
    // assert(packet.getPayload() == packet2.getPayload());

    // // exit(0);

    // // Chunk manager time test (is the manager a bit fast?)
    // {
    //     UDPTunnelPacketChunkManager manager;
    //     const auto& packetId = 0;
    //     size_t numberOfChunks = 1024;
    //     qDebug() << QDateTime::currentDateTime() << "add chunks START!";
    //     for(size_t chunkId = 0; chunkId <= numberOfChunks; chunkId++)
    //     {
    //         // qDebug() << QDateTime::currentDateTime() << "add chunk START!";
    //         UDPTunnelPacketHeader header;
    //         header.setPacketId(packetId);
    //         header.setChunkId(chunkId);
    //         UDPTunnelPacket packet(header, "a");
    //         manager.addChunk(packet);
    //         // qDebug() << QDateTime::currentDateTime() << "add chunk END!";
    //     }
    //     qDebug() << QDateTime::currentDateTime() << "add chunks END!";

    //     const auto& missingPackets = manager.getMissingChunkIds(packetId, 0, numberOfChunks);
    //     qDebug() << QDateTime::currentDateTime() << "number of missing chunks:" << missingPackets.size();
    // }

    // // Is the payload still correct when the chunks are received out of order?
    // {
    //     UDPTunnelPacketChunkManager manager;

    //     {
    //         UDPTunnelPacketHeader header;
    //         header.setPacketId(0);
    //         header.setChunkId(1);
    //         UDPTunnelPacket packet(header, "b");
    //         manager.addChunk(packet);
    //     }

    //     {
    //         UDPTunnelPacketHeader header;
    //         header.setPacketId(0);
    //         header.setChunkId(0);
    //         UDPTunnelPacket packet(header, "a");
    //         manager.addChunk(packet);
    //     }

    //     {
    //         UDPTunnelPacketHeader header;
    //         header.setPacketId(0);
    //         header.setChunkId(3);
    //         UDPTunnelPacket packet(header, "d");
    //         manager.addChunk(packet);
    //     }

    //     {
    //         UDPTunnelPacketHeader header;
    //         header.setPacketId(0);
    //         header.setChunkId(2);
    //         UDPTunnelPacket packet(header, "c");
    //         manager.addChunk(packet);
    //     }

    //     const auto& payload = manager.convertChunksToPayload(0);
    //     assert(payload == "abcd");
    // }

    // exit(0);

    // TCPTunnelClientConnection conn1;

    // // Test if a connection can be made
    // QEventLoop eventLoop;
    // QObject::connect(&conn1, &TCPTunnelClientConnection::connected, [&eventLoop](){
    //     qDebug() << QDateTime::currentDateTime() << "Connected!";
    //     eventLoop.quit();
    // });
    // QObject::connect(&conn1, &TCPTunnelClientConnection::connectionTimeout, [&eventLoop](){
    //     qDebug() << QDateTime::currentDateTime() << "Connection timeout";
    //     eventLoop.quit();
    //     exit(1);
    // });
    // QObject::connect(&conn1, &TCPTunnelClientConnection::receivedData, [&eventLoop](const QByteArray receivedData){
    //     qDebug() << QDateTime::currentDateTime() << "Bytes received:" << receivedData;
    //     // eventLoop.quit();
    // });
    // QObject::connect(&conn1, &TCPTunnelClientConnection::bytesWritten, [&eventLoop](){
    //     qDebug() << QDateTime::currentDateTime() << "Bytes written!";
    //     eventLoop.quit();
    // });
    // QObject::connect(&conn1, &TCPTunnelClientConnection::disconnected, [&eventLoop](){
    //     qDebug() << QDateTime::currentDateTime() << "Disconnected!";
    //     eventLoop.quit();
    // });
    // qDebug() << QDateTime::currentDateTime() << "Connect!";
    // conn1.handleTcpTunnelPacket(TCPTunnelPacket::createConnectRequest("127.0.0.1", 80));
    // eventLoop.exec();
    // conn1.handleTcpTunnelPacket(TCPTunnelPacket::createWriteRequest("GET / HTTP/1.1\nHost: 127.0.0.1\nConnection: close\r\n\r\n"));
    // eventLoop.exec();
    // // conn1.handleTcpTunnelPacket(TCPTunnelPacket::createDisconnectRequest());
    // eventLoop.exec();

    // exit(0);

    // UDPTunnelConnectionSettings senderSettings("127.0.0.1", 6969, "127.0.0.1", 7070);
    // UDPTunnelConnectionSettings receiverSettings("127.0.0.1", 7070, "127.0.0.1", 6969);
    // UDPTunnelConnection udpTunnelConnection(senderSettings, receiverSettings);

    // QObject::connect(&udpTunnelConnection, &UDPTunnelConnection::dataSent, [](){
    //     qDebug() << QDateTime::currentDateTime() << "data sent!";
    //     exit(0);
    // });
    // QByteArray data = "";
    // for(size_t charId = 0; charId < 1024*2024*1024; charId++)
    // {
    //     data.push_back('a');
    // }
    // QObject::connect(&udpTunnelConnection, &UDPTunnelConnection::receivedData, [&data](const QByteArray receivedData){
    //     // qDebug() << QDateTime::currentDateTime() << "data received:" << receivedData;
    //     qDebug() << QDateTime::currentDateTime() << "data size:" << receivedData.size();
    //     qDebug() << QDateTime::currentDateTime() << "data size K:" << receivedData.size() / 1024;
    //     qDebug() << QDateTime::currentDateTime() << "data size M:" << receivedData.size() / 1024 / 1024;
    //     if(data == receivedData)
    //     {
    //         qDebug() << QDateTime::currentDateTime() << "Data matches!";
    //         // exit(0);
    //     }
    //     else
    //     {
    //         qDebug() << QDateTime::currentDateTime() << "Data no matches! len:" << receivedData.size() << data.size();
    //         qDebug() << "";
    //     }
    // });

    // for(size_t sendId = 0; sendId < 1; sendId++)
    // {
    //     udpTunnelConnection.sendData(data);
    // }
    // // data = "abcd";
    // // udpTunnelConnection.sendData(data);

    // QEventLoop eventLoop;
    // eventLoop.exec();


    // TODO: mediteer op een manier om maar 1x poorten te hoeven reserveren, maar wel efficient
    //       connection management te kunnen doen. Iets met vectors aan TCP connecties misschien?
    //       Linked list? En dan moet een vector van pointers erbij voor de snelle hashmap-achtige toegang?
    //       Iets in die geest in ieder geval. De TCP host en client + UDP tunnel werken nu eindelijk weer goed,
    //       nu nog "even" de rest, en dan alles weer aan de proxy server lijmen.

    // UDPTunnelConnectionSettings hostSenderSettings("127.0.0.1", 6969, "127.0.0.1", 7070);
    // UDPTunnelConnectionSettings hostReceiverSettings("127.0.0.1", 1111, "127.0.0.1", 2222);
    // ProxyTunnelHostConnection hostConnection(hostSenderSettings, hostReceiverSettings);

    // UDPTunnelConnectionSettings clientSenderSettings("127.0.0.1", 2222, "127.0.0.1", 1111);
    // UDPTunnelConnectionSettings clientReceiverSettings("127.0.0.1", 7070, "127.0.0.1", 6969);
    // ClientConnection clientConnection(clientSenderSettings, clientReceiverSettings);

    // for(size_t id = 0; id < 1000; id++)
    // {
    //     qDebug() << QDateTime::currentDateTime() << "Connect";
    //     QEventLoop connectionEventLoop;
    //     QObject::connect(&hostConnection, &ProxyTunnelHostConnection::clientIsConnected, &connectionEventLoop, &QEventLoop::quit);
    //     hostConnection.connectToHost("127.0.0.1", 80);
    //     connectionEventLoop.exec();
    //     QObject::disconnect(&hostConnection, &ProxyTunnelHostConnection::clientIsConnected, &connectionEventLoop, &QEventLoop::quit);

    //     qDebug() << QDateTime::currentDateTime() << "Request";
    //     QEventLoop eventLoop;
    //     size_t bytesReceived = 0;
    //     QObject::connect(&hostConnection, &ProxyTunnelHostConnection::receivedData, [&eventLoop, &bytesReceived](const QByteArray& data){
    //         const size_t& expectedDataSize = 1642220;
    //         bytesReceived += data.size();
    //         if(bytesReceived >= expectedDataSize)
    //         {
    //             eventLoop.quit();
    //         }
    //     });
    //     hostConnection.send("GET /image.jpg HTTP/1.1\nHost: 127.0.0.1\r\n\r\n");
    //     eventLoop.exec();
    //     QObject::disconnect(&eventLoop, nullptr, nullptr, nullptr);

    //     for(size_t time = 0; time < 1000; time++)
    //     {
    //         QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    //         QThread::msleep(1);
    //     }

    //     qDebug() << QDateTime::currentDateTime() << "Disconnect";
    //     QEventLoop disconnectionEventLoop;
    //     QObject::connect(&hostConnection, &ProxyTunnelHostConnection::clientWasDisconnected, &disconnectionEventLoop, &QEventLoop::quit);
    //     hostConnection.disconnect();
    //     disconnectionEventLoop.exec();
    //     QObject::disconnect(&hostConnection, &ProxyTunnelHostConnection::clientWasDisconnected, &disconnectionEventLoop, &QEventLoop::quit);

    //     qDebug() << QDateTime::currentDateTime() << "End of loop";

    //     // QThread::msleep(1000);
    // }

    // hostConnection.disconnect();
    // hostConnection.quit();

    QCoreApplication::processEvents();



    exit(0);

    // QCoreApplication::setApplicationName(APPLICATION_NAME);
    // QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    // app.parseCommandLineOptions();
    // app.initialize();

    return app.exec();
}
