#ifndef TEST_UDPTUNNELCONNECTIONSETTINGS_H
#define TEST_UDPTUNNELCONNECTIONSETTINGS_H

#include <QObject>
#include <QTest>

class test_udptunnelconnectionsettings : public QObject
{
    Q_OBJECT
public:
    test_udptunnelconnectionsettings();
    ~test_udptunnelconnectionsettings();

private slots:
    void testDefaultConstructor();
    void testConstructorFromAddressesAndPorts_data();
    void testConstructorFromAddressesAndPorts();
    void testConstructorFromEncodedData_data();
    void testConstructorFromEncodedData();
    void testGetters_data();
    void testGetters();
    void testSetters_data();
    void testSetters();
};

#endif // TEST_UDPTUNNELCONNECTIONSETTINGS_H
