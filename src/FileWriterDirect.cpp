#include "FileWriterDirect.h"
#include <fcntl.h>
#include <fmt/format.h>
#include <stdexcept>
#include <unistd.h>

FileWriterDirect::FileWriterDirect(const std::string &basename)
    : basename_(basename) {
    allocate_meta();
    open(fmt::format("{}_{}.bin", basename_, file_nr_));
}

FileWriterDirect::~FileWriterDirect() {
    write_meta();
    ::close(fd_);
    free(meta_);
}

void FileWriterDirect::open(const std::string &fname) {
    fd_ = ::open(fname.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_DIRECT, S_IRWXU | S_IROTH);
    if (fd_ == -1) {
        throw std::runtime_error(fmt::format("fderror: {}\n", strerror(errno)));
    }
    memset(meta_, 0, meta_size_); // clear out old meta data
}

void FileWriterDirect::allocate_meta() {
    auto bytes_needed =
        frames_per_file_ * sizeof(*meta_) +
        2 * sizeof(*meta_); // number of frames + n_frames and footer_size
    meta_size_ = bytes_needed + (IO_ALIGNMENT - bytes_needed % IO_ALIGNMENT);
    fmt::print("Allocating {} bytes for footer\n", meta_size_);
    meta_ =
        static_cast<int64_t *>(std::aligned_alloc(IO_ALIGNMENT, meta_size_));
}

void FileWriterDirect::write_meta() {
    auto npos = meta_size_ / sizeof(*meta_);
    meta_[npos - 2] = n_written_;
    meta_[npos - 1] = meta_size_;
    ::write(fd_, meta_, meta_size_);
}

void FileWriterDirect::write(const Image &img) {

    if (n_written_ == frames_per_file_) {
        write_meta();
        ::close(fd_);
        n_written_ = 0;
        open(fmt::format("{}_{}.bin", basename_, ++file_nr_));
    }

    meta_[n_written_++] = img.frameNumber;
    auto rc = ::write(fd_, img.data, FRAME_SIZE);
    if (rc != FRAME_SIZE) {
        throw std::runtime_error(
            fmt::format("ERROR: Wrote {} with error {}", rc, errno));
    }
}
