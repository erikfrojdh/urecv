#include "FileWriterDirect.h"
#include <fcntl.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <unistd.h>

FileWriterDirect::FileWriterDirect(const std::string& basename):
FileWriter(basename){}

FileWriterDirect::~FileWriterDirect(){
    close_impl();
}

void FileWriterDirect::write_impl(void *buf, size_t len) {
    int rc = ::write(fd_, buf, len);
    if (rc != len) {
        throw std::runtime_error("Failed write");
    }
}

void FileWriterDirect::open_impl(const std::string &fname) {
    fd_ = ::open(fname.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_DIRECT,
                 S_IRWXU | S_IROTH);
    if (fd_ == -1) {
        throw std::runtime_error(fmt::format("fderror: {}\n", strerror(errno)));
    }
}

void FileWriterDirect::close_impl() { ::close(fd_); }

// FileWriterDirect::FileWriterDirect(const std::string &basename)
//     : basename_(basename) {
//     allocate_meta();
//     open(currentFname());
// }

// FileWriterDirect::~FileWriterDirect() {
//     close();
//     ::close(fd_);
//     free(meta_);
// }

// std::string FileWriterDirect::currentFname(){
//     return fmt::format("{}_{}.bin", basename_, file_nr_);
// }

// void FileWriterDirect::open(const std::string &fname) {
//     fmt::print(fg(fmt::color::green), "Opening: {} for writing",
//     currentFname()); fmt::print("\n"); fd_ = ::open(fname.c_str(), O_RDWR |
//     O_CREAT | O_TRUNC | O_DIRECT, S_IRWXU | S_IROTH); if (fd_ == -1) {
//         throw std::runtime_error(fmt::format("fderror: {}\n",
//         strerror(errno)));
//     }
//     memset(meta_, 0, meta_size_); // clear out old meta data
// }

// void FileWriterDirect::close(){
//     fmt::print(fg(fmt::color::green), "Closing: {}\n", currentFname());
//     write_meta();
//     n_written_ = 0;
//     ::close(fd_);
// }

// void FileWriterDirect::allocate_meta() {
//     auto bytes_needed =
//         frames_per_file_ * sizeof(*meta_) +
//         2 * sizeof(*meta_); // number of frames + n_frames and footer_size
//     meta_size_ = bytes_needed + (IO_ALIGNMENT - bytes_needed % IO_ALIGNMENT);
//     fmt::print("Allocating {} bytes for footer\n", meta_size_);
//     meta_ =
//         static_cast<int64_t *>(std::aligned_alloc(IO_ALIGNMENT, meta_size_));
// }

// void FileWriterDirect::write_meta() {
//     auto npos = meta_size_ / sizeof(*meta_);
//     meta_[npos - 2] = n_written_;
//     meta_[npos - 1] = meta_size_;
//     ::write(fd_, meta_, meta_size_);
// }

// void FileWriterDirect::write(const Image &img) {
//     if (n_written_ == frames_per_file_) {
//         close();
//         open(fmt::format("{}_{}.bin", basename_, ++file_nr_));
//     }
//     meta_[n_written_++] = img.frameNumber;
//     auto rc = ::write(fd_, img.data, FRAME_SIZE);
//     if (rc != FRAME_SIZE) {
//         throw std::runtime_error(
//             fmt::format("ERROR: Wrote {} with error {}", rc,
//             strerror(errno)));
//     }
// }
