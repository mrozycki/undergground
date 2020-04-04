#include "process.h"

#include <csignal>

#include <boost/algorithm/string.hpp>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace ugg {
namespace system {

bool process::kill() {
    if (::kill(pid_, SIGKILL)) {
        spdlog::warn("Failed to kill child process {}: {}", pid_, strerror(errno));
        return false;
    }

    return true;
}

std::future<process_result> process::exit_future() {
    return std::async(std::launch::async, [this]() {
        process_result result;

        int status;
        waitpid(pid_, &status, 0);
        if (WIFEXITED(status)) {
            result.exit_code = WEXITSTATUS(status);
            result.exit_status = result.exit_code ? exit_status::error : exit_status::success;
        } else if (WIFSIGNALED(status)) {
            result.exit_code = WTERMSIG(status);
            result.exit_status = exit_status::terminated;
        } else {
            result.exit_code = 0;
            result.exit_status = exit_status::error;
        }

        rusage usage;
        getrusage(RUSAGE_CHILDREN, &usage);
        result.time_taken = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000 +
            (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000;
        result.memory_usage = usage.ru_maxrss;

        return result;
    });
}

namespace {
char* copy(char const* const s) {
    char* result = new char[strlen(s) + 1];
    strcpy(result, s);
    return result;
}

char** build_arguments(boost::filesystem::path const& executable, std::vector<std::string_view> const& arguments) {
    char** native_arguments = new char*[arguments.size() + 2];
    auto next_argument = native_arguments;
    *(next_argument++) = copy(executable.filename().c_str());
    for (auto const& argument : arguments) {
        *(next_argument++) = copy(argument.data());
    }
    *(next_argument++) = nullptr;
    return native_arguments;
}

enum direction { OUT, IN };

void connect(int pipe[2], int direction, int fd) {
    close(pipe[1 - direction]);
    dup2(pipe[direction], fd);
}
} // namespace

process start_process(
    boost::filesystem::path const& executable,
    std::vector<std::string_view> arguments,
    rlim_t memory_limit_value,
    rlim_t process_limit_value) {
    int stdin_pipe[2], stdout_pipe[2], stderr_pipe[2], ready_pipe[2];
    ::pipe(stdin_pipe);
    ::pipe(stdout_pipe);
    ::pipe(stderr_pipe);
    ::pipe(ready_pipe);

    auto pid = fork();
    if (pid == 0) {
        // Kill this process when its parent dies
        prctl(PR_SET_PDEATHSIG, SIGKILL);

        rlimit memory_limit;
        memory_limit.rlim_cur = memory_limit_value;
        memory_limit.rlim_max = memory_limit_value;
        setrlimit(RLIMIT_STACK, &memory_limit);

        rlimit process_limit;
        process_limit.rlim_cur = process_limit_value;
        process_limit.rlim_max = process_limit_value;
        setrlimit(RLIMIT_NPROC, &process_limit);

        connect(stdin_pipe, direction::OUT, STDIN_FILENO);
        connect(stdout_pipe, direction::IN, STDOUT_FILENO);
        connect(stderr_pipe, direction::IN, STDERR_FILENO);
        close(ready_pipe[direction::OUT]);
        write(ready_pipe[direction::IN], "A", 2);
        close(ready_pipe[direction::IN]);

        execv(executable.c_str(), build_arguments(executable, arguments));
        exit(-1);
    } else {
        close(stdin_pipe[direction::OUT]);
        close(stdout_pipe[direction::IN]);
        close(stderr_pipe[direction::IN]);

        spdlog::info("Waiting for process to start");
        close(ready_pipe[direction::IN]);
        char c;
        read(ready_pipe[direction::OUT], &c, 1);
        close(ready_pipe[direction::OUT]);
        spdlog::info("Process started, returning");

        return process(pid, stdin_pipe[direction::IN], stdout_pipe[direction::OUT], stderr_pipe[direction::OUT]);
    }
}

} // namespace system
} // namespace ugg
