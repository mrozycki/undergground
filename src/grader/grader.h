#pragma once

#include <string_view>

#include <boost/filesystem.hpp>

namespace ugg {
int grade(std::string_view problem_id, boost::filesystem::path const& executable_path);
}