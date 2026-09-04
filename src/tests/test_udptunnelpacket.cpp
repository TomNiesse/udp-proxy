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
    const std::vector<std::vector<size_t>> missingChunkIdsList = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {0}};

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

void test_udptunnelpacket::testUnitTestFinished()
{
    QCOMPARE(true, false);
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