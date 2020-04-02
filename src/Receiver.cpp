#include "Receiver.h"
#include "File.h"
#include "DirectWriter.h"
#include "FwriteWriter.h"
#include "Streamer.h"
#include "UdpSocket.h"
#include <chrono>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

using namespace std::literals;

Receiver::Receiver(const std::string &node, const std::string &port,
                   size_t frame_queue_size)
    : frame_queue_size_(frame_queue_size), free_queue_(frame_queue_size_),
      data_queue_(frame_queue_size_) {
    sock = std::make_unique<UdpSocket>(node, port, PACKET_SIZE);
    fmt::print("Listening to: {}:{}\n", node, port);
    sock->setBufferSize(DEFAULT_UDP_BUFFER_SIZE);
    fmt::print("UDP buffer size: {} MB\n",
               sock->bufferSize() / (1024. * 1024.));

    // O_DIRECT needs aligned data
    data_ = static_cast<std::byte *>(
        std::aligned_alloc(IO_ALIGNMENT, FRAME_SIZE * frame_queue_size_));
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

void Receiver::receivePackets(int cpu) {
    pin_this_thread(cpu);
    set_realtime_priority();
    std::byte packet_buffer[PACKET_SIZE];
    PacketHeader header{};
    Image img;
    sock->receivePacket(packet_buffer, header); // waits here for data
    uint64_t currentFrameNumber = header.frameNumber;
    int numPacketsReceived = 0;
    int64_t totalPacketsLost = 0;
    int64_t totalFramesReceived = 0;
    while (!stopped_) {
        while (!free_queue_.pop(img))
            ;
        img.frameNumber = currentFrameNumber;
        while (!stopped_) {
            memcpy(img.data + PAYLOAD_SIZE * header.packetNumber,
                   packet_buffer + sizeof(PacketHeader), PAYLOAD_SIZE);
            ++numPacketsReceived;
            sock->receivePacket(packet_buffer, header); // waits here for data
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if (numPacketsReceived != PACKETS_PER_FRAME) {
            auto lost = PACKETS_PER_FRAME - numPacketsReceived;
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber,
                       lost);
            totalPacketsLost += lost;
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0;
        while (!data_queue_.push(img))
            ;

        totalFramesReceived += 1;
        if(totalFramesReceived%1000 == 0){
            fmt::print("Received: {} frames, lost {} packets\n", totalFramesReceived, totalPacketsLost);
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(
        1000)); // make sure we have time to sink images
    receiver_done_ = true;
    fmt::print("UDP thread done\n");
}

void Receiver::streamImages(const std::string &endpoint, int cpu) {
    pin_this_thread(cpu);
    Image img;
    Streamer strm(endpoint);
    while (!receiver_done_) {
        if (data_queue_.pop(img)) {
            strm.send(img, FRAME_SIZE);
            free_queue_.push(img);
            fmt::print("Streamed img {}\n", img.frameNumber);
        } else {
            std::this_thread::sleep_for(100us);
        }
    }
}

void Receiver::writeImages(const std::string &basename, int cpu) {
    pin_this_thread(cpu);
    Image img;
    File<FwriteWriter> writer(basename);
    while (!receiver_done_) {
        if (data_queue_.pop(img)) {
            writer.write(img);
            free_queue_.push(img); // no need to loop since size should be ok
        } else {
            std::this_thread::sleep_for(100us);
        }
    }
    fmt::print(fg(fmt::color::green), "Writer stopped\n");
}

void Receiver::zeroImages(int cpu) {
    pin_this_thread(cpu);
    Image img;
    while (!receiver_done_) {
        if (data_queue_.pop(img)) {
            img.frameNumber = 0;
            memset(img.data, 0, FRAME_SIZE);
            free_queue_.push(img); // no need to loop since size should be ok
        } else {
            std::this_thread::sleep_for(100us);
        }
    }
    fmt::print(fg(fmt::color::green), "Zero stopped\n");
}

void Receiver::finish() {
    stopped_ = true;
    sock->shutdown();
}