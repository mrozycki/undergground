#pragma once

#include <future>

#include <boost/filesystem.hpp>

#include "system/process.h"

namespace ugg {
class io_handler {
public:
    io_handler(system::process& solution_process) : solution_process_(solution_process) {}

    std::future<void> feed(boost::filesystem::path const& input_path);
    std::future<bool> verify_output(boost::filesystem::path const& output_path);

private:
    system::process& solution_process_;
};
} // namespace ugg
