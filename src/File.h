#pragma once

#include "defs.h"
#include "utils.h"
#include <string>

#include <fmt/color.h>
#include <fmt/format.h>

template <typename T> class File {
    static constexpr auto writer_color = fmt::color::green;
    T writerImpl;

    int64_t *meta_;
    size_t n_written_ = 0;
    size_t frames_per_file_ = 1000;
    size_t file_nr_ = 0;
    size_t meta_size_;
    std::string basename_;

    void allocate_meta() {
        auto bytes_needed =
            frames_per_file_ * sizeof(*meta_) +
            2 * sizeof(*meta_); // number of frames + n_frames and footer_size
        meta_size_ =
            bytes_needed + (IO_ALIGNMENT - bytes_needed % IO_ALIGNMENT);
        fmt::print("Allocating {} bytes for footer\n", meta_size_);
        meta_ = static_cast<int64_t *>(
            std::aligned_alloc(IO_ALIGNMENT, meta_size_));
    }

    std::string currentFname() {
        return fmt::format("{}_{}.bin", basename_, file_nr_);
    }

    void open(const std::string &fname) {
        fmt::print(fg(writer_color), "Opening: {} for writing\n",
                   currentFname());
        writerImpl.open(fname);
        memset(meta_, 0, meta_size_); // clear out old meta data
    }

    void close() {
        fmt::print(fg(writer_color), "Closing: {}\n", currentFname());
        write_meta();
        n_written_ = 0;
        writerImpl.close();
    }
    void write_meta() {
        auto npos = meta_size_ / sizeof(*meta_);
        fmt::print("npos: {}\n", npos);
        meta_[npos - 2] = n_written_;
        meta_[npos - 1] = meta_size_;
        writerImpl.write(meta_, meta_size_);
    }

  public:
    File(const std::string &basename) : File(basename, 1000) {}
    File(const std::string &basename, size_t frames_per_file)
        : frames_per_file_(frames_per_file), basename_(basename) {
        allocate_meta();
        writerImpl.open(currentFname());
    }
    ~File() {
        close();
        free(meta_);
    }

    void write(const Image &img) {
        if (n_written_ == frames_per_file_) {
            close();
            open(fmt::format("{}_{}.bin", basename_, ++file_nr_));
        }
        meta_[n_written_++] = img.frameNumber;
        writerImpl.write(img.data, FRAME_SIZE);
    }
};