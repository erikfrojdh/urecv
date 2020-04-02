#include "utils.h"
#include "defs.h"
#include <fmt/color.h>
#include <fmt/format.h>
#include <stdexcept>
#include <string_view>
#include <thread>

#include <termios.h>
#include <unistd.h>

Args parse_args(int argc, char *argv[]) {
    Args res;
    if (argc < 2)
        throw std::runtime_error("Need at least one argument");
    std::string_view arg(argv[1]);
    if (auto pos = arg.find(':'); pos != std::string_view::npos) {
        res.node = arg.substr(0, pos);
        res.port = arg.substr(pos + 1);
    }
    if (argc == 4 || argc == 6) {
        if (std::string_view arg3(argv[2]); arg3 == "-s") {
            res.endpoint = argv[3];
        } else if (arg3 == "-w") {
            res.fname = argv[3];
        } else {
            res.cpu0 = std::stoi(argv[2]);
            res.cpu1 = std::stoi(argv[3]);
        }
    }
    if (argc == 6) {
        // cpu id
        res.cpu0 = std::stoi(argv[4]);
        res.cpu1 = std::stoi(argv[5]);
    }

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

void direct_input() {
    struct termios ctrl {};
    tcgetattr(STDIN_FILENO, &ctrl);
    ctrl.c_lflag &= ~ICANON;
    ctrl.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}

void reset_terminal() {
    struct termios ctrl {};
    tcgetattr(STDIN_FILENO, &ctrl);
    ctrl.c_lflag |= ICANON;
    ctrl.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ctrl);
}