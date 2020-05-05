#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "UdpSocket.h"
#include "defs.h"
#include "utils.h"
#include <memory>

class Receiver {
    ImageFifo fifo_;
    std::atomic<bool> stopped_{false};
    std::atomic<bool> receiver_done_{false}; // watch out for cache lines
    std::unique_ptr<UdpSocket> sock;
    void fillFreeQueue();

  public:
    Receiver(const std::string &node, const std::string &port);

    Receiver(const std::string &node, const std::string &port,
             size_t fifo_size);
    ~Receiver();
    void receivePackets(int cpu);
    void finish();
    ImageFifo *fifo() { return &fifo_; }
};
