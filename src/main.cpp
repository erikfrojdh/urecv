#include "FrameAssembler.h"
#include "Receiver.h"
#include "Streamer.h"
#include "Writer.h"
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

int main(int argc, char *argv[]) {
    direct_input();
    try {
        // Create receivers
        std::vector<std::unique_ptr<Receiver>> receivers;
        for (int i = 0; i < 4; ++i) {
            receivers.push_back(ur::make_unique<Receiver>(
                node, std::to_string(starting_port + i)));
        }

        ur::FrameAssembler assembler(receivers);

        // Start listening threads
        int cpu = 0;
        std::vector<std::thread> threads;
        for (auto &r : receivers) {
            threads.emplace_back(&Receiver::receivePackets, r.get(), cpu++);
        }
        threads.emplace_back(&ur::FrameAssembler::assemble, &assembler, cpu++);

        // ur::Streamer streamer(endpoint, assembler.fifo());
        // threads.emplace_back(&ur::Streamer::stream, &streamer, cpu++);

        ur::Writer writer(assembler.fifo());
        threads.emplace_back(&ur::Writer::write, &writer, cpu++);

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Recived \'q\' aborting!\n");
                for (auto &r : receivers)
                    r->finish();
                assembler.stop();
                // streamer.stop();
                writer.stop();
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