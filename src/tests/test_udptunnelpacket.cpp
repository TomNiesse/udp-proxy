#include "test_udptunnelpacket.h"
#include "../udptunnelpacket.h"
#include "../udptunnelpacketmissingchunklist.h"
#include <QSignalSpy>

test_udptunnelpacket::test_udptunnelpacket()
{

}

test_udptunnelpacket::~test_udptunnelpacket()
{

}

void test_udptunnelpacket::testConstructors_data()
{
    QTest::addColumn<size_t>("packetId");
    QTest::addColumn<size_t>("chunkId");
    QTest::addColumn<size_t>("chunkIdStart");
    QTest::addColumn<size_t>("chunkIdEnd");
    QTest::addColumn<QByteArray>("payload");
    QTest::addColumn<std::vector<size_t>>("missingChunkIds");

    const std::vector<size_t>& packetIds = {0, 1, 2, 3};
    const std::vector<size_t>& chunkIds = {0, 1, 2, 3};
    const std::vector<size_t>& chunkIdStarts = {0, 1, 2, 3};
    const std::vector<size_t>& chunkIdEnds = {0, 1, 2, 3};
    const std::vector<QByteArray>& payloads = {"0", "1", "2", "3"};
    const std::vector<std::vector<size_t>>& missingChunkIdsList = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {0}};

    for(const auto& packetId : packetIds)
    {
        for(const auto& chunkId : chunkIds)
        {
            for(const auto& chunkIdStart : chunkIdStarts)
            {
                for(const auto& chunkIdEnd : chunkIdEnds)
                {
                    for(const auto& payload : payloads)
                    {
                        for(const auto& missingChunkIds : missingChunkIdsList)
                        {
                            const auto& testName = QString("Test static UDPTunnelPacket constructor (%1, %2, %3, %4, %5, %6)").arg(packetId).arg(chunkId).arg(chunkIdStart).arg(chunkIdEnd).arg(payload).arg(toString(missingChunkIds));
                            QTest::newRow(testName.toStdString().c_str()) << packetId << chunkId << chunkIdStart << chunkIdEnd << payload << missingChunkIds;
                        }
                    }
                }
            }
        }
    }
}

