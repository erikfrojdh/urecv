

#include "DirectWriter.h"
#include "File.h"
#include "FwriteWriter.h"
#include "ImageFifo.h"
#include "ImageView.h"
#include "defs.h"
#include "utils.h"

#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <fmt/format.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    std::string fname{"test"};
    const size_t n_frames = 50000;
    const size_t image_size = 1024 * 512; // 1M
    ur::ImageFifo fifo(n_frames, image_size);
    for (size_t i = 0; i < n_frames; ++i) {
        auto img = fifo.pop_free();
        std::fill_n(img.data, image_size, 0);
        fifo.push_image(img);
    }

    ur::File<DirectWriter> f(fname, n_frames, image_size);
    fmt::print("Writing: {} frames to {}\n", n_frames, f.currentFname());
    auto t0 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < n_frames; ++i) {
        auto img = fifo.pop_image();
        f.write(img);
    }
    std::chrono::duration<double, std::milli> dt(
        std::chrono::steady_clock::now() - t0);
    auto mbps = (image_size * n_frames / 1024 / 1024) / (dt.count() / 1000.);
    fmt::print("Write took: {} ms, speed: {} MB/s {} FPS\n", dt.count(), mbps, n_frames/(dt.count() / 1000.));
}