#include "Receiver.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

#include "utils.h"
#include <iostream>
#include <vector>


// usage
// ./urecv 192.162.1.1:50001
// ./urecv 192.162.1.1:50001 -s tcp://*:4545
// ./urecv 192.162.1.1:50001 -w test

int main(int argc, char *argv[]) {
    direct_input();
    try {
        auto [node, port, endpoint, fname] = parse_args(argc, argv);
        Receiver r(node, port);
        std::vector<std::thread> threads;
        threads.emplace_back(&Receiver::receivePackets, &r, 0);
        if (!endpoint.empty() && fname.empty()) {
            threads.emplace_back(&Receiver::streamImages, &r, endpoint, 1);
        } else if (endpoint.empty() && !fname.empty()) {
            threads.emplace_back(&Receiver::writeImages, &r, fname, 1);
        } else if (endpoint.empty() && fname.empty()) {
            threads.emplace_back(&Receiver::zeroImages, &r, 1);
        } else {
            throw std::runtime_error("Something went wrong with the parsing");
        }

        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Stopping receiver\n");
                r.finish();
                break;
            }
        }
        for (auto &&t : threads)
            t.join();
    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }

    fmt::print(fg(fmt::color::chartreuse), "Bye!\n");
    reset_terminal();
}