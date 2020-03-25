
#include "Streamer.h"
#include <zmq.h>
#include <fmt/format.h>
#include <stdexcept>
#include <thread>
using namespace std::literals;
Streamer::Streamer(const std::string &endpoint) {
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PUB);
    if (zmq_bind(socket, endpoint.c_str()))
        throw std::runtime_error("Could not bind endpoint");

    std::this_thread::sleep_for(1s);
    fmt::print("Publishing data on on: {}\n", endpoint);
}

void Streamer::send(Image& img, size_t data_size){
    int sent = zmq_send (socket, &img.frameNumber, sizeof(img.frameNumber), ZMQ_SNDMORE);
    sent += zmq_send (socket, img.data, data_size, 0);
    if (sent!=(data_size+sizeof(img.frameNumber))){
        auto msg = fmt::format("Failed to send image. Sent {}  of bytes", sent);
        throw std::runtime_error(msg);
    }       
}

Streamer::~Streamer() {
    zmq_close(socket);
    zmq_ctx_destroy(context);
}