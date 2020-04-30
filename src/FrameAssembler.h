#pragma once
#include "ImageFifo.h"
#include "ImageView.h"
#include "Receiver.h"
#include "utils.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>
#include <vector>

namespace urecv {

class FrameAssembler {
    std::vector<ImageFifo *> fifos_;
    std::vector<ImageView> images_;
    std::atomic<bool> stopped_{false};
    ImageFifo assembled_images_;

  public:
    FrameAssembler(const std::vector<std::unique_ptr<Receiver>> &rec)
        : assembled_images_(10, rec.size() * FRAME_SIZE) {
        for (auto &r : rec) {
            fifos_.push_back(r->fifo());
        }
        images_.resize(fifos_.size());
        fmt::print(fg(fmt::color::hot_pink), "FrameAssembler: {}, {}\n",
                   fifos_.size(), images_.size());
    }

    ImageFifo *fifo() { return &assembled_images_; }

    void stop() { stopped_ = true; }

    void assemble(int cpu) {
        pin_this_thread(cpu);
        while (!stopped_) {
            // pop one image from all, this will block
            for (int i = 0; i < images_.size(); ++i) {
                images_[i] = fifos_[i]->pop_image();
            }

            // ensure that we are in sync
            auto current_frame = images_[0].frameNumber;
            for (auto &im : images_)
                if (im.frameNumber != current_frame)
                    throw std::runtime_error("FRAME NUMBERS DIFFER!");

            fmt::print(fg(fmt::color::hot_pink),
                       "Assembled frame {}, frames in fifo: {}\n",
                       current_frame, fifos_[0]->numFilledSlots());

            //Assemble the image
            auto full_image = assembled_images_.pop_free();
            full_image.frameNumber = current_frame;
            auto part_size = fifos_[0]->image_size();
            for (size_t i = 0; i<images_.size(); ++i){
                std::copy_n(images_[i].data, part_size, full_image.data+i*part_size );
            }
            assembled_images_.push_image(full_image);



            // put back in free q
            for (int i = 0; i < images_.size(); ++i) {
                fifos_[i]->push_free(images_[i]);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};
} // namespace urecv