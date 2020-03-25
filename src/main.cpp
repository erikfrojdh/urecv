#include "Receiver.h"
#include <fmt/format.h>
#include <fmt/color.h>
#include <thread>

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    try {
        auto [node, port] = parse_args(argc, argv);
        Receiver r;
        std::thread receive_thread(&Receiver::ReceivePackets, &r, node, port);
        std::thread stream_thread(&Receiver::StreamImages, &r, "tcp://*:4545");
        receive_thread.join();
        stream_thread.join();
    }catch(const std::runtime_error& e){
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}