void test_udptunnelpacket::testConstructors()
{
    QFETCH(size_t, packetId);
    QFETCH(size_t, chunkId);
    QFETCH(size_t, chunkIdStart);
    QFETCH(size_t, chunkIdEnd);
    QFETCH(QByteArray, payload);
    QFETCH(std::vector<size_t>, missingChunkIds);

    const auto& announcementPacket = UDPTunnelPacket::createAnnouncementPacket(packetId, chunkIdStart, chunkIdEnd);
    QCOMPARE(announcementPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(announcementPacket.getHeader().getChunkId(), 0);
    QCOMPARE(announcementPacket.getHeader().getChunkIdStart(), chunkIdStart);
    QCOMPARE(announcementPacket.getHeader().getChunkIdEnd(), chunkIdEnd);
    QCOMPARE(announcementPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_ANNOUNCEMENT);
    QCOMPARE(announcementPacket.getPayload(), "");

    const auto& announcementAcknowledgementPacket = UDPTunnelPacket::createAnnouncementAcknowledgementPacket(packetId, chunkIdStart, chunkIdEnd);
    QCOMPARE(announcementAcknowledgementPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(announcementAcknowledgementPacket.getHeader().getChunkId(), 0);
    QCOMPARE(announcementAcknowledgementPacket.getHeader().getChunkIdStart(), chunkIdStart);
    QCOMPARE(announcementAcknowledgementPacket.getHeader().getChunkIdEnd(), chunkIdEnd);
    QCOMPARE(announcementAcknowledgementPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT);
    QCOMPARE(announcementAcknowledgementPacket.getPayload(), "");

    const auto& udpDataPacket = UDPTunnelPacket::createUdpDataPacket(packetId, chunkId, payload);
    QCOMPARE(udpDataPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(udpDataPacket.getHeader().getChunkId(), chunkId);
    QCOMPARE(udpDataPacket.getHeader().getChunkIdStart(), 0);
    QCOMPARE(udpDataPacket.getHeader().getChunkIdEnd(), 0);
    QCOMPARE(udpDataPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA);
    QCOMPARE(udpDataPacket.getPayload(), payload);

    const auto& udpDataAcknowledgementPacket = UDPTunnelPacket::createUdpDataAcknowledgementPacket(packetId, chunkId);
    QCOMPARE(udpDataAcknowledgementPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(udpDataAcknowledgementPacket.getHeader().getChunkId(), chunkId);
    QCOMPARE(udpDataAcknowledgementPacket.getHeader().getChunkIdStart(), 0);
    QCOMPARE(udpDataAcknowledgementPacket.getHeader().getChunkIdEnd(), 0);
    QCOMPARE(udpDataAcknowledgementPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT);
    QCOMPARE(udpDataAcknowledgementPacket.getPayload(), "");

    const auto& udpDataReceivedRequestPacket = UDPTunnelPacket::createUdpDataReceivedRequestPacket(packetId);
    QCOMPARE(udpDataReceivedRequestPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(udpDataReceivedRequestPacket.getHeader().getChunkId(), 0);
    QCOMPARE(udpDataReceivedRequestPacket.getHeader().getChunkIdStart(), 0);
    QCOMPARE(udpDataReceivedRequestPacket.getHeader().getChunkIdEnd(), 0);
    QCOMPARE(udpDataReceivedRequestPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST);
    QCOMPARE(udpDataReceivedRequestPacket.getPayload(), "");

    const auto& udpDataReceivedResponsePackets = UDPTunnelPacket::createUdpDataReceivedResponsePackets(packetId, chunkIdStart, chunkIdEnd, missingChunkIds);
    for(const auto& udpDataReceivedResponsePacket : udpDataReceivedResponsePackets)
    {
        // Create a header here, so the encoded header size can be calculated in advance
        UDPTunnelPacketHeader udpTunnelPacketHeader;
        const auto& packetType = UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE;
        udpTunnelPacketHeader.setPacketType(packetType);

        // Split chunks based on the calculated header size
        const auto& headerSize = udpTunnelPacketHeader.getHeaderSize();
        const auto& splitMissingChunksLists = UDPTunnelPacketMissingChunkList::split(missingChunkIds, headerSize);
        const auto& encodedSplitMissingChunksLists = UDPTunnelPacketMissingChunkList::encode(splitMissingChunksLists);

        QCOMPARE(udpDataReceivedResponsePacket.getHeader().getPacketId(), packetId);
        QCOMPARE(udpDataReceivedResponsePacket.getHeader().getChunkId(), 0);
        QCOMPARE(udpDataReceivedResponsePacket.getHeader().getChunkIdStart(), chunkIdStart);
        QCOMPARE(udpDataReceivedResponsePacket.getHeader().getChunkIdEnd(), chunkIdEnd);
        QCOMPARE(udpDataReceivedResponsePacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE);

        // Make sure all the missing chunk ID's are converted into packages
        const bool& missingChunksFound = std::any_of(encodedSplitMissingChunksLists.begin(), encodedSplitMissingChunksLists.end(), [udpDataReceivedResponsePacket](const auto& encodedSplitMissingChunks){
            return udpDataReceivedResponsePacket.getPayload() == encodedSplitMissingChunks;
        });
        QVERIFY(missingChunksFound);
    }

    const auto& udpDataFlushPacket = UDPTunnelPacket::createUdpDataFlushPacket(packetId);
    QCOMPARE(udpDataFlushPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(udpDataFlushPacket.getHeader().getChunkId(), 0);
    QCOMPARE(udpDataFlushPacket.getHeader().getChunkIdStart(), 0);
    QCOMPARE(udpDataFlushPacket.getHeader().getChunkIdEnd(), 0);
    QCOMPARE(udpDataFlushPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA_FLUSH);
    QCOMPARE(udpDataFlushPacket.getPayload(), "");

    const auto& udpDataFlushAcknowledgementPacket = UDPTunnelPacket::createUdpDataFlushAcknowledgementPacket(packetId);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getHeader().getPacketId(), packetId);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getHeader().getChunkId(), 0);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getHeader().getChunkIdStart(), 0);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getHeader().getChunkIdEnd(), 0);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getHeader().getPacketType(), UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT);
    QCOMPARE(udpDataFlushAcknowledgementPacket.getPayload(), "");
}

void test_udptunnelpacket::testGetHeader_data()
{
    QTest::addColumn<UDPTunnelPacketHeader>("udpTunnelPacketHeader");

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
        for(size_t packetId = 0; packetId < 2; packetId++)
        {
            for(size_t chunkId = 0; chunkId < 2; chunkId++)
            {
                for(size_t chunkIdStart = 0; chunkIdStart < 2; chunkIdStart++)
                {
                    for(size_t chunkIdEnd = 0; chunkIdEnd < 2; chunkIdEnd++)
                    {
                        UDPTunnelPacketHeader udpTunnelPacketHeader;
                        udpTunnelPacketHeader.setPacketType(udpTunnelPacketType);
                        udpTunnelPacketHeader.setPacketId(packetId+1);
                        udpTunnelPacketHeader.setChunkId(chunkId+2);
                        udpTunnelPacketHeader.setChunkIdStart(chunkIdStart+3);
                        udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd+4);

                        const auto& testName = QString("Test UDPTunnelPacket header getter (%1)").arg(udpTunnelPacketHeader.toString());
                        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketHeader;
                    }
                }
            }
        }
    }
}

