#pragma once
#include <string>
#include <cstddef>
void pin_this_thread(int i);
void set_realtime_priority();
void direct_input();
void reset_terminal();
struct Image {
    int64_t frameNumber = -1;
    std::byte *data = nullptr;
};
struct Args{
    std::string node;
    std::string port;
    std::string endpoint;
    std::string fname;
    int cpu0{0};
    int cpu1{1};
};
Args parse_args(int argc, char *argv[]);
