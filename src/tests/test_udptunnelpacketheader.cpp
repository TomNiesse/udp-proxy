#include "test_udptunnelpacketheader.h"
#include "../udptunnelpacketheader.h"
#include <QSignalSpy>

test_udptunnelpacketheader::test_udptunnelpacketheader()
{

}

test_udptunnelpacketheader::~test_udptunnelpacketheader()
{

}

// Private slots

void test_udptunnelpacketheader::testGetPacketType_data()
{
    QTest::addColumn<UDPTunnelPacketType>("udpTunnelPacketType");

    const std::vector<UDPTunnelPacketType>& udpTunnelPacketTypes = {
        INVALID,
        UDP_ANNOUNCEMENT,
        UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT,
        UDP_DATA,
        UDP_DATA_ACKNOWLEDGEMENT,
        UDP_DATA_RECEIVED_REQUEST,
        UDP_DATA_RECEIVED_RESPONSE,
        UDP_DATA_FLUSH,
        UDP_DATA_FLUSH_ACKNOWLEDGEMENT
    };

    for(const auto& udpTunnelPacketType : udpTunnelPacketTypes)
    {
        const auto& testName = QString("GetPacketType (%1)").arg(udpTunnelPacketType);
        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketType;
    }
}

void test_udptunnelpacketheader::testGetPacketType()
{
    QFETCH(UDPTunnelPacketType, udpTunnelPacketType);

    UDPTunnelPacketHeader header;
    header.packetType = udpTunnelPacketType;

    QCOMPARE(header.getPacketType(), udpTunnelPacketType);
}

void test_udptunnelpacketheader::testSetPacketType_data()
{
    QTest::addColumn<UDPTunnelPacketType>("udpTunnelPacketType");

    const std::vector<UDPTunnelPacketType>& udpTunnelPacketTypes = {
        INVALID,
        UDP_ANNOUNCEMENT,
        UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT,
        UDP_DATA,
        UDP_DATA_ACKNOWLEDGEMENT,
        UDP_DATA_RECEIVED_REQUEST,
        UDP_DATA_RECEIVED_RESPONSE,
        UDP_DATA_FLUSH,
        UDP_DATA_FLUSH_ACKNOWLEDGEMENT
    };

    for(const auto& udpTunnelPacketType : udpTunnelPacketTypes)
    {
        const auto& testName = QString("SetPacketType (%1)").arg(udpTunnelPacketType);
        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketType;
    }
}

void test_udptunnelpacketheader::testSetPacketType()
{
    QFETCH(UDPTunnelPacketType, udpTunnelPacketType);

    UDPTunnelPacketHeader header;
    header.setPacketType(udpTunnelPacketType);

    QCOMPARE(header.getPacketType(), udpTunnelPacketType);
}

void test_udptunnelpacketheader::testGetPacketId_data()
{
    QTest::addColumn<size_t>("packetId");

    for(size_t packetId = 0; packetId <= 100; packetId+=1)
    {
        const auto& testName = QString("GetPacketId (%1)").arg(packetId);
        QTest::newRow(testName.toStdString().c_str()) << packetId;
    }
}

void test_udptunnelpacketheader::testGetPacketId()
{
    QFETCH(size_t, packetId);

    UDPTunnelPacketHeader header;
    header.packetId = packetId;

    QCOMPARE(header.getPacketId(), packetId);
}

void test_udptunnelpacketheader::testSetPacketId_data()
{
    QTest::addColumn<size_t>("packetId");

    for(size_t packetId = 0; packetId <= 100; packetId+=1)
    {
        const auto& testName = QString("SetPacketId (%1)").arg(packetId);
        QTest::newRow(testName.toStdString().c_str()) << packetId;
    }
}

void test_udptunnelpacketheader::testSetPacketId()
{
    QFETCH(size_t, packetId);

    UDPTunnelPacketHeader header;
    header.setPacketId(packetId);

    QCOMPARE(header.getPacketId(), packetId);
}

