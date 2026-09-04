#include "udptunnelpacketsender.h"
#include <QVariant>
#include <QDateTime>
#include <QCoreApplication>

#define MAX_RECEIVE_BUFFER_SIZE 50

UDPTunnelPacketSender::UDPTunnelPacketSender(const UDPTunnelConnectionSettings& udpTunnelConnectionSettings, QObject *parent)
    : IUDPTunnelPacketHandler(parent)
    , udpTunnelConnectionSettings(udpTunnelConnectionSettings)
{
    QObject::connect(&this->ingressSocket, &QUdpSocket::readyRead, this, [this](){
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
                        const auto& udpTunnelPacket = UDPTunnelPacket(data);
                        // qDebug() << QDateTime::currentDateTime() << "Sender received:" << udpTunnelPacket.toString().size();

                        // If the received data is a response to a request, stop the response timeout timer
                        if(responseReceived(udpTunnelPacket))
                        {
                            this->udpTunnelPacketBuffer.increaseBufferSize(1);
                            // this->decreaseResponseTimeout(1);
                            this->lastSentUdpTunnelPacket = UDPTunnelPacket();
                            stopResponseTimeoutTimer();
                        }

                        // Handle the received data and send back the responses
                        const auto& responses = this->handleUdpTunnelPacket(udpTunnelPacket);

                        // In case the last chunk is a request (not data), set it as the last sent packet, in case the request did not arrive
                        if(responses.size() > 0)
                        {
                            const auto& response = responses.at(responses.size()-1);
                            // If the last sent packet is a request, prepare the the timeout handling
                            switch(response.getHeader().getPacketType())
                            {
                            case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
                                this->lastSentUdpTunnelPacket = response;
                                restartResponseTimeoutTimer();
                                break;
                            case UDPTunnelPacketType::UDP_DATA_FLUSH:
                                this->lastSentUdpTunnelPacket = response;
                                restartResponseTimeoutTimer();
                                break;
                            case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
                                this->lastSentUdpTunnelPacket = response;
                                restartResponseTimeoutTimer();
                                break;
                            case UDPTunnelPacketType::UDP_DATA:
                                // this->lastSentUdpTunnelPacket = response;
                                // restartResponseTimeoutTimer();
                                // break;
                            case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
                            case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
                            case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
                            case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
                            case UDPTunnelPacketType::INVALID:
                            default:
                                break;
                            }
                        }

                        for(const auto& response : responses)
                        {
                            const auto& encodedData = response.encode();

                            // qDebug() << QDateTime::currentDateTime() << "Sender send:" << response.getHeader().toString();
                            bool dataSent = false;
                            do
                            {
                                qint64 bytesWritten = this->egressSocket.writeDatagram(encodedData, QHostAddress(this->udpTunnelConnectionSettings.getEgressAddress()), this->udpTunnelConnectionSettings.getEgressPort());
                                QCoreApplication::processEvents();
                                dataSent = (bytesWritten == encodedData.size());
                                if(!dataSent)
                                {
                                    qDebug() << QDateTime::currentDateTime() << "Failed to send data this round!" << Qt::flush;
                                }
                            } while(!dataSent);
                        }
                    }
                }
            });
        }

        receiveBufferLock.unlock();
    });

    connect(&udpTunnelPacketBuffer, &UDPTunnelPacketBuffer::resendPacket, this, [this](const UDPTunnelPacket udpTunnelPacket){
        this->udpTunnelPacketBuffer.decreaseBufferSize(1);
        const auto& encodedData = udpTunnelPacket.encode();
        // qDebug() << "RESEND:" << udpTunnelPacket.getHeader().toString();
        this->egressSocket.writeDatagram(encodedData, QHostAddress(this->udpTunnelConnectionSettings.getEgressAddress()), this->udpTunnelConnectionSettings.getEgressPort());
        QCoreApplication::processEvents();
    });

    bool listening = false;
    if(this->udpTunnelConnectionSettings.getIngressPort() == 0)
    {
        listening = this->ingressSocket.bind(QHostAddress(this->udpTunnelConnectionSettings.getIngressAddress()));
        this->udpTunnelConnectionSettings.setIngressPort(this->ingressSocket.localPort());
    }
    else
    {
        listening = this->ingressSocket.bind(QHostAddress(this->udpTunnelConnectionSettings.getIngressAddress()), this->udpTunnelConnectionSettings.getIngressPort());
    }
    if(!listening)
    {
        qDebug() << QDateTime::currentDateTime() << "Sender: Failed to bind";
        exit(EXIT_FAILURE);
    }
    this->ingressSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(1024 * 1024));

    // Set buffer sizes
    const auto& receiveBufferSize = 1024 * 1024 * 100 * 8;
    this->ingressSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(receiveBufferSize));
    this->ingressSocket.setReadBufferSize(receiveBufferSize);

    const auto& sendBufferSize = 1024 * 1024 * 100 * 8;
    this->egressSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, QVariant(sendBufferSize));
    this->egressSocket.setReadBufferSize(sendBufferSize);
}

