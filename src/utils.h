#pragma once
#include <string>
void pin_this_thread(int i);
void set_realtime_priority();
struct Image {
    int64_t frameNumber = -1;
    std::byte *data = nullptr;
};
struct Args{
    std::string node;
    std::string port;
    std::string endpoint;
};
Args parse_args(int argc, char *argv[]);