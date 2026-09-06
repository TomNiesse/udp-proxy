#ifndef TEST_UDPTUNNELPACKET_H
#define TEST_UDPTUNNELPACKET_H

#include <QObject>
#include <QTest>

class test_udptunnelpacket : public QObject
{
    Q_OBJECT
public:
    test_udptunnelpacket();
    ~test_udptunnelpacket();

private slots:
    void testConstructors_data();
    void testConstructors();

    void testGetHeader_data();
    void testGetHeader();
    void testSetHeader_data();
    void testSetHeader();
    void testGetPayload_data();
    void testGetPayload();
    void testSetPayload_data();
    void testSetPayload();
    void testEncodeDecode_data();
    void testEncodeDecode();

    // TODO: FINISH UNIT TEST!

    // static const std::vector<QByteArray> split(const QByteArray& payload, const size_t& chunkSize);
    // static const std::vector<UDPTunnelPacket> addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& splitPayload);
    // static const QByteArray payloadFromChunks(const std::vector<UDPTunnelPacket>& chunks);

private:
    inline static const QString toString(const std::vector<size_t>& missingChunkIds);
};

#endif // TEST_UDPTUNNELPACKET_H