void test_udptunnelpacket::testGetHeader()
{
    QFETCH(UDPTunnelPacketHeader, udpTunnelPacketHeader);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.header = udpTunnelPacketHeader;

    QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacketHeader.getPacketType());
    QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacketHeader.getPacketId());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacketHeader.getChunkId());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkIdStart(), udpTunnelPacketHeader.getChunkIdStart());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkIdEnd(), udpTunnelPacketHeader.getChunkIdEnd());
}

void test_udptunnelpacket::testSetHeader_data()
{
    QTest::addColumn<UDPTunnelPacketHeader>("udpTunnelPacketHeader");

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
        for(size_t packetId = 0; packetId < 2; packetId++)
        {
            for(size_t chunkId = 0; chunkId < 2; chunkId++)
            {
                for(size_t chunkIdStart = 0; chunkIdStart < 2; chunkIdStart++)
                {
                    for(size_t chunkIdEnd = 0; chunkIdEnd < 2; chunkIdEnd++)
                    {
                        UDPTunnelPacketHeader udpTunnelPacketHeader;
                        udpTunnelPacketHeader.setPacketType(udpTunnelPacketType);
                        udpTunnelPacketHeader.setPacketId(packetId+1);
                        udpTunnelPacketHeader.setChunkId(chunkId+2);
                        udpTunnelPacketHeader.setChunkIdStart(chunkIdStart+3);
                        udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd+4);

                        const auto& testName = QString("Test UDPTunnelPacket header setter (%1)").arg(udpTunnelPacketHeader.toString());
                        QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketHeader;
                    }
                }
            }
        }
    }
}

void test_udptunnelpacket::testSetHeader()
{
    QFETCH(UDPTunnelPacketHeader, udpTunnelPacketHeader);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);

    QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacketHeader.getPacketType());
    QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacketHeader.getPacketId());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacketHeader.getChunkId());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkIdStart(), udpTunnelPacketHeader.getChunkIdStart());
    QCOMPARE(udpTunnelPacket.getHeader().getChunkIdEnd(), udpTunnelPacketHeader.getChunkIdEnd());
}

void test_udptunnelpacket::testGetPayload_data()
{
    QTest::addColumn<QByteArray>("udpTunnelPacketPayload");

    const std::vector<QByteArray>& payloads = {
        "abcd"
    };

    for(const auto& payload : payloads)
    {
        const auto& testName = QString("Test UDPTunnelPacket payload getter (payload: '%1')").arg(payload);
        QTest::newRow(testName.toStdString().c_str()) << payload;
    }
}

void test_udptunnelpacket::testGetPayload()
{
    QFETCH(QByteArray, udpTunnelPacketPayload);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.payload = udpTunnelPacketPayload;

    QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacketPayload);
}

