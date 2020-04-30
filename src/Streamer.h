#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "ZmqSocket.h"
#include <string>
#include <atomic>
namespace urecv {
class Streamer {
    ImageFifo *fifo_;
    ZmqSocket socket_;
    std::atomic<bool> stopped_;

  public:
    Streamer(const std::string &endpoint, ImageFifo *fifo)
        : fifo_(fifo), socket_(endpoint) {}
    void stream(){

        while (!stopped_){
            ImageView img = fifo_->pop_image();
            socket_.send(img, fifo_->image_size());
            fifo_->push_free(img);
        }

    }
    void stop(){
        stopped_ = true;
    }
};
} // namespace urecv