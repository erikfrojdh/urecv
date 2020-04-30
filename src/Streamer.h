#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "ZmqSocket.h"
#include <atomic>
#include <fmt/format.h>
#include <string>
#include <thread>
namespace urecv {

class Streamer {
    ImageFifo *fifo_;
    ZmqSocket socket_;
    std::atomic<bool> stopped_{false};

  public:
    Streamer(const std::string &endpoint, ImageFifo *fifo)
        : fifo_(fifo), socket_(endpoint) {}
    void stream(int cpu) {
        pin_this_thread(cpu);
        while (!stopped_) {
            ImageView img;
            if (fifo_->try_pop_image(img)) {
                socket_.send(img, fifo_->image_size());
                if (img.frameNumber % PRINT_MOD == 0)
                    fmt::print(fg(fmt::color::gold), "Streamed out frame {}\n",
                               img.frameNumber);
                fifo_->push_free(img);
            }
            std::this_thread::sleep_for(DEFAULT_WAIT);
        }
    }
    void stop() { stopped_ = true; }
};
} // namespace urecv