#pragma once

#include <string>
#include "defs.h"
#include "utils.h"
class FileWriterDirect {
    int fd_ = -1;
    int64_t *meta_;
    size_t n_written_ = 0;
    size_t frames_per_file_ = 1000;
    size_t file_nr_ = 0;
    size_t meta_size_;
    std::string basename_;
    void open(const std::string& fname);
    void allocate_meta();
    void write_meta();
  public:
    FileWriterDirect(const std::string &basename);
    ~FileWriterDirect();
    void write(const Image& img);
};