#pragma once
#include <cstddef>
#include <cstdint>
struct ImageView {
    // ImageView() {}
    // ImageView(int64_t frameNumber, char *data)
    //     : frameNumber(frameNumber), data(data) {}
    int64_t frameNumber{-1};
    char *data{nullptr};
};