UDPTunnelPacketSender::~UDPTunnelPacketSender()
{
    disconnect(&this->ingressSocket, &QUdpSocket::readyRead, this, nullptr);
    disconnect(&udpTunnelPacketBuffer, &UDPTunnelPacketBuffer::resendPacket, this, nullptr);
    this->ingressSocket.close();
}

void UDPTunnelPacketSender::sendData(const QByteArray& data)
{
    QMutexLocker lock(&this->lock);

    this->bufferData(data);
}

const UDPTunnelConnectionSettings UDPTunnelPacketSender::getUdpTunnelConnectionSettings() const
{
    return this->udpTunnelConnectionSettings;
}

void UDPTunnelPacketSender::setEgressAddress(const QByteArray& egressAddress)
{
    this->udpTunnelConnectionSettings.setEgressAddress(egressAddress);
}

void UDPTunnelPacketSender::setEgressPort(const int& egressPort)
{
    this->udpTunnelConnectionSettings.setEgressPort(egressPort);
}

// Private slots

void UDPTunnelPacketSender::handleResponseTimeout()
{
    emit responseTimeoutOccurred();

    this->udpTunnelPacketBuffer.decreaseBufferSize(1);
    // this->increaseResponseTimeout(2);

    if(this->packetId != this->lastSentUdpTunnelPacket.getHeader().getPacketId())
    {
        qDebug() << QDateTime::currentDateTime() << "Not sending stale packet";
        return;
    }

    // Resend the data
    const auto& encodedData = this->lastSentUdpTunnelPacket.encode();
    bool dataSent = false;
    do
    {
        const auto& egressAddress = this->udpTunnelConnectionSettings.getEgressAddress();
        const auto& egressPort = this->udpTunnelConnectionSettings.getEgressPort();
        qint64 bytesWritten = this->egressSocket.writeDatagram(encodedData, QHostAddress(egressAddress), egressPort);
        QCoreApplication::processEvents();
        dataSent = (bytesWritten == encodedData.size());
        if(!dataSent)
        {
            qDebug() << QDateTime::currentDateTime() << "Failed to send data this round!" << Qt::flush;
        }
    } while(!dataSent);
}

// Private