void test_udptunnelpacketheader::testGetChunkId_data()
{
    QTest::addColumn<size_t>("chunkId");

    for(size_t chunkId = 0; chunkId <= 100; chunkId+=1)
    {
        const auto& testName = QString("GetChunkId (%1)").arg(chunkId);
        QTest::newRow(testName.toStdString().c_str()) << chunkId;
    }
}

void test_udptunnelpacketheader::testGetChunkId()
{
    QFETCH(size_t, chunkId);

    UDPTunnelPacketHeader header;
    header.chunkId = chunkId;

    QCOMPARE(header.getChunkId(), chunkId);
}

void test_udptunnelpacketheader::testSetChunkId_data()
{
    QTest::addColumn<size_t>("chunkId");

    for(size_t chunkId = 0; chunkId <= 100; chunkId+=1)
    {
        const auto& testName = QString("SetChunkId (%1)").arg(chunkId);
        QTest::newRow(testName.toStdString().c_str()) << chunkId;
    }
}

void test_udptunnelpacketheader::testSetChunkId()
{
    QFETCH(size_t, chunkId);

    UDPTunnelPacketHeader header;
    header.setChunkId(chunkId);

    QCOMPARE(header.getChunkId(), chunkId);
}

void test_udptunnelpacketheader::testGetChunkIdStart_data()
{
    QTest::addColumn<size_t>("chunkIdStart");

    for(size_t chunkIdStart = 0; chunkIdStart <= 100; chunkIdStart+=1)
    {
        const auto& testName = QString("GetChunkIdStart (%1)").arg(chunkIdStart);
        QTest::newRow(testName.toStdString().c_str()) << chunkIdStart;
    }
}

void test_udptunnelpacketheader::testGetChunkIdStart()
{
    QFETCH(size_t, chunkIdStart);

    UDPTunnelPacketHeader header;
    header.chunkIdStart = chunkIdStart;

    QCOMPARE(header.getChunkIdStart(), chunkIdStart);
}

void test_udptunnelpacketheader::testSetChunkIdStart_data()
{
    QTest::addColumn<size_t>("chunkIdStart");

    for(size_t chunkIdStart = 0; chunkIdStart <= 100; chunkIdStart+=1)
    {
        const auto& testName = QString("GetChunkIdStart (%1)").arg(chunkIdStart);
        QTest::newRow(testName.toStdString().c_str()) << chunkIdStart;
    }
}

void test_udptunnelpacketheader::testSetChunkIdStart()
{
    QFETCH(size_t, chunkIdStart);

    UDPTunnelPacketHeader header;
    header.setChunkIdStart(chunkIdStart);

    QCOMPARE(header.getChunkIdStart(), chunkIdStart);
}

void test_udptunnelpacketheader::testGetChunkIdEnd_data()
{
    QTest::addColumn<size_t>("chunkIdEnd");

    for(size_t chunkIdEnd = 0; chunkIdEnd <= 100; chunkIdEnd+=1)
    {
        const auto& testName = QString("GetChunkIdEnd (%1)").arg(chunkIdEnd);
        QTest::newRow(testName.toStdString().c_str()) << chunkIdEnd;
    }
}

void test_udptunnelpacketheader::testGetChunkIdEnd()
{
    QFETCH(size_t, chunkIdEnd);

    UDPTunnelPacketHeader header;
    header.chunkIdEnd = chunkIdEnd;

    QCOMPARE(header.getChunkIdEnd(), chunkIdEnd);
}

void test_udptunnelpacketheader::testSetChunkIdEnd_data()
{
    QTest::addColumn<size_t>("chunkIdEnd");

    for(size_t chunkIdEnd = 0; chunkIdEnd <= 100; chunkIdEnd+=1)
    {
        const auto& testName = QString("GetChunkIdEnd (%1)").arg(chunkIdEnd);
        QTest::newRow(testName.toStdString().c_str()) << chunkIdEnd;
    }
}

void test_udptunnelpacketheader::testSetChunkIdEnd()
{
    QFETCH(size_t, chunkIdEnd);

    UDPTunnelPacketHeader header;
    header.setChunkIdEnd(chunkIdEnd);

    QCOMPARE(header.getChunkIdEnd(), chunkIdEnd);
}

