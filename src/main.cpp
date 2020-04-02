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
        auto args = parse_args(argc, argv);
        Receiver r(args.node, args.port);
        fmt::print("Pinning to: {}, and {}\n", args.cpu0, args.cpu1);
        std::vector<std::thread> threads;
        threads.emplace_back(&Receiver::receivePackets, &r, args.cpu0);
        if (!args.endpoint.empty() && args.fname.empty()) {
            threads.emplace_back(&Receiver::streamImages, &r, args.endpoint, args.cpu1);
        } else if (args.endpoint.empty() && !args.fname.empty()) {
            threads.emplace_back(&Receiver::writeImages, &r, args.fname, args.cpu1);
        } else if (args.endpoint.empty() && args.fname.empty()) {
            threads.emplace_back(&Receiver::zeroImages, &r, args.cpu1);
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