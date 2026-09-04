#include "test_udptunnelconnectionsettings.h"
#include "../udptunnelconnectionsettings.h"
#include <QSignalSpy>

test_udptunnelconnectionsettings::test_udptunnelconnectionsettings()
{

}

test_udptunnelconnectionsettings::~test_udptunnelconnectionsettings()
{

}

void test_udptunnelconnectionsettings::testDefaultConstructor()
{
    UDPTunnelConnectionSettings udpTunnelConnectionSettings;

    QCOMPARE(udpTunnelConnectionSettings.ingressAddress, "0.0.0.0");
    QCOMPARE(udpTunnelConnectionSettings.ingressPort, 0);
    QCOMPARE(udpTunnelConnectionSettings.egressAddress, "127.0.0.1");
    QCOMPARE(udpTunnelConnectionSettings.egressPort, 0);
}

void test_udptunnelconnectionsettings::testConstructorFromAddressesAndPorts_data()
{
    QTest::addColumn<QByteArray>("ingressAddress");
    QTest::addColumn<uint16_t>("ingressPort");
    QTest::addColumn<QByteArray>("egressAddress");
    QTest::addColumn<uint16_t>("egressPort");

    for(uint16_t testId = 0; testId < 10; testId++)
    {
        const auto& ingressAddress = QString("%1.%2.%3.%4").arg(testId).arg(testId+1).arg(testId+2).arg(testId+3);
        const uint16_t& ingressPort = testId+4;
        const auto& egressAddress = QString("%1.%2.%3.%4").arg(testId+5).arg(testId+6).arg(testId+7).arg(testId+8);
        const uint16_t& egressPort = testId+9;

        const auto& testName = QString("Test UDPTunnelConnectionSettings from addresses and ports (%1, %2, %3, %4)").arg(ingressAddress).arg(ingressPort).arg(egressAddress).arg(egressPort);
        QTest::newRow(testName.toStdString().c_str()) << QByteArray::fromStdString(ingressAddress.toStdString()) << ingressPort << QByteArray::fromStdString(egressAddress.toStdString()) << egressPort;
    }
}

void test_udptunnelconnectionsettings::testConstructorFromAddressesAndPorts()
{
    QFETCH(QByteArray, ingressAddress);
    QFETCH(uint16_t, ingressPort);
    QFETCH(QByteArray, egressAddress);
    QFETCH(uint16_t, egressPort);

    UDPTunnelConnectionSettings udpTunnelConnectionSettings(ingressAddress, ingressPort, egressAddress, egressPort);

    QCOMPARE(udpTunnelConnectionSettings.ingressAddress, ingressAddress);
    QCOMPARE(udpTunnelConnectionSettings.ingressPort, ingressPort);
    QCOMPARE(udpTunnelConnectionSettings.egressAddress, egressAddress);
    QCOMPARE(udpTunnelConnectionSettings.egressPort, egressPort);
}

void test_udptunnelconnectionsettings::testConstructorFromEncodedData_data()
{
    QTest::addColumn<QByteArray>("ingressAddress");
    QTest::addColumn<uint16_t>("ingressPort");
    QTest::addColumn<QByteArray>("egressAddress");
    QTest::addColumn<uint16_t>("egressPort");

    for(uint16_t testId = 0; testId < 10; testId++)
    {
        const auto& ingressAddress = QString("%1.%2.%3.%4").arg(testId).arg(testId+1).arg(testId+2).arg(testId+3);
        const uint16_t& ingressPort = testId+4;
        const auto& egressAddress = QString("%1.%2.%3.%4").arg(testId+5).arg(testId+6).arg(testId+7).arg(testId+8);
        const uint16_t& egressPort = testId+9;

        const auto& testName = QString("Test UDPTunnelConnectionSettings from encoded data (%1, %2, %3, %4)").arg(ingressAddress).arg(ingressPort).arg(egressAddress).arg(egressPort);
        QTest::newRow(testName.toStdString().c_str()) << QByteArray::fromStdString(ingressAddress.toStdString()) << ingressPort << QByteArray::fromStdString(egressAddress.toStdString()) << egressPort;
    }
}

void test_udptunnelconnectionsettings::testConstructorFromEncodedData()
{
    QFETCH(QByteArray, ingressAddress);
    QFETCH(uint16_t, ingressPort);
    QFETCH(QByteArray, egressAddress);
    QFETCH(uint16_t, egressPort);

    UDPTunnelConnectionSettings udpTunnelConnectionSettings(ingressAddress, ingressPort, egressAddress, egressPort);
    const auto& encodedData = udpTunnelConnectionSettings.encode();
    UDPTunnelConnectionSettings udpTunnelConnectionSettings2(encodedData);

    QCOMPARE(udpTunnelConnectionSettings.ingressAddress, udpTunnelConnectionSettings2.ingressAddress);
    QCOMPARE(udpTunnelConnectionSettings.ingressPort, udpTunnelConnectionSettings2.ingressPort);
    QCOMPARE(udpTunnelConnectionSettings.egressAddress, udpTunnelConnectionSettings2.egressAddress);
    QCOMPARE(udpTunnelConnectionSettings.egressPort, udpTunnelConnectionSettings2.egressPort);
}