void test_udptunnelpacketheader::testEncodeDecode_data()
{
    QTest::addColumn<UDPTunnelPacketType>("udpTunnelPacketType");

    const std::vector<UDPTunnelPacketType>& udpTunnelPacketTypes = {
        INVALID,
        UDP_ANNOUNCEMENT,
        UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT,
        UDP_DATA,
        UDP_DATA_ACKNOWLEDGEMENT,
        UDP_DATA_RECEIVED_REQUEST,
        UDP_DATA_RECEIVED_RESPONSE,
        UDP_DATA_FLUSH,
        UDP_DATA_FLUSH_ACKNOWLEDGEMENT
    };

    for(const auto& udpTunnelPacketType : udpTunnelPacketTypes)
    {
        const auto& testName = QString("Encode/Decode (%1)").arg(static_cast<size_t>(udpTunnelPacketType));
        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketType;
    }
}

void test_udptunnelpacketheader::testEncodeDecode()
{
    QFETCH(UDPTunnelPacketType, udpTunnelPacketType);

    for(size_t testId = 0; testId < 100; testId++)
    {
        UDPTunnelPacketHeader header;
        header.setPacketType(udpTunnelPacketType);
        header.setPacketId(testId);
        header.setChunkId(testId+1);
        header.setChunkIdStart(testId+2);
        header.setChunkIdEnd(testId+3);

        const auto& encodedHeader = header.encode();

        if(udpTunnelPacketType == UDPTunnelPacketType::INVALID)
        {
            QCOMPARE(encodedHeader.size(), 0);
            return;
        }

        UDPTunnelPacketHeader header2(encodedHeader);

        // The encode function doesn't always encode all the header bytes for every type of request.
        // The unit test only checks for decoded bytes that are relevant to the request.
        switch(udpTunnelPacketType)
        {
        case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkIdStart(), header2.getChunkIdStart());
            QCOMPARE(header.getChunkIdEnd(), header2.getChunkIdEnd());
            break;
        case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkIdStart(), header2.getChunkIdStart());
            QCOMPARE(header.getChunkIdEnd(), header2.getChunkIdEnd());
            break;
        case UDPTunnelPacketType::UDP_DATA:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkId(), header2.getChunkId());
            break;
        case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkId(), header2.getChunkId());
            break;
        case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            break;
        case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            break;
        case UDPTunnelPacketType::UDP_DATA_FLUSH:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkId(), header2.getChunkId());
            break;
        case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
            QCOMPARE(header.getPacketType(), header2.getPacketType());
            QCOMPARE(header.getPacketId(), header2.getPacketId());
            QCOMPARE(header.getChunkId(), header2.getChunkId());
            break;
        }
    }
}

void test_udptunnelpacketheader::testGetHeaderSize_data()
{
    QTest::addColumn<UDPTunnelPacketType>("udpTunnelPacketType");

    const std::vector<UDPTunnelPacketType>& udpTunnelPacketTypes = {
        INVALID,
        UDP_ANNOUNCEMENT,
        UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT,
        UDP_DATA,
        UDP_DATA_ACKNOWLEDGEMENT,
        UDP_DATA_RECEIVED_REQUEST,
        UDP_DATA_RECEIVED_RESPONSE,
        UDP_DATA_FLUSH,
        UDP_DATA_FLUSH_ACKNOWLEDGEMENT
    };

    for(const auto& udpTunnelPacketType : udpTunnelPacketTypes)
    {
        const auto& testName = QString("GetHeaderSize matches encoded data size (%1)").arg(static_cast<size_t>(udpTunnelPacketType));
        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketType;
    }
}

void test_udptunnelpacketheader::testGetHeaderSize()
{
    QFETCH(UDPTunnelPacketType, udpTunnelPacketType);

    UDPTunnelPacketHeader header;
    header.setPacketType(udpTunnelPacketType);
    const auto& encodedHeader = header.encode();
    const auto& headerSize = header.getHeaderSize();

    QCOMPARE(encodedHeader.size(), headerSize);
}

QTEST_MAIN(test_udptunnelpacketheader)
#include "test_udptunnelpacketheader.moc"