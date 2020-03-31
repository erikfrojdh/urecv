#include "Receiver.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    try {
        auto [node, port, endpoint] = parse_args(argc, argv);
        Receiver r;
        std::thread receive_thread(&Receiver::receivePackets, &r, node, port);
        std::thread stream_thread(&Receiver::writeImages, &r, "test");
        // std::thread stream_thread(&Receiver::streamImages, &r, endpoint);
        receive_thread.join();
        stream_thread.join();
    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
}