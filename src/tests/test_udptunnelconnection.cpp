#include "test_udptunnelconnection.h"
#include <QSignalSpy>

test_udptunnelconnection::test_udptunnelconnection()
{

}

test_udptunnelconnection::~test_udptunnelconnection()
{

}

// Private slots

void test_udptunnelconnection::testDataTransferSelfTest_data()
{
    QTest::addColumn<UDPTunnelConnectionSettings>("senderUdpTunnelConnectionSettings");
    QTest::addColumn<UDPTunnelConnectionSettings>("receiverUdpTunnelConnectionSettings");

    const UDPTunnelConnectionSettings senderUdpTunnelConnectionSettings = {"127.0.0.1", 1111, "127.0.0.1", 2222};
    const UDPTunnelConnectionSettings receiverUdpTunnelConnectionSettings = {"127.0.0.1", 2222, "127.0.0.1", 1111};

    QTest::newRow("Data transfer self test using localhost and valid ports") << senderUdpTunnelConnectionSettings << receiverUdpTunnelConnectionSettings;
}

void test_udptunnelconnection::testDataTransferSelfTest()
{
    QFETCH(UDPTunnelConnectionSettings, senderUdpTunnelConnectionSettings);
    QFETCH(UDPTunnelConnectionSettings, receiverUdpTunnelConnectionSettings);

    UDPTunnelConnection udpTunnelConnection(senderUdpTunnelConnectionSettings, receiverUdpTunnelConnectionSettings);

    QSignalSpy dataSentSpy(&udpTunnelConnection, SIGNAL(dataSent()));
    QSignalSpy dataReceivedSpy(&udpTunnelConnection, SIGNAL(receivedData(const QByteArray)));

    QByteArray receivedData;
    QObject::connect(&udpTunnelConnection, &UDPTunnelConnection::receivedData, this, [this, &receivedData](const QByteArray _receivedData){
        receivedData = _receivedData;
    });

    QCOMPARE(dataSentSpy.size(), 0);
    QCOMPARE(dataReceivedSpy.size(), 0);

    const QByteArray sentData = "abcd";
    udpTunnelConnection.sendData(sentData);

    QCOMPARE(udpTunnelConnection.udpTunnelPacketSender->sendBuffer.size(), 1);

    QTest::qWait(10);

    QCOMPARE(dataSentSpy.size(), 1);
    QCOMPARE(dataReceivedSpy.size(), 1);
    QCOMPARE(sentData, receivedData);

    QCOMPARE(udpTunnelConnection.udpTunnelPacketSender->sendBuffer.size(), 0);

    QObject::disconnect(&udpTunnelConnection, nullptr, nullptr, nullptr);
}

void test_udptunnelconnection::testDataTransferPacketLoss_data()
{
    QTest::addColumn<UDPTunnelConnectionSettings>("senderUdpTunnelConnectionSendSettings");
    QTest::addColumn<UDPTunnelConnectionSettings>("senderUdpTunnelConnectionReceiveSettings");
    QTest::addColumn<UDPTunnelConnectionSettings>("receiverUdpTunnelConnectionSendSettings");
    QTest::addColumn<UDPTunnelConnectionSettings>("receiverUdpTunnelConnectionReceiveSettings");

    const UDPTunnelConnectionSettings senderUdpTunnelConnectionSendSettings = {"127.0.0.1", 1111, "127.0.0.1", 2222};
    const UDPTunnelConnectionSettings senderUdpTunnelConnectionReceiveSettings = {"127.0.0.1", 3333, "127.0.0.1", 4444};
    const UDPTunnelConnectionSettings receiverUdpTunnelConnectionSendSettings = {"127.0.0.1", 4444, "127.0.0.1", 3333};
    const UDPTunnelConnectionSettings receiverUdpTunnelConnectionReceiveSettings = {"127.0.0.1", 2222, "127.0.0.1", 1111};

    QTest::newRow("Data transfer packet loss and timeout test using localhost and valid ports") << senderUdpTunnelConnectionSendSettings << senderUdpTunnelConnectionReceiveSettings << receiverUdpTunnelConnectionSendSettings << receiverUdpTunnelConnectionReceiveSettings;
}

void test_udptunnelconnection::testDataTransferPacketLoss()
{
    QFETCH(UDPTunnelConnectionSettings, senderUdpTunnelConnectionSendSettings);
    QFETCH(UDPTunnelConnectionSettings, senderUdpTunnelConnectionReceiveSettings);
    QFETCH(UDPTunnelConnectionSettings, receiverUdpTunnelConnectionSendSettings);
    QFETCH(UDPTunnelConnectionSettings, receiverUdpTunnelConnectionReceiveSettings);

    UDPTunnelConnection senderUdpTunnelConnection(senderUdpTunnelConnectionSendSettings, senderUdpTunnelConnectionReceiveSettings);
    UDPTunnelConnection receiverUdpTunnelConnection(receiverUdpTunnelConnectionSendSettings, receiverUdpTunnelConnectionReceiveSettings);
    bool threadActive = true;
    bool responseTimeoutOccurred = false;

    QEventLoop eventLoop;
    QObject::connect(&senderUdpTunnelConnection, &UDPTunnelConnection::responseTimeoutOccurred, this, [this, &responseTimeoutOccurred](){
        responseTimeoutOccurred = true;

        // Stop the test! Some packet was lost in localhost somewhere!
        QCOMPARE(true, false);
    });
    QObject::connect(&receiverUdpTunnelConnection, &UDPTunnelConnection::receivedData, this, [this, &threadActive, &eventLoop](const QByteArray& receivedData){
        // qDebug() << "RECEIVED:" << this->receivedPacketIds++;
        if(this->receivedPacketIds >= 1024)
        {
            threadActive = false;
            eventLoop.quit();
        }
    });

    QTimer::singleShot(0, this, [this, &senderUdpTunnelConnection](){
        qDebug() << "Generating test data";

        QByteArray data;
        for(size_t bytePos = 0; bytePos < 1024; bytePos++)
        {
            data.push_back("a");
        }

        qDebug() << "Generated test data";

        for(size_t packetId = 0; packetId <= 100; packetId++)
        {
            senderUdpTunnelConnection.sendData(data);
        }
    });

    // Give the background threads some time to exit
    QTest::qWait(1000);

    QObject::disconnect(&senderUdpTunnelConnection, &UDPTunnelConnection::responseTimeoutOccurred, nullptr, nullptr);
    QObject::disconnect(&receiverUdpTunnelConnection, &UDPTunnelConnection::receivedData, nullptr, nullptr);
}

QTEST_MAIN(test_udptunnelconnection)
#include "test_udptunnelconnection.moc"