#ifndef TEST_UDPTUNNELCONNECTION_H
#define TEST_UDPTUNNELCONNECTION_H

#include "../udptunnelconnectionsettings.h"
#include "../udptunnelconnection.h"
#include <QObject>
#include <QTest>

class test_udptunnelconnection : public QObject
{
    Q_OBJECT
public:
    test_udptunnelconnection();
    ~test_udptunnelconnection();

private slots:
    void testDataTransferSelfTest_data();
    void testDataTransferSelfTest();
    void testDataTransferPacketLoss_data();
    void testDataTransferPacketLoss();

private:
    size_t sentPacketIds = 0;
    size_t receivedPacketIds = 0;
};

#endif // TEST_UDPTUNNELCONNECTION_H
