#pragma once

#include <future>

#include <boost/filesystem.hpp>

#include "file.h"

namespace ugg {
namespace system {
enum class exit_status {
    SUCCESS, ERROR
};

class process {
public:
    process(pid_t pid, int stdin_fd, int stdout_fd, int stderr_fd)
        : pid_(pid), in_(stdin_fd, "w"), out_(stdout_fd, "r"), err_(stderr_fd, "r") {}

    file& in() { return in_; }
    file& out() { return out_; }
    file& err() { return err_; }

    bool kill();
    std::future<exit_status> exit_future();

private:
    pid_t pid_;
    file in_, out_, err_;
};

process start_process(
    boost::filesystem::path const& executable,
    std::vector<std::string_view> arguments = {},
    int memory_limit_value = 134217728,
    int process_limit_value = 1);
}
}