#include "udptunnelpacketreceiver.h"
#include <QNetworkDatagram>
#include <QVariant>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

#define MAX_RECEIVE_BUFFER_SIZE 50

UDPTunnelPacketReceiver::UDPTunnelPacketReceiver(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, QObject *parent)
    : IUDPTunnelPacketHandler(parent)
    , udpTunnelConnectionSettings(udpTunnelConnectionSettings)
{
    startHandlingPackets();

    const auto& listening = this->ingressSocket.bind(QHostAddress(udpTunnelConnectionSettings.getIngressAddress()), udpTunnelConnectionSettings.getIngressPort());
    if(!listening)
    {
        qDebug() << QDateTime::currentDateTime() << "Receiver: Failed to bind";
        exit(EXIT_FAILURE);
    }

    // If the receiver is allowed to pick a port at random, update the settings after bind() is called.
    // This way, the host connecion(s) know where to send their data to
    if(this->udpTunnelConnectionSettings.getIngressPort() == 0)
    {
        this->udpTunnelConnectionSettings.setIngressPort(this->ingressSocket.localPort());
    }

    setBufferSizes();
}

UDPTunnelPacketReceiver::~UDPTunnelPacketReceiver()
{
    qDebug() << QDateTime::currentDateTime() << "Receiver is being destructed" << Qt::flush;

    QObject::disconnect(&this->ingressSocket, &QUdpSocket::readyRead, this, nullptr);
    this->ingressSocket.close();
}

const UDPTunnelConnectionSettings UDPTunnelPacketReceiver::getUdpTunnelConnectionSettings() const
{
    return this->udpTunnelConnectionSettings;
}

void UDPTunnelPacketReceiver::setEgressAddress(const QByteArray& egressAddress)
{
    this->udpTunnelConnectionSettings.setEgressAddress(egressAddress);
}

void UDPTunnelPacketReceiver::setEgressPort(const int& egressPort)
{
    this->udpTunnelConnectionSettings.setEgressPort(egressPort);
}

const std::vector<UDPTunnelPacket> UDPTunnelPacketReceiver::handleUdpTunnelPacket(const UDPTunnelPacket& udpTunnelPacket)
{
    std::vector<UDPTunnelPacket> udpTunnelPacketResponses;

    const auto& udpTunnelPacketHeader = udpTunnelPacket.getHeader();
    const auto& udpTunnelPacketType = udpTunnelPacketHeader.getPacketType();

    switch(udpTunnelPacketType)
    {
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
        if(this->receiveData && udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA FLUSH REQUEST" << Qt::flush;
            const auto& payload = this->udpTunnelPacketChunkManager.convertChunksToPayload(this->packetId);
            emit receivedData(payload);

            this->packetId = 0;
            this->chunkIdStart = 0;
            this->chunkIdEnd = 0;
            this->receiveData = false;
            udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataFlushAcknowledgementPacket(udpTunnelPacketHeader.getPacketId()));

            // The packet has been flushed. All other resent packages don't matter now. Clear the receive buffer
            const QMutexLocker lock(&this->receiveBufferLock);
            this->receiveBuffer.clear();
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "Resend flush ack" << Qt::flush;
            udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataFlushAcknowledgementPacket(udpTunnelPacketHeader.getPacketId()));
        }
        break;
    case UDPTunnelPacketType::UDP_DATA:
        if(this->receiveData && udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA";

            this->udpTunnelPacketChunkManager.addChunk(udpTunnelPacket);
            if(udpTunnelPacketHeader.getChunkId() == (this->highestContiguousChunkId + 1))
            {
                this->highestContiguousChunkId = udpTunnelPacketHeader.getChunkId();
                if(udpTunnelPacketHeader.getChunkId() % 10000 == 0)
                {
                    qDebug() << QDateTime::currentDateTime() << "Received:" << udpTunnelPacketHeader.getChunkId();
                }
            }
            udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataAcknowledgementPacket(udpTunnelPacketHeader.getPacketId(), udpTunnelPacketHeader.getChunkId()));
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA FOR ANOTHER PACKET!";
            // qDebug() << "PACKET IN QUESTION:" << udpTunnelPacket.getHeader().toString();
        }
        break;
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
        // Set internal registers to the received values
        if(!this->receiveData)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP ANNOUNCEMENT";

            this->receiveData = true;
            this->packetId = udpTunnelPacketHeader.getPacketId();
            this->chunkIdStart = udpTunnelPacketHeader.getChunkIdStart();
            this->chunkIdEnd = udpTunnelPacketHeader.getChunkIdEnd();
            this->highestContiguousChunkId = udpTunnelPacketHeader.getChunkIdStart();

            // Respond with ready to receive and set the expected packet ID and chunk range
            udpTunnelPacketResponses.push_back(UDPTunnelPacket::createAnnouncementAcknowledgementPacket(this->packetId, this->chunkIdStart, this->chunkIdEnd));
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "The resend mechanism did a weird thing"; // TODO: The resend mechanism is acting really weird since the timeout was introduced
        }

        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
        if(this->receiveData && udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA RECEIVED REQUEST";

            // Respond with missing chunk ID's
            const auto& missingChunkIds = this->udpTunnelPacketChunkManager.getMissingChunkIds(this->packetId, this->highestContiguousChunkId, this->chunkIdEnd);
            const auto& udpDataReceivedResponsePackets = UDPTunnelPacket::createUdpDataReceivedResponsePackets(this->packetId, this->highestContiguousChunkId, this->chunkIdEnd, missingChunkIds);
            for(const auto& missingChunks : udpDataReceivedResponsePackets)
            {
                udpTunnelPacketResponses.push_back(missingChunks);
                break;
            }
        }
        else
        {
            qDebug() << QDateTime::currentDateTime() << "RECEIVED UNEXPECTED UDP DATA RECEIVED REQUEST";
        }
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
    case UDPTunnelPacketType::INVALID:
    default:
        qDebug() << QDateTime::currentDateTime() << "RECEIVED INVALID REQUEST";
        break;
    }

    return udpTunnelPacketResponses;
}

