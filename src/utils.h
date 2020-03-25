#pragma once
#include <string>
std::pair<std::string, std::string> parse_args(int argc, char *argv[]);
void pin_this_thread(int i);
void set_realtime_priority();
struct Image {
    int64_t frameNumber = -1;
    std::byte *data = nullptr;
};