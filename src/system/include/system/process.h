#pragma once

#include <future>
#include <optional>

#include <boost/filesystem.hpp>
#include <sys/resource.h>

#include "file.h"

namespace ugg {
namespace system {
enum class exit_status { success, error, terminated };

struct process_result {
    long memory_usage;
    long time_taken;
    exit_status status;
    int exit_code;
};

class process {
public:
    process(pid_t pid, int stdin_fd, int stdout_fd, int stderr_fd)
        : pid_(pid), in_(stdin_fd, "w"), out_(stdout_fd, "r"), err_(stderr_fd, "r") {}

    file& in() { return in_; }
    file& out() { return out_; }
    file& err() { return err_; }

    bool kill();
    std::future<process_result> exit_future();

private:
    pid_t pid_;
    file in_, out_, err_;
};

std::optional<process> start_process(
    boost::filesystem::path const& executable,
    std::vector<std::string_view> arguments = {},
    rlim_t process_limit_value = 0,
    rlim_t memory_limit_value = 128 * 1024 * 1024);
} // namespace system
} // namespace ugg
