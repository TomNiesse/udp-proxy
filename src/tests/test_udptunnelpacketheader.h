#ifndef TEST_UDPTUNNELPACKETHEADER_H
#define TEST_UDPTUNNELPACKETHEADER_H

#include <QObject>
#include <QTest>

class test_udptunnelpacketheader : public QObject
{
    Q_OBJECT
public:
    test_udptunnelpacketheader();
    ~test_udptunnelpacketheader();

private slots:
    void testGetPacketType_data();
    void testGetPacketType();
    void testSetPacketType_data();
    void testSetPacketType();
    void testGetPacketId_data();
    void testGetPacketId();
    void testSetPacketId_data();
    void testSetPacketId();
    void testGetChunkId_data();
    void testGetChunkId();
    void testSetChunkId_data();
    void testSetChunkId();
    void testGetChunkIdStart_data();
    void testGetChunkIdStart();
    void testSetChunkIdStart_data();
    void testSetChunkIdStart();
    void testGetChunkIdEnd_data();
    void testGetChunkIdEnd();
    void testSetChunkIdEnd_data();
    void testSetChunkIdEnd();
    void testEncodeDecode_data();
    void testEncodeDecode();
    void testGetHeaderSize_data();
    void testGetHeaderSize();

signals:
};

#endif // TEST_UDPTUNNELPACKETHEADER_H
