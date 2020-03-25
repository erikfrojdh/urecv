// our
#include "SimpleQueue.h"
#include "Streamer.h"
#include "UdpSocket.h"
#include "defs.h"
#include "utils.h"

// std
#include <fmt/format.h>
#include <string>
#include <string_view>
#include <thread>

constexpr size_t queueSize = 100;
SimpleQueue<Image> freeQueue(queueSize);
SimpleQueue<Image> dataQueue(queueSize);
std::byte *data = new std::byte[frame_size * queueSize];

void fill_queue(SimpleQueue<Image> *queue) {
    Image img{-1, nullptr};
    std::fill_n(data, frame_size * queueSize, std::byte(0));
    for (size_t i = 0; i < queueSize; ++i) {
        img.data = data + i * frame_size;
        queue->push(img);
    }
}

void receive_packets(const std::string &node, const std::string &port) {
    std::byte packet_buffer[packet_size];
    PacketHeader header{};
    Image img;
    UdpSocket sock(node, port, packet_size);
    sock.ReceivePacket(packet_buffer, header);
    uint64_t currentFrameNumber = header.frameNumber;
    int numPacketsReceived = 0;
    while (true) {
        freeQueue.pop(img);
        img.frameNumber = currentFrameNumber;
        fmt::print("Start frame: {}\n", currentFrameNumber);
        while (true) {
            memcpy(img.data + payload_size * numPacketsReceived,
                   packet_buffer + sizeof(PacketHeader), payload_size);
            ++numPacketsReceived;
            sock.ReceivePacket(packet_buffer, header);
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if (numPacketsReceived != packetsPerFrame) {
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber,
                       128 - numPacketsReceived);
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0;
        dataQueue.push(img);
        fmt::print("free: {} data: {}\n", freeQueue.sizeGuess(),
                   dataQueue.sizeGuess());
    }
}

void stream_images(const std::string &endpoint) {
    Image img;
    Streamer strm(endpoint);
    while (true) {
        while (!dataQueue.pop(img))
            ;
        strm.send(img, frame_size);
        fmt::print("Streamed img {}\n", img.frameNumber);
        while (!freeQueue.push(img))
            ;
    }
}

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    auto [node, port] = parse_args(argc, argv);

    fill_queue(&freeQueue);
    fmt::print("size: {}\n", freeQueue.sizeGuess());

    std::thread receiveThread(receive_packets, node, port);
    std::thread streamThread(stream_images, "tcp://*:4545");
    receiveThread.join();
    streamThread.join();
    // clean up before exit
}