void test_udptunnelconnectionsettings::testGetters_data()
{
    QTest::addColumn<QByteArray>("ingressAddress");
    QTest::addColumn<uint16_t>("ingressPort");
    QTest::addColumn<QByteArray>("egressAddress");
    QTest::addColumn<uint16_t>("egressPort");

    for(uint16_t testId = 0; testId < 10; testId++)
    {
        const auto& ingressAddress = QString("%1.%2.%3.%4").arg(testId).arg(testId+1).arg(testId+2).arg(testId+3);
        const uint16_t& ingressPort = testId+4;
        const auto& egressAddress = QString("%1.%2.%3.%4").arg(testId+5).arg(testId+6).arg(testId+7).arg(testId+8);
        const uint16_t& egressPort = testId+9;

        const auto& testName = QString("Test UDPTunnelConnectionSettings getters (%1, %2, %3, %4)").arg(ingressAddress).arg(ingressPort).arg(egressAddress).arg(egressPort);
        QTest::newRow(testName.toStdString().c_str()) << QByteArray::fromStdString(ingressAddress.toStdString()) << ingressPort << QByteArray::fromStdString(egressAddress.toStdString()) << egressPort;
    }
}

void test_udptunnelconnectionsettings::testGetters()
{
    QFETCH(QByteArray, ingressAddress);
    QFETCH(uint16_t, ingressPort);
    QFETCH(QByteArray, egressAddress);
    QFETCH(uint16_t, egressPort);

    UDPTunnelConnectionSettings udpTunnelConnectionSettings;

    udpTunnelConnectionSettings.ingressAddress = ingressAddress;
    udpTunnelConnectionSettings.ingressPort = ingressPort;
    udpTunnelConnectionSettings.egressAddress = egressAddress;
    udpTunnelConnectionSettings.egressPort = egressPort;

    QCOMPARE(udpTunnelConnectionSettings.getIngressAddress(), udpTunnelConnectionSettings.ingressAddress);
    QCOMPARE(udpTunnelConnectionSettings.getIngressPort(), udpTunnelConnectionSettings.ingressPort);
    QCOMPARE(udpTunnelConnectionSettings.getEgressAddress(), udpTunnelConnectionSettings.egressAddress);
    QCOMPARE(udpTunnelConnectionSettings.getEgressPort(), udpTunnelConnectionSettings.egressPort);
}

void test_udptunnelconnectionsettings::testSetters_data()
{
    QTest::addColumn<QByteArray>("ingressAddress");
    QTest::addColumn<uint16_t>("ingressPort");
    QTest::addColumn<QByteArray>("egressAddress");
    QTest::addColumn<uint16_t>("egressPort");

    for(uint16_t testId = 0; testId < 10; testId++)
    {
        const auto& ingressAddress = QString("%1.%2.%3.%4").arg(testId).arg(testId+1).arg(testId+2).arg(testId+3);
        const uint16_t& ingressPort = testId+4;
        const auto& egressAddress = QString("%1.%2.%3.%4").arg(testId+5).arg(testId+6).arg(testId+7).arg(testId+8);
        const uint16_t& egressPort = testId+9;

        const auto& testName = QString("Test UDPTunnelConnectionSettings setters (%1, %2, %3, %4)").arg(ingressAddress).arg(ingressPort).arg(egressAddress).arg(egressPort);
        QTest::newRow(testName.toStdString().c_str()) << QByteArray::fromStdString(ingressAddress.toStdString()) << ingressPort << QByteArray::fromStdString(egressAddress.toStdString()) << egressPort;
    }
}

void test_udptunnelconnectionsettings::testSetters()
{
    QFETCH(QByteArray, ingressAddress);
    QFETCH(uint16_t, ingressPort);
    QFETCH(QByteArray, egressAddress);
    QFETCH(uint16_t, egressPort);

    UDPTunnelConnectionSettings udpTunnelConnectionSettings;

    udpTunnelConnectionSettings.setIngressAddress(ingressAddress);
    udpTunnelConnectionSettings.setIngressPort(ingressPort);
    udpTunnelConnectionSettings.setEgressAddress(egressAddress);
    udpTunnelConnectionSettings.setEgressPort(egressPort);

    QCOMPARE(udpTunnelConnectionSettings.getIngressAddress(), ingressAddress);
    QCOMPARE(udpTunnelConnectionSettings.getIngressPort(), ingressPort);
    QCOMPARE(udpTunnelConnectionSettings.getEgressAddress(), egressAddress);
    QCOMPARE(udpTunnelConnectionSettings.getEgressPort(), egressPort);
}

QTEST_MAIN(test_udptunnelconnectionsettings)
#include "test_udptunnelconnectionsettings.moc"