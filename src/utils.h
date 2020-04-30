#pragma once
#include <cstddef>
#include <string>
#include <algorithm>
void pin_this_thread(int i);
void set_realtime_priority();
void direct_input();
void reset_terminal();
// struct Image {
//     int64_t frameNumber = -1;
//     std::byte *data = nullptr;
// };
// struct Args{
//     std::string node;
//     std::string port;
//     std::string endpoint;
//     std::string fname;
// };
// Args parse_args(int argc, char *argv[]);

namespace urecv {
/** Compare elements in a Container to see if they are all equal */
template <typename Container> bool allEqual(const Container &c) {
    if (!c.empty() &&
        std::all_of(begin(c), end(c),
                    [c](const typename Container::value_type &element) {
                        return element == c.front();
                    }))
        return true;
    return false;
}
} // namespace urecv