const std::vector<UDPTunnelPacket> UDPTunnelPacketSender::handleUdpTunnelPacket(const UDPTunnelPacket& udpTunnelPacket)
{
    std::vector<UDPTunnelPacket> udpTunnelPacketResponses;

    const auto& udpTunnelPacketHeader = udpTunnelPacket.getHeader();
    const auto& udpTunnelPacketType = udpTunnelPacketHeader.getPacketType();
    const auto& udpTunnelPacketPayload = udpTunnelPacket.getPayload();

    switch(udpTunnelPacketType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        if(udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP ANNOUNCEMENT ACKNOWLEDGEMENT";

            const auto& initialUdpTunnelPackets = udpTunnelPacketBuffer.getInitialPacketChunks();
            for(const auto& udpTunnelPacket : initialUdpTunnelPackets)
            {
                udpTunnelPacketResponses.push_back(udpTunnelPacket);
            }

            // // Send a data received request
            // udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataReceivedRequestPacket(this->packetId));
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP ANNOUNCEMENT ACKNOWLEDGEMENT FOR ANOTHER PACKET!";
        }
        break;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        // if(udpTunnelPacketHeader.getPacketId() == this->packetId)
        // {
        //     qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA RECEIVED RESPONSE";

        //     // If the payload contains missing packets, send the missing packets
        //     UDPTunnelPacketMissingChunkList udpTunnelPacketMissingChunkList(udpTunnelPacketPayload);
        //     const auto& missingChunkIds = udpTunnelPacketMissingChunkList.getMissingChunks();
        //     this->udpTunnelPacketsLock.lock();
        //     const auto numberOfNotReceivedChunks = this->udpTunnelPackets.size();
        //     this->udpTunnelPacketsLock.unlock();
        //     if(missingChunkIds.size() > 0 && numberOfNotReceivedChunks > 0)
        //     {
        //         this->udpTunnelPacketsLock.lock();
        //         for(const auto& missingChunkId : missingChunkIds)
        //         {
        //             for(size_t chunkId = this->highestContiguousChunkId; chunkId <= this->chunkIdEnd; chunkId++)
        //             {
        //                 const auto& chunkIndex = (chunkId - this->chunkIdStart);
        //                 const auto& chunk = this->udpTunnelPackets.at(chunkIndex);
        //                 if(chunk.getHeader().getChunkId() == missingChunkId)
        //                 {
        //                     udpTunnelPacketResponses.push_back(chunk);
        //                     break;
        //                 }
        //             }
        //             break;// TODO: remove!
        //         }
        //         this->udpTunnelPacketsLock.unlock();

        //         // // Send a data received request
        //         // udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataReceivedRequestPacket(this->packetId));
        //     }
        //     else
        //     {
        //         // Respond with a data flush request
        //         udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataFlushPacket(this->packetId));
        //     }
        // }
        // else
        // {
        //     qDebug() << "RECEIVED A RECEIVED DATA RESPONSE, BUT ITS NOT FOR THIS PACKET ID!!";
        // }
        break;
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
        if(udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << "RECEIVED UDP DATA ACKNOWLEDGEMENT!";
            const auto& udpTunnelPackets = this->udpTunnelPacketBuffer.handleReceivedPacket(udpTunnelPacket);
            if(udpTunnelPackets.size() > 0)
            {
                for(const auto& udpTunnelPacket : udpTunnelPackets)
                {
                    udpTunnelPacketResponses.push_back(udpTunnelPacket);
                }
            }
            else
            {
                const auto& bufferSize = this->udpTunnelPacketBuffer.getBufferSize();
                if(bufferSize <= 0)
                {
                    // Respond with a data flush request
                    udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataFlushPacket(this->packetId));
                }
            }

            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA ACKNOWLEDGEMENT!";
            // if(udpTunnelPacketHeader.getChunkId() == (this->highestContiguousChunkId + 1))
            // {
            //     this->highestContiguousChunkId = udpTunnelPacketHeader.getChunkId();
            // }
            // udpTunnelPacketResponses.push_back(UDPTunnelPacket::createUdpDataReceivedRequestPacket(this->packetId));
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA ACKNOWLEDGEMENT FOR ANOTHER PACKET!";
        }
        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        if(udpTunnelPacketHeader.getPacketId() == this->packetId)
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA FLUSH ACKNOWLEDGEMENT";

            stopResponseTimeoutTimer();
            incrementPacketId();
            emit dataSent();

            // The packet has been flushed. All other resent packages don't matter now. Clear the receive buffer
            const QMutexLocker lock(&this->receiveBufferLock);
            this->receiveBuffer.clear();
        }
        else
        {
            // qDebug() << QDateTime::currentDateTime() << "RECEIVED UDP DATA FLUSH ACKNOWLEDGEMENT FOR ANOTHER PACKET";
        }

        break;
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
    case UDPTunnelPacketType::UDP_DATA:
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
    case UDPTunnelPacketType::INVALID:
    default:
        qDebug() << QDateTime::currentDateTime() << "received invalid!";
        break;
    }

    return udpTunnelPacketResponses;
}

