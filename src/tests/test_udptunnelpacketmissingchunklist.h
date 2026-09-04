#ifndef TEST_UDPTUNNELPACKETMISSINGCHUNKLIST_H
#define TEST_UDPTUNNELPACKETMISSINGCHUNKLIST_H

#include <QObject>
#include <QTest>

class test_udptunnelpacketmissingchunklist : public QObject
{
    Q_OBJECT
public:
    test_udptunnelpacketmissingchunklist();
    ~test_udptunnelpacketmissingchunklist();

private slots:
    void testDefaultConstructor();
    void testConstructorFromMissingChunks_data();
    void testConstructorFromMissingChunks();
    void testConstructorFromEncodedData_data();
    void testConstructorFromEncodedData();
    void testGetMissingChunks_data();
    void testGetMissingChunks();
    void testSetMissingChunks_data();
    void testSetMissingChunks();
    void testSplitMissingChunks_data();
    void testSplitMissingChunks();

private:
    inline static const QString toString(const std::vector<size_t>& missingChunkIds);
};

#endif // TEST_UDPTUNNELPACKETMISSINGCHUNKLIST_H