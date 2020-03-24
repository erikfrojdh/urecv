// our
#include "UdpSocket.h"
#include "defs.h"

// std
#include <charconv>
#include <fmt/format.h>
#include <string>
#include <string_view>

std::pair<std::string, std::string> parse_args(int argc, char *argv[]) {
    std::pair<std::string, std::string> res;
    if (argc != 2) {
        throw std::runtime_error("Wrong number of arguments\n");
    }
    std::string_view arg(argv[1]);
    if (auto pos = arg.find(':'); pos != std::string_view::npos) {
        res.first = arg.substr(0, pos);
        res.second = arg.substr(pos + 1);
    }
    fmt::print("Node: {} port: {}\n", res.first, res.second);
    return res;
}

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    auto [node, port] = parse_args(argc, argv);
    UdpSocket sock(node, port, packet_size);

    std::byte packet_buffer[packet_size];
    auto frame_buffer = std::make_unique<std::byte[]>(frame_size);
    PacketHeader header{};

    // receive packets

    sock.ReceivePacket(packet_buffer, header);
    uint64_t currentFrameNumber = 0UL;
    int numPacketsReceived = header.frameNumber;
    while (true) {
        fmt::print("Start frame: {}\n", currentFrameNumber);
        while (true) {
            memcpy(frame_buffer.get() + payload_size * numPacketsReceived,
                   packet_buffer + sizeof(PacketHeader), payload_size);
            ++numPacketsReceived;
            sock.ReceivePacket(packet_buffer, header);
            // fmt::print("Frame {}, packet: {}\n", header.frameNumber,
            //            header.packetNumber);
            if (currentFrameNumber != header.frameNumber)
                break;
        }
        if(numPacketsReceived!=128){
            fmt::print("Frame: {} lost {} pkts\n", currentFrameNumber, 128-numPacketsReceived);
        }
        currentFrameNumber = header.frameNumber;
        numPacketsReceived = 0; 
    }

    // clean up before exit
}