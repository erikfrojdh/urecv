#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "ZmqSocket.h"
#include <string>
#include <atomic>
#include <thread>
#include <fmt/format.h>
namespace urecv {

class Streamer {
    ImageFifo *fifo_;
    ZmqSocket socket_;
    std::atomic<bool> stopped_{false};

  public:
    Streamer(const std::string &endpoint, ImageFifo *fifo)
        : fifo_(fifo), socket_(endpoint) {
            fmt::print(fg(fmt::color::gold), "Streamer()\n");
        }
    void stream(){
        while (!stopped_){
            ImageView img;
            if(fifo_->try_pop_image(img)){
                socket_.send(img, fifo_->image_size());
                fmt::print(fg(fmt::color::gold), "Streamed out frame {}\n", img.frameNumber);
                fifo_->push_free(img);
            }
            std::this_thread::sleep_for(DEFAULT_WAIT);
        }
    }
    void stop(){
        stopped_ = true;
    }
};
} // namespace urecv