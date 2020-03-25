#include "utils.h"
#include "defs.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <string_view>
#include <thread>

Args parse_args(int argc, char *argv[]) {
    Args res;
    if (!(argc == 2 || argc == 3))
        throw std::runtime_error("Wrong number of arguments");
    std::string_view arg(argv[1]);
    if (auto pos = arg.find(':'); pos != std::string_view::npos) {
        res.node = arg.substr(0, pos);
        res.port = arg.substr(pos + 1);
    }
    res.endpoint = (argc == 3) ? argv[2] : DEFAULT_ENDPOINT;
    if (res.node.empty() || res.port.empty())
        throw std::runtime_error("Could not decode either hostname or port");
    return res;
}

void pin_this_thread(int i) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc)
        throw std::runtime_error("Could not pin thread");
}

void set_realtime_priority() {
    struct sched_param params {};
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &params);
    if (ret)
        fmt::print(fg(fmt::color::yellow),
                   "Warning could not set thread priority!\n");
}
