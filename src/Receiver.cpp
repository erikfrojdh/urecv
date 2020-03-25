#include "Receiver.h"
#include "Streamer.h"
#include "UdpSocket.h"
#include <fmt/format.h>

Receiver::Receiver(size_t frame_queue_size)
    : frame_queue_size_(frame_queue_size), freeQueue_(frame_queue_size_),
      dataQueue_(frame_queue_size_),
      data_(new std::byte[FRAME_SIZE * frame_queue_size_]) {
    fillFreeQueue();
}

Receiver::~Receiver(){
    delete[] data_;
}

void Receiver::fillFreeQueue() {
    Image img{-1, nullptr};
    std::fill_n(data_, FRAME_SIZE * frame_queue_size_, std::byte(0));
    for (size_t i = 0; i < frame_queue_size_; ++i) {
        img.data = data_ + i * FRAME_SIZE;
        freeQueue_.push(img);
    }
}

void Receiver::ReceivePackets(const std::string &node, const std::string &port) {
    pin_this_thread(1);
    set_realtime_priority();
    fmt::print("Listening to: {}:{}\n", node, port);
    std::byte packet_buffer[PACKET_SIZE];
    PacketHeader header{};
    Image img;
    UdpSocket sock(node, port, PACKET_SIZE);
    sock.setBufferSize(DEFAULT_UDP_BUFFER_SIZE);
    fmt::print("UDP buffer size: {} MB\n", sock.bufferSize()/(1024.*1024.));
    sock.ReceivePacket(packet_buffer, header);
    uint64_t currentFrameNumber = header.frameNumber;
    int numPacketsReceived = 0;
    while (true) {
        while (!freeQueue_.pop(img))
            ;
        img.frameNumber = currentFrameNumber;
        while (true) {
            memcpy(img.data + PAYLOAD_SIZE * numPacketsReceived,
                   packet_buffer + sizeof(PacketHeader), PAYLOAD_SIZE);
            ++numPacketsReceived;
            sock.ReceivePacket(packet_buffer, header);
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if (numPacketsReceived != PACKETS_PER_FRAME) {
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber,
                       PACKETS_PER_FRAME - numPacketsReceived);
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0;
        while (!dataQueue_.push(img))
            ;
        // fmt::print("free: {} data: {}\n", freeQueue.sizeGuess(),
        //            dataQueue.sizeGuess());
    }
}

void Receiver::StreamImages(const std::string &endpoint) {
    pin_this_thread(2);
    Image img;
    Streamer strm(endpoint);
    while (true) {
        while (!dataQueue_.pop(img))
            ;
        strm.send(img, FRAME_SIZE);
        fmt::print("Streamed img {}\n", img.frameNumber);
        while (!freeQueue_.push(img))
            ;
    }
}