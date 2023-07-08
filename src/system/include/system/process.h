#pragma once

#include <chrono>
#include <filesystem>
#include <future>
#include <optional>
#include <vector>

#include <sys/resource.h>

#include "file.h"

namespace ugg {
namespace system {
enum class exit_status { success, error, terminated, killed };

struct process_limits {
    long max_process_count;
    long max_memory_usage;
    std::chrono::seconds max_time_taken;
};

constexpr process_limits DEFAULT_LIMITS = process_limits{1, 128 * 1024 * 1024, std::chrono::seconds(10)};

struct process_result {
    long memory_usage;
    std::chrono::milliseconds time_taken;
    exit_status status;
    int exit_code;
};

class process {
public:
    process(pid_t pid, int stdin_fd, int stdout_fd, int stderr_fd, std::chrono::seconds time_limit)
        : pid_(pid), in_(stdin_fd, "w"), out_(stdout_fd, "r"), err_(stderr_fd, "r"), time_limit_(time_limit) {}

    file& in() { return in_; }
    file& out() { return out_; }
    file& err() { return err_; }

    bool kill();
    std::future<process_result> exit_future();

private:
    pid_t pid_;
    file in_, out_, err_;
    std::chrono::seconds time_limit_;
};

std::optional<process> start_process(
    std::filesystem::path const& executable,
    std::vector<std::string_view> arguments = {},
    process_limits limits = DEFAULT_LIMITS);
} // namespace system
} // namespace ugg