const bool UDPTunnelPacketReceiver::getReceiveData() const
{
    return this->receiveData;
}

const size_t UDPTunnelPacketReceiver::getPacketId() const
{
    return this->packetId;
}

const size_t UDPTunnelPacketReceiver::getChunkIdStart() const
{
    return this->chunkIdStart;
}

const size_t UDPTunnelPacketReceiver::getChunkIdEnd() const
{
    return this->chunkIdEnd;
}

const UDPTunnelPacketChunkManager& UDPTunnelPacketReceiver::getUdpTunnelPacketChunkManager() const
{
    return this->udpTunnelPacketChunkManager;
}

void UDPTunnelPacketReceiver::startHandlingPackets()
{
    QObject::connect(&this->ingressSocket, &QUdpSocket::readyRead, this, [this](){
        // Set the thread priority to high, to reduce packet loss
        QThread::currentThread()->setPriority(QThread::LowestPriority);

        // qDebug() << QDateTime::currentDateTime() << "START HANDLING PACKETS";

        receiveBufferLock.lock();
        while(this->ingressSocket.hasPendingDatagrams())
        {
            const auto& networkDatagram = this->ingressSocket.receiveDatagram();

            if(this->receiveBuffer.size() < MAX_RECEIVE_BUFFER_SIZE)
            {
                this->receiveBuffer.push_back(networkDatagram);
            }
        }

        if(!handlingReceivedData)
        {
            this->handlingReceivedData = true;

            // Handle received data
            QTimer::singleShot(0, this, [this](){
                bool busy = true;
                while(busy)
                {
                    QNetworkDatagram networkDatagram;

                    receiveBufferLock.lock();
                    const auto& bufferEmpty = this->receiveBuffer.size() <= 0;
                    if(bufferEmpty)
                    {
                        this->handlingReceivedData = false;
                        busy = false;
                    }
                    else
                    {
                        networkDatagram = this->receiveBuffer.back();
                        this->receiveBuffer.pop_back();
                    }
                    receiveBufferLock.unlock();

                    if(busy)
                    {
                        const auto& data = networkDatagram.data();
                        if(data.size() > 0)
                        {
                            const auto& udpTunnelPacket = UDPTunnelPacket(data);
                            // qDebug() << QDateTime::currentDateTime() << "Receiver received:" << udpTunnelPacket.toString();
                            const auto& responses = this->handleUdpTunnelPacket(udpTunnelPacket);
                            for(const auto& response : responses)
                            {
                                // qDebug() << QDateTime::currentDateTime() << "Receiver sends:" << response.getHeader().toString();
                                const auto& encodedData = response.encode();
                                bool dataSent = false;
                                do
                                {
                                    qint64 bytesWritten = this->egressSocket.writeDatagram(encodedData, QHostAddress(this->udpTunnelConnectionSettings.getEgressAddress()), this->udpTunnelConnectionSettings.getEgressPort());
                                    dataSent = (bytesWritten == encodedData.size());
                                    if(!dataSent)
                                    {
                                        qDebug() << QDateTime::currentDateTime() << "Failed to send data this round!" << Qt::flush;
                                    }
                                } while(!dataSent);
                            }
                        }
                    }
                }
            });
        }

        this->receiveBufferLock.unlock();

        QThread::currentThread()->setPriority(QThread::NormalPriority);
    });
}

void UDPTunnelPacketReceiver::setBufferSizes()
{
    // Set buffer sizes
    const auto& receiveBufferSize = 1024 * 1024 * 100 * 8;
    this->ingressSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(receiveBufferSize));
    this->ingressSocket.setReadBufferSize(receiveBufferSize);

    const auto& sendBufferSize = 1024 * 1024 * 100 * 8;
    this->egressSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(sendBufferSize));
    this->egressSocket.setReadBufferSize(sendBufferSize);
}