void UDPTunnelPacketSender::bufferData(const QByteArray& data)
{
    this->sendBufferLock.lock();
    this->sendBuffer.push_back(data);
    this->sendBufferLock.unlock();

    this->sendLock.lock();
    if(!this->sendingData)
    {
        this->sendingData = true;
        startSendingData();
    }
    this->sendLock.unlock();
}

void UDPTunnelPacketSender::startSendingData()
{
    this->sendBufferLock.lock();
    const auto data = this->sendBuffer.front();
    this->sendBufferLock.unlock();

    QTimer::singleShot(0, [this, data](){
        this->udpTunnelPacketBuffer.setUdpTunnelPackets(UDPTunnelPacketSender::dataToUdpTunnelPackets(data, this->packetId));
        this->sendUdpTunnelPackets();
    });
}

void UDPTunnelPacketSender::incrementPacketId()
{
    if(++this->packetId >= SIZE_MAX)
    {
        this->packetId = 0;
    }
}

void UDPTunnelPacketSender::sendUdpTunnelPackets()
{
    // Get the first and last chunk ID and create an announcement packet
    const auto& chunkIdStart = udpTunnelPacketBuffer.getChunkIdStart();
    const auto& chunkIdEnd = udpTunnelPacketBuffer.getChunkIdEnd();

    const auto& udpTunnelPacketAnnouncement = UDPTunnelPacket::createAnnouncementPacket(this->packetId, chunkIdStart, chunkIdEnd);

    // Listen on the ingress socket and send the announcement packet
    QEventLoop eventLoop;
    QObject::connect(this, &UDPTunnelPacketSender::dataSent, &eventLoop, &QEventLoop::quit);
    this->lastSentUdpTunnelPacket = udpTunnelPacketAnnouncement;
    const auto& encodedData = udpTunnelPacketAnnouncement.encode();
    bool dataSent = false;
    do
    {
        qint64 bytesWritten = this->egressSocket.writeDatagram(encodedData, QHostAddress(this->udpTunnelConnectionSettings.getEgressAddress()), this->udpTunnelConnectionSettings.getEgressPort());
        QCoreApplication::processEvents();
        dataSent = (bytesWritten == encodedData.size());
        if(!dataSent)
        {
            qDebug() << QDateTime::currentDateTime() << "Failed to send data this round!" << Qt::flush;
        }
    } while(!dataSent);
    restartResponseTimeoutTimer();
    eventLoop.exec();
    QObject::disconnect(this, &UDPTunnelPacketSender::dataSent, &eventLoop, &QEventLoop::quit);

    this->udpTunnelPacketBuffer.clearUdpTunnelPackets();

    this->sendBufferLock.lock();
    if(this->sendBuffer.size() > 0)
    {
        this->sendBuffer.erase(this->sendBuffer.begin());
    }
    const auto& sendBufferEmpty = this->sendBuffer.size() <= 0;
    this->sendBufferLock.unlock();

    if(!sendBufferEmpty)
    {
        startSendingData();
    }
    else
    {
        // The chunks have been sent
        this->sendBufferLock.lock();
        this->sendingData = false;
        this->sendBufferLock.unlock();
    }
}

