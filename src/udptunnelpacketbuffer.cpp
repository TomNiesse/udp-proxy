#include "udptunnelpacketbuffer.h"




#include <QDebug>

UDPTunnelPacketBuffer::UDPTunnelPacketBuffer(QObject *parent)
    : QObject{parent}
{

}

void UDPTunnelPacketBuffer::setUdpTunnelPackets(const std::vector<UDPTunnelPacket>& udpTunnelPackets)
{
    this->udpTunnelPackets = udpTunnelPackets;
    this->chunkIndex = 0;
}

void UDPTunnelPacketBuffer::clearUdpTunnelPackets()
{
    this->udpTunnelPackets.clear();
    this->chunkIndex = 0;
}

const std::vector<UDPTunnelPacket> UDPTunnelPacketBuffer::getInitialPacketChunks()
{
    const QMutexLocker lock(&this->lock);

    std::vector<UDPTunnelPacket> responsePackets;

    while(this->buffer.size() < this->bufferSize && this->chunkIndex < this->udpTunnelPackets.size())
    {
        const auto& udpTunnelPacket = this->udpTunnelPackets.at(this->chunkIndex++);
        this->buffer.push_back({udpTunnelPacket, std::make_unique<QTimer>()});
        auto& resendTimeoutTimer = this->buffer.at(this->buffer.size()-1).second;
        connect(resendTimeoutTimer.get(), &QTimer::timeout, this, [this, udpTunnelPacket](){
            emit resendPacket(udpTunnelPacket);
        });
        resendTimeoutTimer->setSingleShot(false);
        resendTimeoutTimer->setInterval(1);
        resendTimeoutTimer->start();

        responsePackets.push_back(udpTunnelPacket);
    }

    return responsePackets;
}

const std::vector<UDPTunnelPacket> UDPTunnelPacketBuffer::handleReceivedPacket(const UDPTunnelPacket& udpTunnelPacket)
{
    const QMutexLocker lock(&this->lock);

    removeFromBuffer(udpTunnelPacket);
    const auto& responsePackets = updateBufferAndGetNewChunks();

    return responsePackets;
}

void UDPTunnelPacketBuffer::increaseBufferSize(const size_t& amount)
{
    const QMutexLocker lock(&this->lock);

    this->bufferSize += amount;
    // qDebug() << "+Buffer size is now:" << this->bufferSize;
}

void UDPTunnelPacketBuffer::decreaseBufferSize(const size_t& amount)
{
    const QMutexLocker lock(&this->lock);

    if(amount > this->bufferSize)
    {
        this->bufferSize = 1;
    }
    else
    {
        this->bufferSize -= amount;
        if(this->bufferSize <= 0)
        {
            this->bufferSize = 1;
        }
    }
    // qDebug() << "-Buffer size is now:" << this->bufferSize;
}

void UDPTunnelPacketBuffer::packetSent(const UDPTunnelPacket& udpTunnelPacket)
{
    const QMutexLocker lock(&this->lock);

}

const size_t UDPTunnelPacketBuffer::getBufferSize() const
{
    const QMutexLocker lock(&this->lock);

    return this->buffer.size();
}

const size_t UDPTunnelPacketBuffer::getChunkIdStart() const
{
    const QMutexLocker lock(&this->lock);

    if(this->udpTunnelPackets.size() > 0)
    {
        return this->udpTunnelPackets.at(0).getHeader().getChunkId();
    }

    return 0;
}

const size_t UDPTunnelPacketBuffer::getChunkIdEnd() const
{
    const QMutexLocker lock(&this->lock);

    if(this->udpTunnelPackets.size() > 0)
    {
        return this->udpTunnelPackets.at(this->udpTunnelPackets.size() - 1).getHeader().getChunkId();
    }

    return 0;
}

// Private

void UDPTunnelPacketBuffer::removeFromBuffer(const UDPTunnelPacket& udpTunnelPacket)
{
    auto itr = std::remove_if(this->buffer.begin(), this->buffer.end(), [this, udpTunnelPacket](const std::pair<UDPTunnelPacket, std::unique_ptr<QTimer>>& bufferedUdpTunnelPacket){
        const auto& bufferedUdpTunnelPacketId = bufferedUdpTunnelPacket.first.getHeader().getPacketId();
        const auto& bufferedUdpTunnelPacketChunkId = bufferedUdpTunnelPacket.first.getHeader().getChunkId();
        const auto& udpTunnelPacketId = udpTunnelPacket.getHeader().getPacketId();
        const auto& udpTunnelPacketChunkId = udpTunnelPacket.getHeader().getChunkId();
        const auto& removeFromBuffer = (bufferedUdpTunnelPacketId == udpTunnelPacketId && bufferedUdpTunnelPacketChunkId == udpTunnelPacketChunkId);

        if(removeFromBuffer)
        {
            // qDebug() << "REMOVE FROM BUFFER:" << udpTunnelPacket.getHeader().toString();

            // Remove the timeout timer
            auto& resendTimeoutTimer = bufferedUdpTunnelPacket.second;
            resendTimeoutTimer->stop();
            disconnect(resendTimeoutTimer.get(), &QTimer::timeout, this, nullptr);
        }

        return removeFromBuffer;
    });
    this->buffer.erase(itr, this->buffer.end());
}

const std::vector<UDPTunnelPacket> UDPTunnelPacketBuffer::updateBufferAndGetNewChunks()
{
    // qDebug() << "Start buffer size:" << this->buffer.size();

    std::vector<UDPTunnelPacket> responsePackets;

    while(this->buffer.size() < this->bufferSize && this->chunkIndex < this->udpTunnelPackets.size())
    {
        const auto& udpTunnelPacket = this->udpTunnelPackets.at(this->chunkIndex++);
        this->buffer.push_back({udpTunnelPacket, std::make_unique<QTimer>()});
        auto& resendTimeoutTimer = this->buffer.at(this->buffer.size()-1).second;
        connect(resendTimeoutTimer.get(), &QTimer::timeout, this, [this, udpTunnelPacket](){
            emit resendPacket(udpTunnelPacket);
        });
        resendTimeoutTimer->setSingleShot(false);
        resendTimeoutTimer->setInterval(1);
        resendTimeoutTimer->start();

        responsePackets.push_back(udpTunnelPacket);
    }

    // qDebug() << "End buffer size:" << this->buffer.size();
    // qDebug() << "New chunks size:" << responsePackets.size();

    return responsePackets;
}