#include "test_udptunnelpacketmissingchunklist.h"
#include "../udptunnelpacketmissingchunklist.h"
#include <QSignalSpy>

test_udptunnelpacketmissingchunklist::test_udptunnelpacketmissingchunklist()
{

}

test_udptunnelpacketmissingchunklist::~test_udptunnelpacketmissingchunklist()
{

}

void test_udptunnelpacketmissingchunklist::testDefaultConstructor()
{
    UDPTunnelPacketMissingChunkList missingChunkList;
    QCOMPARE(missingChunkList.missingChunks.size(), 0);
}

void test_udptunnelpacketmissingchunklist::testConstructorFromMissingChunks_data()
{
    QTest::addColumn<std::vector<size_t>>("missingChunks");

    const auto& numberOfIterations = 4;

    for(size_t a = 0; a < numberOfIterations; a++)
    {
        for(size_t b = 0; b < numberOfIterations; b++)
        {
            for(size_t c = 0; c < numberOfIterations; c++)
            {
                for(size_t d = 0; d < numberOfIterations; d++)
                {
                    const std::vector<size_t>& missingChunks = {a, b, c, d};

                    const auto& testName = QString("Constructor from missing chunks (%1)").arg(toString(missingChunks));
                    QTest::newRow(testName.toStdString().c_str()) << missingChunks;
                }
            }
        }
    }
}

void test_udptunnelpacketmissingchunklist::testConstructorFromMissingChunks()
{
    QFETCH(std::vector<size_t>, missingChunks);

    UDPTunnelPacketMissingChunkList missingChunkList(missingChunks);
    QCOMPARE(missingChunkList.missingChunks.size(), missingChunks.size());
    QCOMPARE(missingChunkList.missingChunks, missingChunks);
}

void test_udptunnelpacketmissingchunklist::testConstructorFromEncodedData_data()
{
    QTest::addColumn<std::vector<size_t>>("missingChunks");

    const auto& numberOfIterations = 4;

    for(size_t a = 0; a < numberOfIterations; a++)
    {
        for(size_t b = 0; b < numberOfIterations; b++)
        {
            for(size_t c = 0; c < numberOfIterations; c++)
            {
                for(size_t d = 0; d < numberOfIterations; d++)
                {
                    const std::vector<size_t>& missingChunks = {a, b, c, d};

                    const auto& testName = QString("Encode/decode missing chunks (%1)").arg(toString(missingChunks));
                    QTest::newRow(testName.toStdString().c_str()) << missingChunks;
                }
            }
        }
    }
}

void test_udptunnelpacketmissingchunklist::testConstructorFromEncodedData()
{
    QFETCH(std::vector<size_t>, missingChunks);

    UDPTunnelPacketMissingChunkList missingChunkList(missingChunks);
    QCOMPARE(missingChunkList.missingChunks.size(), missingChunks.size());
    QCOMPARE(missingChunkList.missingChunks, missingChunks);

    // Test encode() function that encodes a single missing chunks list
    const auto& encodedData = missingChunkList.encode(missingChunkList.missingChunks);
    UDPTunnelPacketMissingChunkList missingChunkList2(encodedData);
    QCOMPARE(missingChunkList2.missingChunks.size(), missingChunkList.missingChunks.size());
    QCOMPARE(missingChunkList2.missingChunks, missingChunkList.missingChunks);

    // Test encode() function that encodes a list of missing chunks lists
    const auto& encodedData2 = missingChunkList.encode(std::vector<std::vector<size_t>>({missingChunkList.missingChunks}));
    QCOMPARE(encodedData2.size(), 1);
    UDPTunnelPacketMissingChunkList missingChunkList3(encodedData2.at(0));
    QCOMPARE(missingChunkList3.missingChunks.size(), missingChunkList.missingChunks.size());
    QCOMPARE(missingChunkList3.missingChunks, missingChunkList.missingChunks);
}

