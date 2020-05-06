#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "UdpSocket.h"
#include "defs.h"
#include "utils.h"
#include <memory>

namespace ur {

class Receiver {
    ImageFifo fifo_;
    std::atomic<bool> stopped_{false};
    std::unique_ptr<UdpSocket> sock;

  public:
    Receiver(const std::string &node, const std::string &port);
    Receiver(const std::string &node, const std::string &port,
             size_t fifo_size);
    void receivePackets(int cpu);
    void stop();
    ImageFifo *fifo() { return &fifo_; }
};

} // namespace ur
