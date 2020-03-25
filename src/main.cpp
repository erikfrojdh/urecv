#include "Receiver.h"
#include <fmt/format.h>
#include <thread>

// usage ./urecv 192.162.1.1:50001
int main(int argc, char *argv[]) {
    auto [node, port] = parse_args(argc, argv);
    Receiver r;
    std::thread receive_thread(&Receiver::ReceivePackets, &r, node, port);
    std::thread stream_thread(&Receiver::StreamImages, &r, "tcp://*:4545");
    receive_thread.join();
    stream_thread.join();
}