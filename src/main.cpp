#include "Receiver.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

#include "utils.h"
#include <iostream>

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    direct_input();
    try {
        auto [node, port, endpoint] = parse_args(argc, argv);
        Receiver r(node, port);
        std::thread receive_thread(&Receiver::receivePackets, &r, 1);
        std::thread process_thread(&Receiver::writeImages, &r, "test");
        // std::thread stream_thread(&Receiver::streamImages, &r, endpoint);

        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Stopping receiver\n");
                r.finish();
                break;
            }
        }
        receive_thread.join();
        process_thread.join();
    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }

    fmt::print(fg(fmt::color::azure), "Bye!\n");
}