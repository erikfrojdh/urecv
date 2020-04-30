#pragma once
#include <cstdint>
#include <cstddef>
struct ImageView {
    ImageView(){}
    ImageView(int64_t frameNumber, char* data):frameNumber(frameNumber), data(data){}
    // ImageView(const ImageView&) = default;
    // ImageView(ImageView&&) = default;
    // ImageView& operator=(const ImageView& other) = default;
    int64_t frameNumber{-1};
    size_t size{0};
    char *data{nullptr};
};
