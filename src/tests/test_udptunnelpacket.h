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
    void testUnitTestFinished(); // TODO: finish the unit test!

    // const UDPTunnelPacketHeader& getHeader() const;
    // void setHeader(const UDPTunnelPacketHeader& header);
    // const QByteArray& getPayload() const;
    // void setPayload(const QByteArray& payload);
    // const QByteArray encode() const;

    // const QString toString() const;

    // static const std::vector<QByteArray> split(const QByteArray& payload, const size_t& chunkSize);
    // static const std::vector<UDPTunnelPacket> addHeaders(const UDPTunnelPacketHeader& header, const std::vector<QByteArray>& splitPayload);
    // static const QByteArray payloadFromChunks(const std::vector<UDPTunnelPacket>& chunks);

private:
    inline static const QString toString(const std::vector<size_t>& missingChunkIds);
};

#endif // TEST_UDPTUNNELPACKET_H
