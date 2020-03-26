#pragma once
#include "utils.h"
#include <cstddef>
#include <string>
class Streamer {
    void *context{nullptr};
    void *socket{nullptr};

  public:
    Streamer(const std::string &endpoint);
    ~Streamer();
    void send(Image &img, size_t data_size);
};