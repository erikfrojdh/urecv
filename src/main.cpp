#include "FrameAssembler.h"
#include "Streamer.h"
#include "Receiver.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>

#include "RxMemory.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <vector>

constexpr auto node = "192.168.122.1";
constexpr auto endpoint = "tcp://*:4545";
constexpr int starting_port = 50001;
constexpr int starting_cpu = 0;

int main(int argc, char *argv[]) {
    direct_input();
    try {
        // Create receivers
        std::vector<std::unique_ptr<Receiver>> receivers;
        for (int i = 0; i < 2; ++i) {
            receivers.push_back(urecv::make_unique<Receiver>(
                node, std::to_string(starting_port + i)));
        }

        urecv::FrameAssembler assembler(receivers);
        urecv::Streamer(endpoint, assembler.fifo());

        // Start listening threads
        std::vector<std::thread> threads;
        for (int i = 0; i < receivers.size(); ++i) {
            threads.emplace_back(&Receiver::receivePackets, receivers[i].get(),
                                 starting_cpu + i);
        }

        threads.emplace_back(&urecv::FrameAssembler::assemble, &assembler);

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Stopping receivers\n");
                for (auto &r : receivers)
                    r->finish();
                assembler.stop();
                break;
            }
        }

        for (auto &&t : threads)
            t.join();
    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }

    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
    reset_terminal();
}