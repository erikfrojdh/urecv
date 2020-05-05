#pragma once
#include "ImageView.h"
#include "utils.h"
#include <cstddef>
#include <string>
class ZmqSocket {
    void *context{nullptr};
    void *socket{nullptr};

  public:
    ZmqSocket(const std::string &endpoint);
    ~ZmqSocket();
    void send(ImageView &img, size_t data_size);
};