void test_udptunnelpacket::testSetPayload_data()
{
    QTest::addColumn<QByteArray>("udpTunnelPacketPayload");

    const std::vector<QByteArray>& payloads = {
        "abcd"
    };

    for(const auto& payload : payloads)
    {
        const auto& testName = QString("Test UDPTunnelPacket payload setter (payload: '%1')").arg(payload);
        QTest::newRow(testName.toStdString().c_str()) << payload;
    }
}

void test_udptunnelpacket::testSetPayload()
{
    QFETCH(QByteArray, udpTunnelPacketPayload);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setPayload(udpTunnelPacketPayload);

    QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacketPayload);
}

void test_udptunnelpacket::testEncodeDecode_data()
{
    QTest::addColumn<UDPTunnelPacketHeader>("udpTunnelPacketHeader");
    QTest::addColumn<QByteArray>("udpTunnelPacketPayload");

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

    const std::vector<QByteArray>& payloads = {
        "abcd"
    };

    for(const auto& udpTunnelPacketType : udpTunnelPacketTypes)
    {
        for(size_t packetId = 0; packetId < 2; packetId++)
        {
            for(size_t chunkId = 0; chunkId < 2; chunkId++)
            {
                for(size_t chunkIdStart = 0; chunkIdStart < 2; chunkIdStart++)
                {
                    for(size_t chunkIdEnd = 0; chunkIdEnd < 2; chunkIdEnd++)
                    {
                        for(const auto& payload : payloads)
                        {
                            UDPTunnelPacketHeader udpTunnelPacketHeader;
                            udpTunnelPacketHeader.setPacketType(udpTunnelPacketType);
                            udpTunnelPacketHeader.setPacketId(packetId+1);
                            udpTunnelPacketHeader.setChunkId(chunkId+2);
                            udpTunnelPacketHeader.setChunkIdStart(chunkIdStart+3);
                            udpTunnelPacketHeader.setChunkIdEnd(chunkIdEnd+4);

                            const auto& testName = QString("Test UDPTunnelPacket header setter (%1, payload: '%2')").arg(udpTunnelPacketHeader.toString()).arg(payload);
                            QTest::newRow(testName.toStdString().c_str()) << udpTunnelPacketHeader << payload;
                        }
                    }
                }
            }
        }
    }
}

void test_udptunnelpacket::testEncodeDecode()
{
    QFETCH(UDPTunnelPacketHeader, udpTunnelPacketHeader);
    QFETCH(QByteArray, udpTunnelPacketPayload);

    UDPTunnelPacket udpTunnelPacket;
    udpTunnelPacket.setHeader(udpTunnelPacketHeader);
    udpTunnelPacket.setPayload(udpTunnelPacketPayload);

    const auto& encodedData = udpTunnelPacket.encode();
    UDPTunnelPacket udpTunnelPacket2(encodedData);

    switch(udpTunnelPacket.getHeader().getPacketType())
    {
        case UDPTunnelPacketType::INVALID:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkIdStart(), udpTunnelPacket2.getHeader().getChunkIdStart());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkIdEnd(), udpTunnelPacket2.getHeader().getChunkIdEnd());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkIdStart(), udpTunnelPacket2.getHeader().getChunkIdStart());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkIdEnd(), udpTunnelPacket2.getHeader().getChunkIdEnd());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacket2.getHeader().getChunkId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacket2.getHeader().getChunkId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA_FLUSH:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacket2.getHeader().getChunkId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
        case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
            QCOMPARE(udpTunnelPacket.getHeader().getPacketType(), udpTunnelPacket2.getHeader().getPacketType());
            QCOMPARE(udpTunnelPacket.getHeader().getPacketId(), udpTunnelPacket2.getHeader().getPacketId());
            QCOMPARE(udpTunnelPacket.getHeader().getChunkId(), udpTunnelPacket2.getHeader().getChunkId());
            QCOMPARE(udpTunnelPacket.getPayload(), udpTunnelPacket2.getPayload());
            break;
    }
}

// Private

const QString test_udptunnelpacket::toString(const std::vector<size_t>& missingChunkIds)
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

QTEST_MAIN(test_udptunnelpacket)
#include "test_udptunnelpacket.moc"