void test_udptunnelpacketmissingchunklist::testGetMissingChunks_data()
{
    QTest::addColumn<std::vector<size_t>>("missingChunks");

    const auto& numberOfIterations = 4;

    for(size_t a = 0; a < numberOfIterations; a++)
    {
        for(size_t b = 0; b < numberOfIterations; b++)
        {
            for(size_t c = 0; c < numberOfIterations; c++)
            {
                for(size_t d = 0; d < numberOfIterations; d++)
                {
                    const std::vector<size_t>& missingChunks = {a, b, c, d};

                    const auto& testName = QString("Get missing chunks (%1)").arg(toString(missingChunks));
                    QTest::newRow(testName.toStdString().c_str()) << missingChunks;
                }
            }
        }
    }
}

void test_udptunnelpacketmissingchunklist::testGetMissingChunks()
{
    QFETCH(std::vector<size_t>, missingChunks);

    UDPTunnelPacketMissingChunkList missingChunkList;
    missingChunkList.missingChunks = missingChunks;
    QCOMPARE(missingChunkList.getMissingChunks().size(), missingChunks.size());
    QCOMPARE(missingChunkList.getMissingChunks(), missingChunks);
}

void test_udptunnelpacketmissingchunklist::testSetMissingChunks_data()
{
    QTest::addColumn<std::vector<size_t>>("missingChunks");

    const auto& numberOfIterations = 4;

    for(size_t a = 0; a < numberOfIterations; a++)
    {
        for(size_t b = 0; b < numberOfIterations; b++)
        {
            for(size_t c = 0; c < numberOfIterations; c++)
            {
                for(size_t d = 0; d < numberOfIterations; d++)
                {
                    const std::vector<size_t>& missingChunks = {a, b, c, d};

                    const auto& testName = QString("Set missing chunks (%1)").arg(toString(missingChunks));
                    QTest::newRow(testName.toStdString().c_str()) << missingChunks;
                }
            }
        }
    }
}

void test_udptunnelpacketmissingchunklist::testSetMissingChunks()
{
    QFETCH(std::vector<size_t>, missingChunks);

    UDPTunnelPacketMissingChunkList missingChunkList;
    QCOMPARE(missingChunkList.getMissingChunks().size(), 0);
    missingChunkList.setMissingChunks(missingChunks);
    QCOMPARE(missingChunkList.getMissingChunks().size(), missingChunks.size());
    QCOMPARE(missingChunkList.getMissingChunks(), missingChunks);
}

void test_udptunnelpacketmissingchunklist::testSplitMissingChunks_data()
{
    QTest::addColumn<std::vector<size_t>>("missingChunks");

    const auto& numberOfIterations = 4;

    for(size_t a = 0; a < numberOfIterations; a++)
    {
        for(size_t b = 0; b < numberOfIterations; b++)
        {
            for(size_t c = 0; c < numberOfIterations; c++)
            {
                for(size_t d = 0; d < numberOfIterations; d++)
                {
                    const std::vector<size_t>& missingChunks = {a, b, c, d};

                    const auto& testName = QString("Split missing chunks (%1)").arg(toString(missingChunks));
                    QTest::newRow(testName.toStdString().c_str()) << missingChunks;
                }
            }
        }
    }
}

void test_udptunnelpacketmissingchunklist::testSplitMissingChunks()
{
    QFETCH(std::vector<size_t>, missingChunks);

    const auto& splitMissingChunksLists = UDPTunnelPacketMissingChunkList::split(missingChunks, 2);

    QCOMPARE(splitMissingChunksLists.size(), 2);
    QCOMPARE(splitMissingChunksLists.at(0), std::vector<size_t>({missingChunks.at(0), missingChunks.at(1)}));
    QCOMPARE(splitMissingChunksLists.at(1), std::vector<size_t>({missingChunks.at(2), missingChunks.at(3)}));
}

// Private

const QString test_udptunnelpacketmissingchunklist::toString(const std::vector<size_t>& missingChunkIds)
{
    QString out;

    out += "{";
    for(const auto& missingChunkId : missingChunkIds)
    {
        out += QString("%1").arg(missingChunkId);
        if(&missingChunkId != &missingChunkIds.back())
        {
            out += ", ";
        }
    }
    out += "}";

    return out;
}

QTEST_MAIN(test_udptunnelpacketmissingchunklist)
#include "test_udptunnelpacketmissingchunklist.moc"