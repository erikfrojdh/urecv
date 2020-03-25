#include "utils.h"
#include <stdexcept>
#include <string_view>
#include <fmt/format.h>
std::pair<std::string, std::string> parse_args(int argc, char *argv[]) {
    std::pair<std::string, std::string> res;
    if (argc != 2) {
        throw std::runtime_error("Wrong number of arguments\n");
    }
    std::string_view arg(argv[1]);
    if (auto pos = arg.find(':'); pos != std::string_view::npos) {
        res.first = arg.substr(0, pos);
        res.second = arg.substr(pos + 1);
    }
    fmt::print("Node: {} port: {}\n", res.first, res.second);
    return res;
}