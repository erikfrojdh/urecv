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
// constexpr int starting_port = 50001;

int main(int argc, char *argv[]) {
    ur::direct_input();
    try {
        // Create receivers
        std::vector<std::unique_ptr<ur::Receiver>> receivers;
        receivers.push_back(ur::make_unique<ur::Receiver>(
                "10.1.2.160", "50020"));
        receivers.push_back(ur::make_unique<ur::Receiver>(
                "10.1.1.160", "50021"));
        

        // Start listening threads
        int cpu = 0;
        std::vector<std::thread> threads;
        for (auto &r : receivers) {
            threads.emplace_back(&ur::Receiver::receivePackets, r.get(), cpu++);
        }

        ur::FrameAssembler<256,768> assembler(receivers);
        threads.emplace_back(&ur::FrameAssembler<256,768>::assemble, &assembler, cpu++);

        ur::Streamer streamer(endpoint, assembler.fifo());
        threads.emplace_back(&ur::Streamer::stream, &streamer, cpu++);

        fmt::print("Frame size: {} bytes\n", assembler.frame_size());

        // ur::Writer writer(assembler.fifo());
        // threads.emplace_back(&ur::Writer::write, &writer, cpu++);

        // Listen for 'q'
        while (true) {
            auto key = std::cin.get();
            if (key == 'q') {
                fmt::print(fg(fmt::color::red), "Recived \'q\' aborting!\n");
                for (auto &r : receivers)
                    r->stop();
                assembler.stop();
                streamer.stop();
                // writer.stop();
                break;
            }
        }

        for (auto &&t : threads)
            t.join();

    } catch (const std::runtime_error &e) {
        fmt::print(fg(fmt::color::red), "ERROR: {}\n", e.what());
    }
    fmt::print(fg(fmt::color::hot_pink), "Bye!\n");
    ur::reset_terminal();
}
