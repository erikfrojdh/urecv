#pragma once

#include "FileWriter.h"
#include "defs.h"
#include "utils.h"
#include <string>
class FileWriterDirect final : public FileWriter {
    int fd_ = -1;
    void write_impl(void *buf, size_t len) override;
    void open_impl(const std::string &fname) override;
    void close_impl() override;

  public:
    FileWriterDirect(const std::string& basename);
    ~FileWriterDirect() override;
};