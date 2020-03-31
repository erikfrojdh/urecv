#include "Receiver.h"
#include "Streamer.h"
#include "FileWriterDirect.h"
#include "UdpSocket.h"
#include <chrono>
#include <fmt/format.h>
#include <thread>
#include <cstdlib>

Receiver::Receiver(size_t frame_queue_size)
    : frame_queue_size_(frame_queue_size), free_queue_(frame_queue_size_),
      data_queue_(frame_queue_size_) {

    //O_DIRECT needs aligned data 
    data_ = static_cast<std::byte*>(std::aligned_alloc(IO_ALIGNMENT, FRAME_SIZE*frame_queue_size_));
    fillFreeQueue();
}

Receiver::~Receiver() { delete[] data_; }

void Receiver::fillFreeQueue() {
    Image img{-1, nullptr};
    std::fill_n(data_, FRAME_SIZE * frame_queue_size_, std::byte(0));
    for (size_t i = 0; i < frame_queue_size_; ++i) {
        img.data = data_ + i * FRAME_SIZE;
        free_queue_.push(img);
    }
}

void Receiver::receivePackets(const std::string &node,
                              const std::string &port) {
    pin_this_thread(1);
    set_realtime_priority();
    fmt::print("Listening to: {}:{}\n", node, port);
    std::byte packet_buffer[PACKET_SIZE];
    PacketHeader header{};
    Image img;
    UdpSocket sock(node, port, PACKET_SIZE);
    sock.setBufferSize(DEFAULT_UDP_BUFFER_SIZE);
    fmt::print("UDP buffer size: {} MB\n", sock.bufferSize() / (1024. * 1024.));
    sock.receivePacket(packet_buffer, header);
    uint64_t currentFrameNumber = header.frameNumber;
    int numPacketsReceived = 0;
    while (true) {
        while (!free_queue_.pop(img))
            ;
        img.frameNumber = currentFrameNumber;
        while (true) {
            memcpy(img.data + PAYLOAD_SIZE * numPacketsReceived,
                   packet_buffer + sizeof(PacketHeader), PAYLOAD_SIZE);
            ++numPacketsReceived;
            sock.receivePacket(packet_buffer, header);
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if (numPacketsReceived != PACKETS_PER_FRAME) {
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber,
                       PACKETS_PER_FRAME - numPacketsReceived);
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0;
        while (!data_queue_.push(img))
            ;
    }
}

void Receiver::streamImages(const std::string &endpoint) {
    pin_this_thread(2);
    Image img;
    Streamer strm(endpoint);
    while (true) {
        while (!data_queue_.pop(img))
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        strm.send(img, FRAME_SIZE);
        fmt::print("Streamed img {}\n", img.frameNumber);
        while (!free_queue_.push(img))
            ;
    }
}

void Receiver::writeImages(const std::string &basename) {
    pin_this_thread(2);
    Image img;
    FileWriterDirect writer(basename);
    while (true) {
        while (!data_queue_.pop(img))
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        writer.write(img);
        while (!free_queue_.push(img))
            ;
    }
}