const bool UDPTunnelPacketSender::responseReceived(const UDPTunnelPacket& udpTunnelPacket) const
{
    const auto& udpTunnelPacketId = udpTunnelPacket.getHeader().getPacketId();
    const auto& udpTunnelPacketChunkId = udpTunnelPacket.getHeader().getChunkId();
    const auto& lastSentUdpTunnelPacketId = this->lastSentUdpTunnelPacket.getHeader().getPacketId();
    const auto& lastSentUdpTunnelPacketChunkId = this->lastSentUdpTunnelPacket.getHeader().getChunkId();

    if(udpTunnelPacketId != lastSentUdpTunnelPacketId)
    {
        // qDebug() << QDateTime::currentDateTime() << "Response tunnel packet ID did not match!";
        return false;
    }

    if(udpTunnelPacketChunkId != lastSentUdpTunnelPacketChunkId)
    {
        // qDebug() << QDateTime::currentDateTime() << "Response tunnel chunk ID did not match!";
        return false;
    }

    const auto& lastSentUdpTunnelPacketPacketType = this->lastSentUdpTunnelPacket.getHeader().getPacketType();
    const auto& udpTunnelPacketPacketType = udpTunnelPacket.getHeader().getPacketType();

    switch(udpTunnelPacketPacketType)
    {
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT_ACKNOWLEDGEMENT:
        return lastSentUdpTunnelPacketPacketType == UDPTunnelPacketType::UDP_ANNOUNCEMENT;
    case UDPTunnelPacketType::UDP_DATA_FLUSH_ACKNOWLEDGEMENT:
        return lastSentUdpTunnelPacketPacketType == UDPTunnelPacketType::UDP_DATA_FLUSH;
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_RESPONSE:
        return lastSentUdpTunnelPacketPacketType == UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST;
    case UDPTunnelPacketType::UDP_DATA_ACKNOWLEDGEMENT:
        // return lastSentUdpTunnelPacketPacketType == UDPTunnelPacketType::UDP_DATA;
    case UDPTunnelPacketType::UDP_ANNOUNCEMENT:
    case UDPTunnelPacketType::UDP_DATA:
    case UDPTunnelPacketType::UDP_DATA_FLUSH:
    case UDPTunnelPacketType::UDP_DATA_RECEIVED_REQUEST:
    case UDPTunnelPacketType::INVALID:
    default:
        return false;
    }
}

void UDPTunnelPacketSender::startResponseTimeoutTimer()
{
    QObject::connect(&this->responseTimeoutTimer, &QTimer::timeout, this, &UDPTunnelPacketSender::handleResponseTimeout);
    this->responseTimeoutTimer.setInterval(this->responseTimeout);
    this->responseTimeoutTimer.start();
}

void UDPTunnelPacketSender::stopResponseTimeoutTimer()
{
    QObject::disconnect(&this->responseTimeoutTimer, &QTimer::timeout, this, &UDPTunnelPacketSender::handleResponseTimeout);
    this->responseTimeoutTimer.stop();
}

void UDPTunnelPacketSender::restartResponseTimeoutTimer()
{
    this->stopResponseTimeoutTimer();
    this->startResponseTimeoutTimer();
}

void UDPTunnelPacketSender::increaseResponseTimeout(const int& factor)
{
    this->responseTimeout *= factor;
    this->responseTimeoutTimer.setInterval(this->responseTimeout);
    // qDebug() << QDateTime::currentDateTime() << "*response timeout is now" << this->responseTimeout;
}

void UDPTunnelPacketSender::decreaseResponseTimeout(const int& amount)
{
    this->responseTimeout -= amount;
    if(this->responseTimeout < 1)
    {
        this->responseTimeout = 1;
    }
    this->responseTimeoutTimer.setInterval(this->responseTimeout);
    // qDebug() << QDateTime::currentDateTime() << "-response timeout is now" << this->responseTimeout;
}

void UDPTunnelPacketSender::setResponseTimeout(const int& responseTimeout)
{
    this->responseTimeout = responseTimeout;
    this->responseTimeoutTimer.setInterval(this->responseTimeout);
    // qDebug() << QDateTime::currentDateTime() << "response timeout is now" << this->responseTimeout;
}

const std::vector<UDPTunnelPacket> UDPTunnelPacketSender::dataToUdpTunnelPackets(const QByteArray& data, const size_t& packetId)
{
    std::vector<UDPTunnelPacket> out;

    // Create data packets
    UDPTunnelPacketHeader udpTunnelPacketHeader;
    udpTunnelPacketHeader.setPacketId(packetId);

    const auto& splitData = UDPTunnelPacket::split(data, MAX_UDP_PACKET_PAYLOAD_SIZE);
    const auto& encodedSplitData = UDPTunnelPacket::addHeaders(udpTunnelPacketHeader, splitData);

    // Add all packets to the output list
    for(const auto& udpDataPacket : encodedSplitData)
    {
        out.push_back(udpDataPacket);
    }

    return out;
}