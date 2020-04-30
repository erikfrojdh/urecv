#pragma once
#include "ImageView.h"
#include "SimpleQueue.h"
#include <memory>
#include <chrono>
#include <fmt/format.h>
#include <fmt/color.h>
#include <thread>

//template parameter for aligned alloc????
class ImageFifo {
    size_t fifo_size_;
    size_t image_size_;
    SimpleQueue<ImageView> free_slots;
    SimpleQueue<ImageView> filled_slots;
    char *data;

  public:
    ImageFifo(size_t fifo_size, size_t image_size)
        : fifo_size_(fifo_size), image_size_(image_size),
          free_slots(fifo_size_), filled_slots(fifo_size_),
          data(new char[fifo_size_ * image_size_]) {

        ImageView v;
        for (size_t i = 0; i < fifo_size_; ++i) {
            v.data = data + i * image_size_;
            free_slots.push(v);
        }
    }
    ~ImageFifo() { delete[] data; }

    size_t size() const noexcept { return fifo_size_; }
    size_t image_size() const noexcept { return image_size_; }
    size_t numFilledSlots() const noexcept { return filled_slots.sizeGuess(); }
    size_t numFreeSlots() const noexcept { return free_slots.sizeGuess(); }

    ImageView pop_free() {
        ImageView v;
        while (!free_slots.pop(v))
            ;
        return v;
    }

    ImageView pop_image() {
        ImageView v;
        while (!filled_slots.pop(v))
            ;
        return v;
    }
    ImageView pop_image(std::chrono::nanoseconds wait) {
        ImageView v;
        while (!filled_slots.pop(v)){
            std::this_thread::sleep_for(wait);
        }
        return v;
    }

    ImageView pop_image(std::chrono::nanoseconds wait, std::atomic<bool>& stopped) {
        ImageView v;
        while (!filled_slots.pop(v) && !stopped){
            std::this_thread::sleep_for(wait);
        }
        if(stopped)
            fmt::print(fg(fmt::color::rosy_brown), "STOPPED!\n");
        return v;
    }

    bool try_pop_image(ImageView& img){
        return filled_slots.pop(img);
    }

    void push_image(ImageView &v) {
        while (!filled_slots.push(v))
            ;
    }

    void push_free(ImageView &v) {
        while (!free_slots.push(v))
            ;
    }

};