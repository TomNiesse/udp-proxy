#include "udptunnelpackettransceiver.h"
#include <QTimer>
#include <QEventLoop>
#include <QNetworkDatagram>
#include <QCoreApplication>

UDPTunnelPacketTransceiver::UDPTunnelPacketTransceiver(const QString& ingressAddress, const uint16_t& ingressPort, const QString& egressAddress, const uint16_t& egressPort)
{
    this->ingressAddress = ingressAddress;
    this->ingressPort = ingressPort;
    this->egressAddress = egressAddress;
    this->egressPort = egressPort;

    QObject::connect(&this->ingressSocket, &QUdpSocket::readyRead, this, &UDPTunnelPacketTransceiver::bufferReceivedUdpData, Qt::DirectConnection);

    if(!this->ingressSocket.bind(QHostAddress(this->ingressAddress), this->ingressPort))
    {
        qDebug().noquote() << "Unable to bind to" << this->ingressAddress << this->ingressPort;
        exit(EXIT_FAILURE);
    }
}

UDPTunnelPacketTransceiver::~UDPTunnelPacketTransceiver()
{
    QObject::disconnect(&this->ingressSocket, &QUdpSocket::readyRead, this, &UDPTunnelPacketTransceiver::bufferReceivedUdpData);
}

void UDPTunnelPacketTransceiver::send(const QByteArray& data)
{
    const QMutexLocker lock(&this->lock);

    if(data.length() == 0)
    {
        return;
    }

    this->bufferEgressUdpData(data);
}

// Private slots

void UDPTunnelPacketTransceiver::bufferEgressUdpData(const QByteArray& data)
{
    this->egressBufferLock.lock();
    this->egressBuffer.push_back(data);
    this->egressBufferLock.unlock();

    this->egressBusyLock.lock();
    if(!this->egressBusy)
    {
        this->egressBusy = true;
        QTimer::singleShot(0, [this, data](){
            this->sendData();
        });
    }
    this->egressBusyLock.unlock();
}

void UDPTunnelPacketTransceiver::bufferReceivedUdpData()
{
    this->ingressBufferLock.lock();
    while(this->ingressSocket.hasPendingDatagrams())
    {
        const auto& datagram = this->ingressSocket.receiveDatagram();
        const auto& data = datagram.data();
        if(this->ingressBuffer.empty() || this->ingressBuffer.back() != data)
        {
            this->ingressBuffer.push_back(data);
        }
    }
    this->ingressBufferLock.unlock();

    this->handleReceivedUdpData();
}

void UDPTunnelPacketTransceiver::sendData(const int& timeout)
{
    bool bufferEmpty = false;
    do
    {
        this->egressBufferLock.lock();
        QByteArray data;
        if(!this->egressBuffer.empty())
        {
            data = this->egressBuffer.front();
        }
        this->egressBufferLock.unlock();

        if(data.length() > 0)
        {
            bool responseReceived = false;
            QUdpSocket egressSocket;
            do
            {
                egressSocket.writeDatagram(data, QHostAddress(this->egressAddress), this->egressPort);
                const bool hybridMode = false;
                if(hybridMode == true)
                {
                    QCoreApplication::processEvents();
                }
                responseReceived = this->ingressSocket.waitForReadyRead(timeout);
            } while (!responseReceived);

            this->bufferReceivedUdpData();
        }

        this->egressBufferLock.lock();
        bufferEmpty = this->egressBuffer.empty();
        if(!bufferEmpty)
        {
            this->egressBuffer.erase(this->egressBuffer.begin());
        }
        this->egressBufferLock.unlock();
    } while(!bufferEmpty);

    this->egressBusyLock.lock();
    this->egressBusy = false;
    this->egressBusyLock.unlock();
}

void UDPTunnelPacketTransceiver::handleReceivedUdpData()
{
    bool bufferEmpty = false;
    do
    {
        this->ingressBufferLock.lock();
        if(!this->ingressBuffer.empty())
        {
            const QByteArray& data = this->ingressBuffer.front();
            if(data.length() > 0)
            {
                const auto& udpTunnelPacket = UDPTunnelPacket(data);
                switch(udpTunnelPacket.getHeader().getPacketType())
                {
                case UDPTunnelPacketType::UDP_DATA:
                    QTimer::singleShot(0, this, [this, udpTunnelPacket](){
                        this->handleReceivedData(udpTunnelPacket);
                    });
                    break;
                case UDPTunnelPacketType::UDP_DATA_FLUSH:
                    QTimer::singleShot(0, this, [this, udpTunnelPacket](){
                        this->handleReceivedDataFlush(udpTunnelPacket);
                    });
                    break;
                case UDPTunnelPacketType::UDP_ACKNOWLEDGEMENT:
                    QTimer::singleShot(0, this, [this](){
                        emit this->acknowledgementReceived();
                    });
                    break;
                default:
                    break;
                }
            }
        }
        this->ingressBufferLock.unlock();

        this->ingressBufferLock.lock();
        bufferEmpty = this->ingressBuffer.empty();
        if(!bufferEmpty)
        {
            this->ingressBuffer.erase(this->ingressBuffer.begin());
        }
        this->ingressBufferLock.unlock();
    } while(!bufferEmpty);
}

// Private

void UDPTunnelPacketTransceiver::handleReceivedData(const UDPTunnelPacket& udpTunnelPacket)
{
    this->udpTunnelPacketChunkManager.addChunk(udpTunnelPacket);
    this->sendUdpAcknowledgement(udpTunnelPacket);
}

void UDPTunnelPacketTransceiver::handleReceivedDataFlush(const UDPTunnelPacket& udpTunnelPacket)
{
    const auto& data = this->udpTunnelPacketChunkManager.convertChunksToPayload(udpTunnelPacket.getHeader().getPacketId());
    if(data.size() > 0)
    {
        QTimer::singleShot(0, this, [this, data](){
            emit this->receivedData(data);
        });
    }
    this->sendUdpAcknowledgement(udpTunnelPacket);
}

void UDPTunnelPacketTransceiver::handleReceivedUdpAcknowledgement()
{
    emit this->acknowledgementReceived();
}

void UDPTunnelPacketTransceiver::sendUdpAcknowledgement(const UDPTunnelPacket& udpTunnelPacket)
{
    auto acknowledgementPacket = udpTunnelPacket;
    auto acknowledgementPacketHeader = acknowledgementPacket.getHeader();
    acknowledgementPacketHeader.setPacketType(UDPTunnelPacketType::UDP_ACKNOWLEDGEMENT);
    acknowledgementPacket.setHeader(acknowledgementPacketHeader);
    acknowledgementPacket.setPayload({});

    QUdpSocket egressSocket;
    egressSocket.writeDatagram(acknowledgementPacket.encode(), QHostAddress(this->egressAddress), this->egressPort);
}
